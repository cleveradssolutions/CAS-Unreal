import subprocess
import os
import sys

ENGINES = "/Users/stratos/Documents/Environment/UnrealEngine/"
RUN_UAT = "/Engine/Build/BatchFiles/RunUAT.sh"

THIRD_PARTY = os.path.dirname(os.path.abspath(__file__))
PROJECT_ROOT = os.path.dirname(THIRD_PARTY)
PLUGIN_PATH = PROJECT_ROOT + "/CleverAdsSolutions.uplugin"
OUTPUT_DIR = "/Users/stratos/Documents/Builds/CleverAdsSolutions"

def package_for_ue(version):
    output = OUTPUT_DIR + "_" + version
    cmd = [
        ENGINES + version + RUN_UAT,
        "BuildPlugin",
        f'-Plugin="{PLUGIN_PATH}"'
        f'-Package="{output}"'
    ]
    print("CMD: ", " ".join(cmd))
    subprocess.check_call(cmd)
    print("Package: " + output)

package_for_ue("UE_5.7")
