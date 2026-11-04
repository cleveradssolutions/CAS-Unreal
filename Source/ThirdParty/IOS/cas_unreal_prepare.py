#!/usr/bin/env python3
# Copyright CleverAdsSolutions, CAS.AI. All Rights Reserved.
#
# Prepares the iOS dependencies of the CAS plugin for an Unreal build.
#
# Downloads the frameworks of the selected adapters from the Swift Package Manager
# dependencies, arranges them the way Unreal expects, and writes CASXCConfig.temp, which
# describes the result to CleverAdsSolutions.Build.cs.
#
# The plugin runs this script by itself when the configuration changes. It can also be
# run by hand, before the build, so that the build has nothing left to prepare:
#
#   python3 cas_unreal_prepare.py --adapters OptimalAds
#
# Requires python3, swift and git. Xcode project and CocoaPods are not required.

import argparse
import json
import os
import plistlib
import shutil
import subprocess
import sys

SCRIPT_VERSION = "1.0"
BASE_TARGET = "CASBaseResources"
CONFIG_NAME = "CASXCConfig.temp"
REPORT_NAME = "CASFrameworks.json"
FETCH_SCRIPT = "cas_spm_setup.py"
MEDIATION_LIST = "CASMediation.list"

FRAMEWORKS_DIR = "Frameworks"
RESOURCES_DIR = "Resources"
CACHE_DIR = ".cas_spm_cache"

ROOT_DIR = os.path.dirname(os.path.abspath(__file__))


# MARK: - Logging


def log(message):
    if not args.silent:
        # Flushed, so that the output of this script and of the one it calls stay in the
        # order they happened in
        print(message, flush=True)


def log_step(message):
    log("\n==> " + message)


def log_info(message):
    log("    " + message)


def fail(message):
    print("[CAS.AI] Error: " + message, file=sys.stderr)
    sys.exit(1)


# MARK: - Mediation list


def read_mediation_list(path):
    if not os.path.isfile(path):
        fail("Mediation list not found: " + path)
    with open(path) as file:
        return json.load(file)


def package_candidates(adapter):
    """The names an adapter can be known by in the CAS Swift package.

    The mediation list keeps the name of the CocoaPods pod, and for most adapters the
    Swift package calls its product the same. Where it does not, the difference is the
    `CAS` prefix of the pod: the pod `CASMediationCASExchange` is the product
    `CASMediationExchange`.
    """
    names = [adapter["name"]]
    for lib in adapter.get("libs", []):
        name = lib["name"]
        if name.startswith("CASMediation"):
            names.append(name[len("CASMediation"):])
        else:
            names.append(name)

    result = []
    for name in names:
        for candidate in [name, name[3:] if name.startswith("CAS") else name]:
            if candidate and candidate not in result:
                result.append(candidate)
    return result


def list_package_adapters(version, cache_dir):
    """What the package of this version has, which is the only reliable source of the
    names: the mediation list is written for CocoaPods."""
    script = os.path.join(ROOT_DIR, FETCH_SCRIPT)
    report_path = os.path.join(cache_dir, "CASPackage.json")
    os.makedirs(cache_dir, exist_ok=True)

    command = [
        sys.executable, script, "list",
        "--version", version,
        "--cache", cache_dir,
        "--json", report_path,
        "--silent",
    ]
    if subprocess.call(command) != 0:
        fail("Cannot read the adapters of CAS " + version + ", see the output above")

    with open(report_path) as file:
        return json.load(file)["adapters"]


def extra_package_of(entry):
    """An SDK that is listed with the solutions but is not a part of the CAS package,
    such as the attribution SDK of Tenjin, says where its own Swift package is.

    It can also name the system frameworks it links, because a package that ships only a
    binary declares nothing for the linker and its CocoaPods spec was the only place that
    said so.
    """
    spm = entry.get("spm")
    if spm is None:
        return None
    url = spm.get("url")
    version = spm.get("version") or entry.get("version")
    products = spm.get("products") or [lib["name"] for lib in entry.get("libs", [])]
    if not url or not version or not products:
        fail(
            entry["name"] + " has an incomplete Swift package description in "
            + MEDIATION_LIST + ": a url, a version and the products are required"
        )
    return {
        "fetch": [url, version] + list(products),
        "sysFrameworks": spm.get("sysFrameworks", []),
        "sysWeakFrameworks": spm.get("sysWeakFrameworks", []),
        "sysLibs": spm.get("sysLibs", []),
    }


def resolve_adapters(mediation, selected, available):
    """Turns what the publisher selected into the adapter names of the package.

    A solution, such as OptimalAds, is a set of adapters rather than a dependency of its
    own: the list keeps the ids of what it contains, and every one of them is asked for
    separately.

    An adapter the package does not have is a build error when the publisher named it,
    and a warning when it came with a solution: a solution is a set of networks CAS puts
    together, and it carries whatever of them the package supports.
    """
    by_id = {adapter["id"]: adapter for adapter in mediation["adapters"]}
    by_name = {adapter["name"]: adapter for adapter in mediation["adapters"]}
    solutions = {solution["name"]: solution for solution in mediation["simple"]}
    known = {name.lower(): name for name in available}

    result = []
    packages = []
    missing_named = []
    missing_in_solutions = []

    def take(adapter, from_solution):
        for candidate in package_candidates(adapter):
            name = known.get(candidate.lower())
            if name is not None:
                if name not in result:
                    result.append(name)
                return
        if from_solution:
            missing_in_solutions.append(adapter["name"])
        else:
            missing_named.append(adapter["name"])

    for name in selected:
        adapter = by_name.get(name)
        if adapter is not None:
            take(adapter, False)
            continue

        solution = solutions.get(name)
        if solution is None:
            fail("Unknown name in the selection: " + name)

        contained = solution.get("contains")
        if not contained:
            # A third party SDK is listed among the solutions, and it is distributed by
            # its own vendor rather than as a part of the CAS package.
            package = extra_package_of(solution)
            if package is None:
                fail(
                    name + " is not a part of the CAS Swift package and " + MEDIATION_LIST
                    + " does not say where its own package is. Deselect it, or add that "
                    + "SDK to the project by itself."
                )
            log_info(name + " comes from its own package: " + package["fetch"][0])
            packages.append(package)
            continue

        for identifier in contained:
            adapter = by_id.get(identifier)
            if adapter is None:
                log_info(name + " contains unknown adapter " + str(identifier))
            else:
                take(adapter, True)

    if missing_named:
        fail(
            "The CAS Swift package has no adapter for: " + ", ".join(missing_named)
            + ". Deselect them, or ask CAS support when they are expected."
        )
    if missing_in_solutions:
        log_info(
            "Not in the Swift package and left out of the solution: "
            + ", ".join(sorted(set(missing_in_solutions)))
        )
    if not result:
        # Without a single ad source the build still needs the SDK itself
        log_info("No adapters selected, only the CAS framework is prepared")
        result = [BASE_TARGET]
    return result, packages


# MARK: - Fetch


def fetch_frameworks(version, adapters, packages, fetch_dir, cache_dir, report_path):
    script = os.path.join(ROOT_DIR, FETCH_SCRIPT)
    if not os.path.isfile(script):
        fail("Fetch script not found: " + script)

    command = [
        sys.executable,
        script,
        "fetch",
        "--version", version,
        "--output", fetch_dir,
        "--cache", cache_dir,
        "--json", report_path,
        "--platform", args.platform,
        "--retries", str(args.retries),
    ]
    for package in packages:
        command.append("--extra-package")
        command.extend(package["fetch"])
    if args.silent:
        command.append("--silent")
    command.append("--adapters")
    command.extend(adapters)

    log_step("Download the frameworks of: " + ", ".join(adapters))
    if subprocess.call(command) != 0:
        fail("Download of the frameworks failed, see the output above")

    with open(report_path) as file:
        return json.load(file)


# MARK: - Layout for Unreal


def find_device_slice(xcframework):
    """The slice built for a device. Unreal builds for a device, and the simulator slices
    of the same framework would collide with it under one name."""
    plist_path = os.path.join(xcframework, "Info.plist")
    if not os.path.isfile(plist_path):
        return None
    with open(plist_path, "rb") as file:
        plist = plistlib.load(file)

    for library in plist.get("AvailableLibraries", []):
        if library.get("SupportedPlatform") != "ios":
            continue
        if library.get("SupportedPlatformVariant"):
            continue
        return (
            os.path.join(xcframework, library["LibraryIdentifier"]),
            library["LibraryPath"],
        )
    return None


def place_framework(name, slice_dir, library, frameworks_dir):
    """Unreal takes a framework as a directory `<Name>/<Name>.framework`, so every
    framework gets a directory of its own, named after it.

    With the engine layout there is one more level, `<Name>.embeddedframework`, which is
    where Unreal Build Tool unpacks a framework archive of its own accord. The plugin
    build chooses the layout, see USE_ENGNIE_INTERMEDIATE in CleverAdsSolutions.Build.cs.
    """
    destination_dir = os.path.join(frameworks_dir, name)
    if args.layout == "engine":
        destination_dir = os.path.join(destination_dir, name + ".embeddedframework")
    destination = os.path.join(destination_dir, name + ".framework")
    os.makedirs(destination_dir, exist_ok=True)
    if os.path.exists(destination):
        shutil.rmtree(destination)

    source = os.path.join(slice_dir, library)
    if library.endswith(".framework"):
        shutil.copytree(source, destination, symlinks=True)
    else:
        # A static library is not a framework, and Unreal links it all the same when it
        # is named as the binary of one.
        os.makedirs(destination)
        shutil.copy2(source, os.path.join(destination, name))
    return destination_dir


def bundle_owner(bundle, names):
    """The framework a resource bundle belongs to, by its name. A bundle that belongs to
    no framework is copied to the resources of the application instead."""
    owners = [name for name in names if bundle.startswith(name)]
    if not owners:
        return None
    return max(owners, key=len)


def arrange(report, fetch_dir, frameworks_dir, resources_dir):
    """Moves what was downloaded into the layout the plugin build expects and describes
    what came out of it."""
    log_step("Arrange the frameworks for Unreal")

    shutil.rmtree(frameworks_dir, ignore_errors=True)
    shutil.rmtree(resources_dir, ignore_errors=True)
    os.makedirs(frameworks_dir, exist_ok=True)
    os.makedirs(resources_dir, exist_ok=True)

    linkage = {
        os.path.splitext(item["name"])[0]: item.get("linkage")
        for item in report["frameworks"]
    }

    frameworks = []
    placed = {}
    for item in report["frameworks"]:
        name = os.path.splitext(item["name"])[0]
        xcframework = os.path.join(fetch_dir, item["name"])
        found = find_device_slice(xcframework)
        if found is None:
            fail(
                item["name"] + " has no slice for a device. "
                + "The framework cannot be used in an Unreal build."
            )
        slice_dir, library = found
        placed[name] = place_framework(name, slice_dir, library, frameworks_dir)
        frameworks.append({
            "name": name,
            "bundle": "",
            # A dynamic framework is embedded in the application, a static one is linked
            "dynamic": linkage.get(name) == "dynamic",
        })
        log_info(name + ".framework" + (" (dynamic)" if linkage.get(name) == "dynamic" else ""))

    by_name = {item["name"]: item for item in frameworks}
    additional_resources = []
    for item in report["bundles"]:
        bundle = item["name"]
        owner = bundle_owner(bundle, list(by_name))
        source = os.path.join(fetch_dir, bundle)
        if owner is not None and not by_name[owner]["bundle"]:
            by_name[owner]["bundle"] = bundle
            destination = os.path.join(placed[owner], bundle)
            log_info(bundle + " of " + owner)
        else:
            additional_resources.append(bundle)
            destination = os.path.join(resources_dir, bundle)
            log_info(bundle + " as an application resource")
        if os.path.exists(destination):
            shutil.rmtree(destination)
        shutil.copytree(source, destination, symlinks=True)

    return frameworks, additional_resources


# MARK: - Config for the plugin build


def build_config(version, selected, resolved, packages, report, frameworks, resources):
    """Everything the plugin build needs to link what was prepared.

    The system dependencies come from the package manifests, and from the list for a
    package that declares none of its own.
    """
    system_frameworks = set(report.get("sysFrameworks", []))
    system_libs = set(report.get("sysLibs", []))
    # Swift Package Manager has no notion of a weakly linked framework, so the only ones
    # are those the list names
    weak_frameworks = set()
    for package in packages:
        system_frameworks.update(package["sysFrameworks"])
        system_libs.update(package["sysLibs"])
        weak_frameworks.update(package["sysWeakFrameworks"])
    weak_frameworks -= system_frameworks

    return {
        "Version": version,
        "Frameworks": frameworks,
        # What the publisher selected, which is what the plugin build compares against
        "Adapters": selected,
        "Resolved": resolved,
        "SysLibs": sorted(system_libs),
        "SysFrameworks": sorted(system_frameworks),
        "SysWeakFrameworks": sorted(weak_frameworks),
        "AdditionalResources": resources,
        "Script": SCRIPT_VERSION,
    }


def escape(value):
    return (
        str(value)
        .replace("&", "&amp;")
        .replace("<", "&lt;")
        .replace(">", "&gt;")
        .replace('"', "&quot;")
    )


def write_config_xml(config, path):
    """Unreal Engine 5 reads the config as XML."""
    lines = ['<?xml version="1.0" encoding="utf-8"?>']
    lines.append(
        '<XCodeConfig version="' + escape(config["Version"])
        # The build reads this to tell a config of its own making from one an older
        # plugin left behind, which describes a layout that no longer exists
        + '" script="' + escape(config["Script"]) + '">'
    )
    lines.append("  <bundles>")
    for framework in config["Frameworks"]:
        lines.append(
            '    <bundle name="' + escape(framework["name"])
            + '" bundle="' + escape(framework["bundle"])
            + '" dynamic="' + ("true" if framework["dynamic"] else "false")
            + '" />'
        )
    lines.append("  </bundles>")

    for element, item, values in [
        ("adapters", "adapter", config["Adapters"]),
        ("resolved", "adapter", config["Resolved"]),
        ("sysLibs", "lib", config["SysLibs"]),
        ("sysFrameworks", "lib", config["SysFrameworks"]),
        ("sysWeakFrameworks", "lib", config["SysWeakFrameworks"]),
        ("additRes", "res", config["AdditionalResources"]),
    ]:
        lines.append("  <" + element + ">")
        for value in values:
            lines.append("    <" + item + ">" + escape(value) + "</" + item + ">")
        lines.append("  </" + element + ">")

    lines.append("</XCodeConfig>")
    with open(path, "w") as file:
        file.write("\n".join(lines) + "\n")


def write_config_json(config, path):
    """Unreal Engine 4 reads the config as JSON."""
    with open(path, "w") as file:
        json.dump(config, file, indent=2)


# MARK: - Run


def main():
    mediation_path = args.list or os.path.join(ROOT_DIR, MEDIATION_LIST)
    mediation = read_mediation_list(mediation_path)
    version = args.version or mediation["version"]

    root = os.path.abspath(args.output or ROOT_DIR)
    fetch_dir = os.path.join(root, "Temp", "Frameworks")
    cache_dir = os.path.abspath(args.cache or os.path.join(root, CACHE_DIR))
    report_path = os.path.join(root, REPORT_NAME)

    log_step("CAS " + version + " for Unreal")
    available = list_package_adapters(version, cache_dir)
    resolved, packages = resolve_adapters(mediation, args.adapters, available)

    if args.dry_run:
        log_step("Would prepare " + str(len(resolved)) + " adapters")
        for name in resolved:
            log_info(name)
        for package in packages:
            log_info("and the package " + " ".join(package["fetch"][:2]))
        return

    report = fetch_frameworks(
        version, resolved, packages, fetch_dir, cache_dir, report_path
    )

    frameworks, resources = arrange(
        report,
        fetch_dir,
        os.path.abspath(args.frameworks_dir or os.path.join(root, FRAMEWORKS_DIR)),
        os.path.join(root, RESOURCES_DIR),
    )

    config = build_config(
        version, args.adapters, resolved, packages, report, frameworks, resources
    )
    config_path = os.path.join(root, CONFIG_NAME)
    if args.format == "xml":
        write_config_xml(config, config_path)
    else:
        write_config_json(config, config_path)

    shutil.rmtree(os.path.join(root, "Temp"), ignore_errors=True)
    if not args.keep_cache:
        shutil.rmtree(cache_dir, ignore_errors=True)

    log_step(
        "Done: " + str(len(frameworks)) + " frameworks, "
        + str(len(resources)) + " application resources"
    )
    log_info("Config: " + config_path)
    for warning in report.get("warnings", []):
        log_info("Warning: " + warning)


arg_parser = argparse.ArgumentParser(
    prog="python3 cas_unreal_prepare.py",
    description=(
        "Prepares the iOS frameworks of the CAS plugin for an Unreal build, from the "
        "Swift Package Manager dependencies."
    ),
    epilog="Powered by CAS.AI, script version " + SCRIPT_VERSION,
)
arg_parser.add_argument(
    "--adapters", nargs="*", default=[],
    help=(
        "What to include: adapter names, or a solution such as OptimalAds. "
        "Without it only the CAS framework is prepared."
    ),
)
arg_parser.add_argument("--version", help="Version of the CAS SDK. Default: the version of " + MEDIATION_LIST)
arg_parser.add_argument("--list", help="Path to " + MEDIATION_LIST + ". Default: next to this script")
arg_parser.add_argument("--output", help="Directory of the result. Default: next to this script")
arg_parser.add_argument("--format", choices=["xml", "json"], default="xml", help=(
    "Format of " + CONFIG_NAME + ": xml for Unreal Engine 5, json for Unreal Engine 4. Default: xml"
))
arg_parser.add_argument("--platform", default="15", help="Minimum iOS version. Default: 15")
arg_parser.add_argument("--frameworks-dir", help=(
    "Where the frameworks go. Default: the Frameworks directory next to this script"
))
arg_parser.add_argument("--layout", choices=["plugin", "engine"], default="plugin", help=(
    "plugin: <dir>/<Name>/<Name>.framework, which is what the plugin build expects. "
    "engine: <dir>/<Name>/<Name>.embeddedframework/<Name>.framework, the shape Unreal "
    "Build Tool unpacks a framework archive into. Default: plugin"
))
arg_parser.add_argument("--cache", help="Directory to keep the downloads between the runs")
arg_parser.add_argument("--keep-cache", action="store_true", help="Do not remove the downloads afterwards")
arg_parser.add_argument("--retries", type=int, default=1, help="Repeat the download on a network error. Default: 1")
arg_parser.add_argument("--silent", action="store_true", help="Print nothing but the errors")
arg_parser.add_argument("--dry-run", action="store_true", help=(
    "Print what the selection resolves to in the package and stop, without downloading"
))

if __name__ == "__main__":
    args = arg_parser.parse_args()
    main()
