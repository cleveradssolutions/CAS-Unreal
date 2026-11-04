#!/bin/bash
# Compiles CleverAdsSolutions.Build.cs outside the engine and runs the parts of it that
# can run on their own. Needs mono: `brew install mono`.
#
#   ./check.sh                      compiles both engine branches
#   ./check.sh <CASXCConfig.temp>   also reads that config with the branch it was written
#                                   for, and decides whether a build would reuse it
#
# What a clean run means: the module's own code compiles and its config handling works.
# What it does not mean: the stubs here are an approximation of the Unreal Build Tool API,
# so they cannot prove that the engine accepts the module. Only a real build does that.
set -e
cd "$(dirname "$0")"

MODULE="../../CleverAdsSolutions/CleverAdsSolutions.Build.cs"
UE5="-define:UE_5_0_OR_LATER -define:UE_5_2_OR_LATER -define:UE_5_3_OR_LATER"
OUT="$(mktemp -d)"

# The config is xml for Unreal Engine 5 and json for Unreal Engine 4, so it is read by
# the branch it was written for
GIVEN_FORMAT=""
if [ -n "$1" ]; then
    case "$(head -c 1 "$1")" in
        "<") GIVEN_FORMAT="xml" ;;
        "{") GIVEN_FORMAT="json" ;;
        *) echo "Unknown format of $1"; exit 1 ;;
    esac
fi

for BRANCH in "Unreal Engine 5" "Unreal Engine 4"; do
    if [ "$BRANCH" = "Unreal Engine 5" ]; then DEFINES="$UE5"; FORMAT="xml"; else DEFINES=""; FORMAT="json"; fi
    echo "==> $BRANCH"

    csc -nologo $DEFINES -r:System.Xml.Linq.dll -out:"$OUT/read.exe" \
        UbtStubs.cs CoreStubs.cs "$MODULE" ReadConfig.cs
    csc -nologo $DEFINES -r:System.Xml.Linq.dll -out:"$OUT/reuse.exe" \
        UbtStubs.cs CoreStubs.cs "$MODULE" ReuseConfig.cs
    csc -nologo $DEFINES -r:System.Xml.Linq.dll -out:"$OUT/apply.exe" \
        UbtStubs.cs CoreStubs.cs "$MODULE" ApplyConfig.cs
    echo "    compiles"
    mono "$OUT/apply.exe"

    if [ "$GIVEN_FORMAT" = "$FORMAT" ]; then
        CONFIG_DIR="$OUT/$FORMAT"
        mkdir -p "$CONFIG_DIR"
        cp "$1" "$CONFIG_DIR/CASXCConfig.temp"
        mono "$OUT/read.exe" "$1"
        # Reuse is decided by the frameworks being in place, so the config has to be read
        # where it was written, next to them
        if [ -d "$(dirname "$1")/Frameworks" ]; then
            mono "$OUT/reuse.exe" "$CONFIG_DIR" "$(dirname "$1")"
        else
            echo "    skipped the reuse check: no Frameworks next to the config"
        fi
    fi
done

rm -rf "$OUT"
echo "==> Done"
