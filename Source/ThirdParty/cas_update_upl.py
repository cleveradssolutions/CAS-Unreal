# Copyright CleverAdsSolutions, CAS.AI. All Rights Reserved.
#
# This script created for Internal use only.
# Automaticaly update plugin components for Android and iOS platforms.
# 

import os
import re
import sys
import json
import shutil

_CONFIG_SECTION = '/Script/CleverAdsSolutions.CASDefaultConfig'
_DEFAULT_GOOGLE_APP_ANDROID = 'ca-app-pub-3940256099942544~3347511713'
_DEFAULT_GOOGLE_APP_IOS = 'ca-app-pub-3940256099942544~1458002511'

def read_mediation_list_for(platform):
    with open(os.path.join(rootDir, platform, 'CAS' + platform + 'Mediation.list'), 'r') as file:
        return json.load(file)

def applyDynamicConfigDefaults(googleAppId, uplFile):
    uplFile.append('\t\t<setString result="GoogleAppId" value="' + googleAppId + '"/>\n')
    uplFile.append('\t\t<setString result="CASConfigResFile" value="cas_settings.json"/>\n')

def applyAndroidConfig(mediationArray, uplFile):
    for item in mediationArray:
        itemName = 'Include' + item['name']
        uplFile.append(
            '\t\t<setBoolFromProperty result="' + itemName + 
            '" ini="Engine" section="' + 
            _CONFIG_SECTION+'" property="' + itemName + 
            '" default="false"/>\n'
        )
        print("[Android] Add init " + itemName)

def applyAndroidDependencyItem(line, uplFile):
    uplFile.append('\t\t<insertValue value="' + line + '" /> <insertNewline/>\n')

def applyAndroidDependency(mediationArray, uplFile):
    for item in mediationArray:
        itemName = 'Include' + item['name']
        uplFile.append('\t\t<if condition="' + itemName + '"><true>\n')
        applyAndroidDependencyItem(item['dependency'].replace(':', ',') + item['version'], uplFile)
        uplFile.append('\t\t</true></if>\n')
        print("[Android] Add dependency " + itemName)

def update_file(filePath, isScopeHandler):
    result = list()
    with open(filePath, 'r') as upl:
        isOpenScope = False
        for line in upl:
            if isOpenScope:
                if "<!-- End " in line or "// End " in line:
                    isOpenScope = False
                    result.append(line)
            else:
                result.append(line)
                isOpenScope = isScopeHandler(line, result)

    with open(filePath, "w") as file:
        file.writelines(result)

def handle_upl_ios(line, result):
    if "<!-- Begin Dynamic Config" in line:
        applyDynamicConfigDefaults(_DEFAULT_GOOGLE_APP_IOS, result)
        return True

    if "<!-- Begin Copy Dynamic frameworks" in line:
        for adapter in mediation["adapters"]:
            if "embedLib" in adapter:
                framework = os.path.splitext(os.path.basename(adapter["embedLib"]))[0]
                tempLocation = '/Frameworks/' + framework + '.framework'
                srcPath = '$S(PluginDir)/../ThirdParty/iOS' + tempLocation
                dstPath = '$S(BuildDir)' + tempLocation
                result.append('\t\t<copyDir dst="' + dstPath + '" src="' + srcPath + '" />\n')
                print("[iOS] Add dynamic: " + dstPath)
        return True

    if "<!-- Begin SKAdNetworkItems" in line:
        result.append("\t\t\t<key>SKAdNetworkItems</key>\n")
        result.append("\t\t\t<array>\n")
        itemsCount = 0
        with open(os.path.join(rootDir, 'iOS', 'CASSKAdNetworks.txt'), 'r') as file:
            for item in file:
                result.append("\t\t\t\t<dict>\n")
                result.append("\t\t\t\t\t<key>SKAdNetworkIdentifier</key>\n")
                result.append("\t\t\t\t\t<string>" + item.strip() + ".skadnetwork</string>\n")
                result.append("\t\t\t\t</dict>\n")
                itemsCount += 1
        result.append("\t\t\t</array>\n")
        print("[iOS] Update " + str(itemsCount) + " SKAdNetworkItems")
        return True
    return False

def handle_upl_android(line, result):
    if "<!-- Begin Dynamic Config" in line:
        applyDynamicConfigDefaults(_DEFAULT_GOOGLE_APP_ANDROID, result)
        return True

    elif "<!-- Begin Init Mediation" in line:
        applyAndroidConfig(mediation["simple"], result)
        applyAndroidConfig(mediation["adapters"], result)
        return True

    elif "<!-- Begin Dependencies" in line:
        applyAndroidDependencyItem("repository $S(PluginDir)/../ThirdParty/Android/repository", result)
        applyAndroidDependencyItem("com.cleveradssolutions,cas-unreal-plugin,release", result)
        applyAndroidDependencyItem("com.cleveradssolutions,cas-sdk," + mediation["version"], result)
        applyAndroidDependency(mediation["simple"], result)
        applyAndroidDependency(mediation["adapters"], result)
        return True
    
    elif "<!-- Begin Repos" in line:
        for item in mediation['adapters']:
            if 'source' in item:
                result.append('\t\tmaven {\n')
                result.append('\t\t\turl = "' + item['source'] + '"\n')
                if 'depsSDK' in item:
                    depGroup = item['depsSDK'][0]['name'].split(':')[0]
                    result.append('\t\t\tcontent { it.includeGroup("' + depGroup + '") }\n')
                result.append('\t\t}\n')
                print('[Android] Add maven repository: ' + item['source'])
        return True
    return False

def archive_embedded_frameworks(name):
    dir = os.path.join(rootDir, 'IOS', 'Plugin')
    framework = name + '.framework'
    embedded = name + '.embeddedframework'
    embeddedPath = os.path.join(dir, embedded)
    embeddedZipPath = embeddedPath + '.zip'
    sourcePath = os.path.join(rootDir, '..', '..', '..', '..', '..', '..', 'CAS-Swift', 'libs', framework)

    if not move_native_lib(sourcePath, embeddedPath):
        return

    if os.path.exists(embeddedZipPath):
        os.remove(embeddedZipPath)
    
    archive = shutil.make_archive(
        base_name=embedded, 
        format='zip',
        root_dir=dir, 
        base_dir=embedded
    )
    shutil.move(archive, embeddedPath + '.zip')
    shutil.rmtree(embeddedPath)
    print('[iOS] Archived: ' + archive)

def update_uplugin(path):
    with open(path, 'r') as file:
        data = json.load(file)
    data["VersionName"] = mediation["version"]

    with open(path, "w") as file:
        json.dump(data, file, indent=4, sort_keys=True)
    print('Plugin version: ' + data["VersionName"])

def handle_window_config_header(line, result):
    if "// Begin Adapters" in line:
        for adapter in mediation["adapters"]:
            adapterName = adapter['name']
            result.append('\tUPROPERTY(Config, EditDefaultsOnly, AdvancedDisplay, Category = "Mediation"')

            # 16 - beta bit flag
            if (adapter['labels'] & 16) == 16:
                result.append(', meta = (DisplayName = "Include ' + adapterName + ' (beta)")')
            else:
                includeOptimalIOS = adapterName in iosOptimalSolutionContains
                includeFamiliesIOS = adapterName in iosFamiliesSolutionContains
                includeOptimalAndroid = adapterName in androidOptimalSolutionContains
                includeFamiliesAndroid = adapterName in androidFamiliesSolutionContains
                if includeOptimalIOS or includeFamiliesIOS or includeOptimalAndroid or includeFamiliesAndroid:
                    conditions = []
                    if includeOptimalIOS and includeOptimalAndroid:
                        conditions.append("!IncludeOptimalAds")
                    # Not supported scopes in conditions
                    # elif includeOptimalIOS:
                    #     conditions.append("(!IncludeOptimalAds || ConfigPlatformId!=2)")
                    # elif includeOptimalAndroid:
                    #     conditions.append("(!IncludeOptimalAds || ConfigPlatformId!=1)")

                    if includeFamiliesIOS and includeFamiliesAndroid:
                        conditions.append("!IncludeFamiliesAds")
                    # elif includeFamiliesIOS:
                    #     conditions.append("(!IncludeFamiliesAds || ConfigPlatformId!=2)")
                    # elif includeFamiliesAndroid:
                    #     conditions.append("(!IncludeFamiliesAds || ConfigPlatformId!=1)")
                        
                    result.append(', meta = (EditCondition = "' + " && ".join(conditions) + '")')

            result.append(')\n')
            result.append('\tbool Include' + adapterName + ' = false;\n\n')
        return True
    return False

def apply_solutions_config_impl(optimalSet, familiesSet, isOptimal, result):
    commonSet = optimalSet & familiesSet
    for adapter in commonSet:
        result.append('\t\t\tInclude' + adapter + ' = IncludeOptimalAds || IncludeFamiliesAds;\n')
    
    if isOptimal:
        for adapter in optimalSet:
            if adapter not in commonSet:
                result.append('\t\t\tInclude' + adapter + ' = IncludeOptimalAds;\n')
    else:
        for adapter in familiesSet:
            if adapter not in commonSet:
                result.append('\t\t\tInclude' + adapter + ' = IncludeFamiliesAds;\n')


def handle_window_config_script(line, result):
    if "// Begin Families Ads" in line:
        result.append('\t\tif(ConfigPlatformId == 1) {\n')
        apply_solutions_config_impl(iosOptimalSolutionContains, iosFamiliesSolutionContains, False, result)
        result.append('\t\t} else {\n')
        apply_solutions_config_impl(androidOptimalSolutionContains, androidFamiliesSolutionContains, False, result)
        result.append('\t\t}\n')
        return True
    if "// Begin Optimal Ads" in line:
        result.append('\t\tif(ConfigPlatformId == 1) {\n')
        apply_solutions_config_impl(iosOptimalSolutionContains, iosFamiliesSolutionContains, True, result)
        result.append('\t\t} else {\n')
        apply_solutions_config_impl(androidOptimalSolutionContains, androidFamiliesSolutionContains, True, result)
        result.append('\t\t}\n')
        return True
    return False

def move_native_lib(source, destination):
    if not os.path.exists(source):
        return False
    if os.path.exists(destination):
        os.remove(destination)
    shutil.move(source, destination)
    print("Updated: " + destination)
    return True

rootDir = os.path.dirname(os.path.abspath(sys.argv[0]))
pluginRoot = os.path.abspath(os.path.join(rootDir, '..', 'CleverAdsSolutions'))

iosOptimalSolutionContains = []
iosFamiliesSolutionContains = []
androidOptimalSolutionContains = []
androidFamiliesSolutionContains = []

mediation = read_mediation_list_for('IOS')
for solution in mediation['simple']:
    if solution['name'] == 'OptimalAds':
        iosOptimalSolutionContains = set(solution['contains'])
    elif solution['name'] == 'FamiliesAds':
        iosFamiliesSolutionContains = set(solution['contains'])

update_file(os.path.join(pluginRoot, 'CAS_UPL_IOS.xml'), isScopeHandler=handle_upl_ios)

mediation = read_mediation_list_for('Android')
for solution in mediation['simple']:
    if solution['name'] == 'OptimalAds':
        androidOptimalSolutionContains = set(solution['contains'])
    elif solution['name'] == 'FamiliesAds':
        androidFamiliesSolutionContains = set(solution['contains'])

update_file(os.path.join(pluginRoot, 'CAS_UPL_Android.xml'), isScopeHandler=handle_upl_android)

# Default Config Window update
update_file(filePath=os.path.join(pluginRoot, 'Public', 'CASDefaultConfig.h'), 
            isScopeHandler=handle_window_config_header)

update_file(filePath=os.path.join(pluginRoot, 'Private', 'CASDefaultConfig.cpp'), 
            isScopeHandler=handle_window_config_script)

# Native library update
move_native_lib(source=os.path.join(rootDir, '..', '..', '..', '..', '..', '..', 'CAS-Kotlin', 'buildCAS', 'cas-unreal-plugin-release.aar'),
                destination=os.path.join(rootDir, 'Android', 'repository', 'com', 'cleveradssolutions', 'cas-unreal-plugin', 'release', 'cas-unreal-plugin-release.aar'))

archive_embedded_frameworks('CASUnrealBridge')

update_uplugin(os.path.join(rootDir, '..', '..', 'CleverAdsSolutions.uplugin'))