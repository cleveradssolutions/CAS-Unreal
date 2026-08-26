import subprocess
import os
import sys
import shutil

ENGINES = "/Users/stratos/Documents/Environment/UnrealEngine/"
RUN_UAT = "/Engine/Build/BatchFiles/RunUAT.sh"
THIRD_PARTY = os.path.dirname(os.path.abspath(__file__))
PROJECT_ROOT = os.path.dirname(os.path.dirname(THIRD_PARTY))
PLUGIN_PATH = PROJECT_ROOT + "/CleverAdsSolutions.uplugin"
OUTPUT_DIR = "/Users/stratos/Documents/Builds/CleverAdsSolutions"

ANDROID_SDK = "/Users/stratos/Documents/Environment/AndroidSDK"
ANDROID_NDK_VERSION = "27.2.12479018"
JAVA_HOME = "/Applications/Android Studio.app/Contents/jbr"

def package_for_ue(version, platforms=None):
    cmd = [
        ENGINES + version + RUN_UAT,
        "BuildPlugin",
        f"-Plugin={PLUGIN_PATH}",
        f"-Package={OUTPUT_DIR}",
    ]
    if platforms:
        cmd.append(f"-TargetPlatforms={'+'.join(platforms)}")

    env = os.environ.copy()
    env["ANDROID_HOME"] = ANDROID_SDK
    env["NDKROOT"] = f"{ANDROID_SDK}/ndk/{ANDROID_NDK_VERSION}"
    env["JAVA_HOME"] = JAVA_HOME

    if os.path.exists(OUTPUT_DIR):
        shutil.rmtree(OUTPUT_DIR)
    print("CMD: ", " ".join(cmd))
    subprocess.check_call(cmd, env=env)
    output = OUTPUT_DIR + "_" + version
    shutil.make_archive(output, "zip", OUTPUT_DIR)
    print("Package: " + output + ".zip")

package_for_ue("UE_5.7", platforms=["Mac", "IOS", "Android"])
package_for_ue("UE_5.8", platforms=["Mac", "IOS", "Android"])
