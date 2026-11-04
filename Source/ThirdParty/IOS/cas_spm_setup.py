#!/usr/bin/env python3
"""
Downloads the XCFrameworks and the resource bundles of the CAS adapters
from the Swift Package Manager dependencies, without Xcode and without
an Xcode project. Designed to be called as a process, for example from C#.

    python3 cas_spm_setup.py fetch --adapters GoogleAds UnityAds --version 4.9.0-beta.2

The result is a single directory with every XCFramework and `.bundle`
required by the selected adapters, and a JSON report of what was placed there.
Only the Swift toolchain (`swift`) and `git` are required.
"""

import argparse
import json
import os
import shutil
import subprocess
import sys
import tempfile

CAS_REPO_URL = "https://github.com/cleveradssolutions/CAS-iOS.git"
CAS_PACKAGE_NAME = "CAS-iOS"
ADAPTER_TARGET_PREFIX = "CASMediation"
ADAPTER_TARGET_SUFFIX = "Target"
SCRIPT_VERSION = "1.0"

IS_TTY = sys.stdout.isatty()


def log(message, color=""):
    if not args.silent:
        print((color + message + "\033[0m") if IS_TTY and color else message, flush=True)


def log_step(message):
    log("\n==> " + message, "\033[95m")


def log_info(message):
    log("    " + message, "\033[96m")


def log_warning(message):
    warnings.append(message)
    log("    Warning: " + message, "\033[93m")


def fail(message):
    print("Error: " + message, file=sys.stderr, flush=True)
    write_report({"script": SCRIPT_VERSION, "error": message, "warnings": warnings})
    sys.exit(1)


def run(command, cwd=None, capture=True):
    """Runs a command and fails the script when the command fails."""
    result = subprocess.run(
        command,
        cwd=cwd,
        stdout=subprocess.PIPE if capture else None,
        stderr=subprocess.PIPE if capture else None,
        text=True,
    )
    if result.returncode != 0:
        details = (result.stderr or result.stdout or "").strip()
        fail(" ".join(command) + " failed:\n" + details)
    return result.stdout or ""


# MARK: - Package manifest


def clone_cas_package(version, work_dir):
    """Clones the manifest of the CAS package. The binaries are in the releases,
    so the repository itself is small."""
    package_dir = os.path.join(work_dir, CAS_PACKAGE_NAME)
    if os.path.isdir(os.path.join(package_dir, ".git")):
        log_info("Package of version " + version + " is already cloned")
        return package_dir

    # The releases are tagged both as `4.9.0-beta.2` and `4.9.0-beta2`
    tags = [version]
    if "-" in version:
        release, pre_release = version.split("-", 1)
        alternative = release + "-" + pre_release.replace(".", "")
        if alternative != version:
            tags.append(alternative)

    for tag in tags:
        log_info("Clone " + CAS_REPO_URL + " at " + tag)
        result = subprocess.run(
            ["git", "clone", "--depth", "1", "--branch", tag, CAS_REPO_URL, package_dir],
            stdout=subprocess.PIPE,
            stderr=subprocess.PIPE,
            text=True,
        )
        if result.returncode == 0:
            return package_dir
        shutil.rmtree(package_dir, ignore_errors=True)

    fail("Version " + version + " not found in " + CAS_REPO_URL)


def dump_package(package_dir):
    log_info("Read the package manifest")
    return json.loads(run(["swift", "package", "dump-package", "--package-path", package_dir]))


def find_dependencies(manifest):
    """Package name -> (url, version requirement)"""
    result = {}
    for dependency in manifest.get("dependencies", []):
        for source in dependency.get("sourceControl", []):
            url = source["location"]["remote"][0]["urlString"]
            name = source.get("identity") or url.rstrip("/").split("/")[-1]
            if name.endswith(".git"):
                name = name[: -len(".git")]
            requirement = source.get("requirement", {})
            exact = requirement.get("exact", [None])[0]
            result[name.lower()] = (url, exact)
    return result


def collect_link_settings(target, frameworks, libraries):
    """System frameworks and libraries a target asks the linker for. CocoaPods used to
    report the same set through OTHER_LDFLAGS; with Swift Package Manager it is declared
    in the manifest. Weak linking has no equivalent there, so everything is reported as
    required."""
    for setting in target.get("settings", []):
        if setting.get("tool") != "linker":
            continue
        kind = setting.get("kind", {})
        if "linkedFramework" in kind:
            frameworks.add(kind["linkedFramework"]["_0"])
        elif "linkedLibrary" in kind:
            libraries.add(kind["linkedLibrary"]["_0"])


def find_adapter_target(targets, adapter):
    """The adapter can be named `GoogleAds`, `CASMediationGoogleAds` or with a target suffix."""
    # The target with the suffix keeps the dependencies of the adapter,
    # the target without it is the binary of the adapter alone.
    names = [
        ADAPTER_TARGET_PREFIX + adapter + ADAPTER_TARGET_SUFFIX,
        adapter + ADAPTER_TARGET_SUFFIX,
        ADAPTER_TARGET_PREFIX + adapter,
        adapter,
    ]
    for name in names:
        for target in targets:
            if target["name"].lower() == name.lower():
                return target
    return None


def collect_requirements(manifest, adapters):
    """Walks the targets of the selected adapters and collects
    the binary targets of CAS and the products of the Ad Source packages."""
    targets = {target["name"]: target for target in manifest["targets"]}
    binaries = {}
    products = {}
    system = (set(), set())
    visited = set()
    queue = []

    for adapter in adapters:
        target = find_adapter_target(manifest["targets"], adapter)
        if target is None:
            fail(
                "Adapter '" + adapter + "' not found in the package. "
                + "Use the `list` mode to print the available adapters."
            )
        queue.append(target["name"])

    while queue:
        name = queue.pop()
        if name in visited:
            continue
        visited.add(name)

        target = targets.get(name)
        if target is None:
            continue

        collect_link_settings(target, system[0], system[1])

        if target.get("type") == "binary":
            url = target.get("url")
            if url:
                binaries[name] = (url, target.get("checksum"))
            else:
                log_warning("Binary target " + name + " has no url and is skipped")
            continue

        for dependency in target.get("dependencies", []):
            if "target" in dependency:
                queue.append(dependency["target"][0])
            elif "byName" in dependency:
                queue.append(dependency["byName"][0])
            elif "product" in dependency:
                product_name = dependency["product"][0]
                package_name = dependency["product"][1]
                if package_name:
                    products.setdefault(package_name.lower(), set()).add(product_name)

    return binaries, products, system


# MARK: - Fetch


def platform_of(version):
    """`15` is the enum case `.v15`, while `15.1` has no case of its own and is written
    as a string. Both forms are what a caller may have at hand."""
    version = version.strip()
    if "." in version:
        major, _, minor = version.partition(".")
        if minor.strip("0") == "":
            return ".iOS(.v" + major + ")"
        return '.iOS("' + version + '")'
    return ".iOS(.v" + version + ")"


def write_fetch_package(work_dir, binaries, products, dependencies, platform):
    lines = [
        "// swift-tools-version: 6.0",
        "import PackageDescription",
        "",
        "let package = Package(",
        "    name: \"CASFrameworks\",",
        "    platforms: [" + platform_of(platform) + "],",
        "    dependencies: [",
    ]
    used_packages = []
    for package_name in sorted(products):
        if package_name not in dependencies:
            log_warning("Package " + package_name + " not found in the manifest dependencies")
            continue
        url, exact = dependencies[package_name]
        used_packages.append(package_name)
        if exact:
            lines.append("        .package(url: \"" + url + "\", exact: \"" + exact + "\"),")
        else:
            lines.append("        .package(url: \"" + url + "\", branch: \"master\"),")
    lines.append("    ],")
    lines.append("    targets: [")

    dependency_lines = []
    for name, (url, checksum) in sorted(binaries.items()):
        lines.append("        .binaryTarget(")
        lines.append("            name: \"" + name + "\",")
        lines.append("            url: \"" + url + "\",")
        lines.append("            checksum: \"" + checksum + "\"")
        lines.append("        ),")
        dependency_lines.append("                .target(name: \"" + name + "\"),")

    for package_name in used_packages:
        for product in sorted(products[package_name]):
            url = dependencies[package_name][0]
            repository = url.rstrip("/").split("/")[-1]
            if repository.endswith(".git"):
                repository = repository[: -len(".git")]
            dependency_lines.append(
                "                .product(name: \"" + product + "\", package: \"" + repository + "\"),"
            )

    lines.append("        .target(")
    lines.append("            name: \"CASFrameworks\",")
    lines.append("            dependencies: [")
    lines.extend(dependency_lines)
    lines.append("            ],")
    lines.append("            path: \"Sources\"")
    lines.append("        ),")
    lines.append("    ]")
    lines.append(")")

    os.makedirs(os.path.join(work_dir, "Sources"), exist_ok=True)
    with open(os.path.join(work_dir, "Sources", "Empty.swift"), "w") as file:
        file.write("// The package is used to download the dependencies only.\n")
    manifest_path = os.path.join(work_dir, "Package.swift")
    with open(manifest_path, "w") as file:
        file.write("\n".join(lines) + "\n")
    return manifest_path


def resolve_package(work_dir):
    """Downloads the sources and the binary artifacts of the dependencies.
    A single unavailable artifact fails the whole resolve, so the run is repeated:
    the artifacts downloaded before are taken from the cache."""
    log_step("Download the dependencies, it may take a while")
    command = ["swift", "package", "resolve", "--package-path", work_dir]

    for attempt in range(1, args.retries + 2):
        output = []
        process = subprocess.Popen(
            command, stdout=subprocess.PIPE, stderr=subprocess.STDOUT, text=True
        )
        for line in process.stdout:
            output.append(line)
            if not args.silent:
                sys.stdout.write(line)
        if process.wait() == 0:
            return

        failed = find_failed_downloads(output)
        if attempt <= args.retries:
            log_warning("Download attempt " + str(attempt) + " failed, trying again")
            continue

        fail(
            "Download failed for: " + (", ".join(failed) if failed else "see the output above")
        )


def find_failed_downloads(output):
    """Names of the binary targets that Swift Package Manager could not download."""
    result = []
    for line in output:
        if "failed downloading" in line or "failed extracting" in line:
            marker = "required by binary target '"
            if marker in line:
                name = line.split(marker, 1)[1].split("'", 1)[0]
                if name not in result:
                    result.append(name)
    return result


# MARK: - Collect the result


def is_inside_bundle(path):
    parent = os.path.dirname(path)
    while len(parent) > 1:
        if parent.endswith(".xcframework") or parent.endswith(".framework"):
            return True
        parent = os.path.dirname(parent)
    return False


def collect_artifacts(work_dir, output_dir, extensions):
    found = {}
    for root_name in ["artifacts", "checkouts"]:
        root = os.path.join(work_dir, ".build", root_name)
        if not os.path.isdir(root):
            continue
        for current, directories, _ in os.walk(root):
            # Swift Package Manager unpacks the archives in a temporary directory
            if os.path.basename(current) == "artifacts" and "extract" in directories:
                directories.remove("extract")
            for name in list(directories):
                if not name.endswith(extensions):
                    continue
                path = os.path.join(current, name)
                if is_inside_bundle(path):
                    continue
                directories.remove(name)
                if name in found:
                    if os.path.realpath(found[name]) != os.path.realpath(path):
                        log_warning("Duplicate of " + name + " is skipped: " + path)
                    continue
                found[name] = path

    os.makedirs(output_dir, exist_ok=True)
    for name in sorted(found):
        destination = os.path.join(output_dir, name)
        if os.path.exists(destination):
            shutil.rmtree(destination)
        shutil.copytree(found[name], destination, symlinks=True)
    return sorted(found)


def describe_framework(path):
    """A dynamic framework is embedded in the application, a static one is linked only."""
    binary = None
    for current, _, files in os.walk(path):
        if not current.endswith(".framework"):
            continue
        name = os.path.basename(current)[: -len(".framework")]
        if name in files:
            binary = os.path.join(current, name)
            break
    if binary is None:
        for current, _, files in os.walk(path):
            for name in files:
                if name.endswith(".a"):
                    return "static"
        return "unknown"

    kind = subprocess.run(
        ["file", "-b", binary], stdout=subprocess.PIPE, text=True
    ).stdout
    return "dynamic" if "dynamically linked" in kind else "static"


def measure(path):
    total = 0
    for current, _, files in os.walk(path):
        for name in files:
            file_path = os.path.join(current, name)
            if not os.path.islink(file_path):
                total += os.path.getsize(file_path)
    return total


def describe_result(output_dir, names, with_linkage):
    result = []
    for name in names:
        path = os.path.join(output_dir, name)
        item = {"name": name, "size": measure(path)}
        if with_linkage:
            item["linkage"] = describe_framework(path)
        result.append(item)
    return result


def collect_external_link_settings(work_dir, products, frameworks, libraries):
    """The package of an Ad Source declares the system dependencies of its own SDK.
    A package is recognised by the products that were asked of it, because the name of
    the directory Swift Package Manager checks it out into is not the package identity."""
    wanted = set()
    for names in products.values():
        wanted.update(names)

    checkouts = os.path.join(work_dir, ".build", "checkouts")
    if not os.path.isdir(checkouts):
        return

    for name in sorted(os.listdir(checkouts)):
        package_dir = os.path.join(checkouts, name)
        if not os.path.isfile(os.path.join(package_dir, "Package.swift")):
            continue

        dump = subprocess.run(
            ["swift", "package", "dump-package", "--package-path", package_dir],
            stdout=subprocess.PIPE,
            stderr=subprocess.PIPE,
            text=True,
        )
        if dump.returncode != 0:
            log_warning("Cannot read the manifest of " + name)
            continue

        manifest = json.loads(dump.stdout)
        targets = {target["name"]: target for target in manifest.get("targets", [])}

        queue = []
        for product in manifest.get("products", []):
            if product["name"] in wanted:
                queue.extend(product.get("targets", []))

        visited = set()
        while queue:
            target_name = queue.pop()
            if target_name in visited:
                continue
            visited.add(target_name)
            target = targets.get(target_name)
            if target is None:
                continue
            collect_link_settings(target, frameworks, libraries)
            for dependency in target.get("dependencies", []):
                if "target" in dependency:
                    queue.append(dependency["target"][0])
                elif "byName" in dependency:
                    queue.append(dependency["byName"][0])


def find_source_packages(work_dir):
    """The packages that ship sources instead of a binary require a build,
    so the frameworks of those packages cannot be downloaded."""
    result = []
    checkouts = os.path.join(work_dir, ".build", "checkouts")
    if not os.path.isdir(checkouts):
        return result

    for name in sorted(os.listdir(checkouts)):
        package_dir = os.path.join(checkouts, name)
        if not os.path.isfile(os.path.join(package_dir, "Package.swift")):
            continue
        dump = subprocess.run(
            ["swift", "package", "dump-package", "--package-path", package_dir],
            stdout=subprocess.PIPE,
            stderr=subprocess.PIPE,
            text=True,
        )
        if dump.returncode != 0:
            continue
        manifest = json.loads(dump.stdout)
        package_name = manifest.get("name", name)
        targets = manifest.get("targets", [])
        has_binary = any(target.get("type") == "binary" for target in targets)
        has_sources = any(target.get("type") == "regular" for target in targets)

        for target in targets:
            if target.get("type") == "regular" and target.get("resources"):
                log_warning(
                    "Target " + target["name"] + " of the package " + package_name
                    + " keeps resources in sources, the bundle "
                    + package_name + "_" + target["name"] + ".bundle requires a build"
                )

        if not has_binary and has_sources:
            result.append(package_name)
    return result


# MARK: - Modes


def mode_list():
    work_dir = prepare_work_dir()
    manifest = dump_package(clone_cas_package(args.version, work_dir))
    adapters = []
    for target in manifest["targets"]:
        name = target["name"]
        if name.startswith(ADAPTER_TARGET_PREFIX) and name.endswith(ADAPTER_TARGET_SUFFIX):
            adapters.append(name[len(ADAPTER_TARGET_PREFIX): -len(ADAPTER_TARGET_SUFFIX)])
    adapters.sort()
    if args.json:
        write_report({"version": args.version, "adapters": adapters})
    log_step("Adapters of CAS " + args.version)
    for adapter in adapters:
        log_info(adapter)


def mode_fetch():
    work_dir = prepare_work_dir()
    package_dir = clone_cas_package(args.version, work_dir)
    manifest = dump_package(package_dir)

    log_step("Resolve the adapters: " + ", ".join(args.adapters))
    binaries, products, system = collect_requirements(manifest, args.adapters)
    log_info(str(len(binaries)) + " CAS frameworks, " + str(len(products)) + " Ad Source packages")

    fetch_dir = os.path.join(work_dir, "fetch")
    os.makedirs(fetch_dir, exist_ok=True)
    write_fetch_package(
        fetch_dir, binaries, products, find_dependencies(manifest), args.platform
    )
    resolve_package(fetch_dir)

    log_step("Copy the frameworks to " + args.output)
    output_dir = os.path.abspath(args.output)
    frameworks = collect_artifacts(fetch_dir, output_dir, (".xcframework",))
    bundles = collect_artifacts(fetch_dir, output_dir, (".bundle",))
    for name in frameworks:
        log_info(name + " (" + describe_framework(os.path.join(output_dir, name)) + ")")
    for name in bundles:
        log_info(name)

    collect_external_link_settings(fetch_dir, products, system[0], system[1])
    log_info(
        str(len(system[0])) + " system frameworks, "
        + str(len(system[1])) + " system libraries"
    )

    for package in find_source_packages(fetch_dir):
        log_warning(
            "Package " + package + " ships sources without a binary, "
            + "its framework requires a build and is not downloaded"
        )

    report = {
        "script": SCRIPT_VERSION,
        "version": args.version,
        "adapters": args.adapters,
        "output": output_dir,
        "frameworks": describe_result(output_dir, frameworks, True),
        "bundles": describe_result(output_dir, bundles, False),
        "sysFrameworks": sorted(system[0]),
        "sysLibs": sorted(system[1]),
        "warnings": warnings,
    }
    write_report(report)
    log_step("Done: " + str(len(frameworks)) + " frameworks, " + str(len(bundles)) + " bundles")


def prepare_work_dir():
    if args.cache:
        path = os.path.abspath(args.cache)
        os.makedirs(path, exist_ok=True)
        return path
    global temp_dir
    if temp_dir is None:
        temp_dir = tempfile.mkdtemp(prefix="cas_spm_")
    return temp_dir


def write_report(report):
    if not args.json:
        return
    with open(args.json, "w") as file:
        json.dump(report, file, indent=2)


# MARK: - Arguments

arg_parser = argparse.ArgumentParser(
    prog="python3 cas_spm_setup.py",
    description=(
        "Downloads the XCFrameworks and the resource bundles of the CAS adapters "
        "from the Swift Package Manager dependencies, without Xcode."
    ),
    epilog="Powered by CAS.AI, script version " + SCRIPT_VERSION,
)
arg_parser.add_argument("mode", choices=["fetch", "list"], help=(
    "[fetch] - Download the frameworks of the selected adapters. "
    "[list] - Print the adapters available in the version."
))
arg_parser.add_argument("--version", required=True, help="Version of the CAS SDK, for example 4.9.0-beta.2")
arg_parser.add_argument("--adapters", nargs="*", default=[], help="Names of the adapters, for example GoogleAds UnityAds")
arg_parser.add_argument("--output", default="Frameworks", help="Directory for the result. Default: Frameworks")
arg_parser.add_argument("--platform", default="15", help="Minimum iOS version of the package. Default: 15")
arg_parser.add_argument("--cache", help="Directory to keep the downloads between the runs")
arg_parser.add_argument("--json", help="Path to write the JSON report of the result")
arg_parser.add_argument("--retries", type=int, default=1, help="Repeat the download on a network error. Default: 1")
arg_parser.add_argument("--silent", action="store_true", help="Print nothing but the errors")

args = arg_parser.parse_args()
warnings = []
temp_dir = None

if args.mode == "fetch" and not args.adapters:
    fail("Select at least one adapter with the --adapters argument")

try:
    if args.mode == "list":
        mode_list()
    else:
        mode_fetch()
finally:
    if temp_dir and not args.cache:
        shutil.rmtree(temp_dir, ignore_errors=True)
