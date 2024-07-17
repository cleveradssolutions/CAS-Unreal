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

_ROOT_DIR = os.path.dirname(os.path.abspath(sys.argv[0]))
_PLUGIN_ROOT_DIR = os.path.abspath(os.path.join(_ROOT_DIR, '..'))
_PLUGIN_DIR = os.path.join(_PLUGIN_ROOT_DIR, 'CleverAdsSolutions')
_PLUGIN_MANIFEST = os.path.abspath(os.path.join(_PLUGIN_ROOT_DIR, '..', 'CleverAdsSolutions.uplugin'))

_CONFIG_IOS_DEST = os.path.join(_ROOT_DIR, 'IOS', 'CASMediation.list')
_BRIDGE_IOS_DEST_DIR = os.path.join(_ROOT_DIR, 'IOS', 'Plugin')
_SKADNETWORK_DEST = os.path.join(_ROOT_DIR, 'IOS', 'CASSKAdNetworks.txt')

_CONFIG_ANDROID_DEST = os.path.join(_ROOT_DIR, 'Android', 'CASMediation.list')
_BRIDGE_ANDROID_DEST = os.path.join(_ROOT_DIR, 'Android', 'repository', 'com', 'cleveradssolutions', 'cas-unreal-plugin', 'release', 'cas-unreal-plugin-release.aar')

#REPOS/Unreal-Sample/UEProject/Plugins/CleverAdsSolutions/Source/ThirdParty
_REPOS_DIR = os.path.abspath(os.path.join(_ROOT_DIR, '..', '..', '..', '..', '..', '..'))

_REPO_IOS_DIR = os.path.join(_REPOS_DIR, 'CAS-Swift')
_CONFIG_IOS_SOURCE = os.path.join(_REPO_IOS_DIR, 'CASMediation.list')
_BRIDGE_IOS_SOURCE_DIR = os.path.join(_REPO_IOS_DIR, 'libs')
_SKADNETWORK_SOURCE = os.path.join(_REPO_IOS_DIR, 'PublicSamplesRepo', 'SKAdNetworkCompact.txt')

_REPO_ANDROID_DIR = os.path.join(_REPOS_DIR, 'CAS-Kotlin')
_CONFIG_ANDROID_SOURCE = os.path.join(_REPO_ANDROID_DIR, 'CASMediation.list')
_BRIDGE_ANDROID_SOURCE = os.path.join(_REPO_ANDROID_DIR, 'buildCAS', 'cas-unreal-plugin-release.aar')
                          
def applyDynamicConfigDefaults(googleAppId, uplFile):
    uplFile.append('\t\t<setString result="GoogleAppId" value="' + googleAppId + '"/>\n')

def applyAndroidConfig(mediationArray, uplFile):
    for item in mediationArray:
        itemName = 'Include' + item['name']
        uplFile.append(
            '\t\t<setBoolFromProperty result="' + itemName + 
            '" ini="Engine" section="' + 
            _CONFIG_SECTION+'" property="' + itemName + 
            '" default="false"/>\n'
        )
        print("[Android] Init " + itemName)

def applyAndroidDependencyItem(line, uplFile):
    uplFile.append('\t\t<insertValue value="' + line.replace(':', ',') + '" /> <insertNewline/>\n')
    print("[Android] Add " + line)

def applyAndroidDependency(mediationArray, uplFile):
    for item in mediationArray:
        itemName = 'Include' + item['name']
        uplFile.append('\t\t<if condition="' + itemName + '"><true>\n')
        mainLib = item['libs'][0]
        applyAndroidDependencyItem(mainLib['name'] + mainLib['version'], uplFile)
        uplFile.append('\t\t</true></if>\n')

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
        print('[iOS] Reset dynamic config')
        return True

    if "<!-- Begin SKAdNetworkItems" in line:
        result.append("\t\t\t<key>SKAdNetworkItems</key>\n")
        result.append("\t\t\t<array>\n")
        itemsCount = 0
        with open(_SKADNETWORK_DEST, 'r') as file:
            for item in file:
                result.append("\t\t\t\t<dict>\n")
                result.append("\t\t\t\t\t<key>SKAdNetworkIdentifier</key>\n")
                result.append("\t\t\t\t\t<string>" + item.strip() + ".skadnetwork</string>\n")
                result.append("\t\t\t\t</dict>\n")
                itemsCount += 1
        result.append("\t\t\t</array>\n")
        print("[iOS] Updated: " + str(itemsCount) + " SKAdNetworkItems")
        return True
    return False

def handle_upl_android(line, result):
    if "<!-- Begin Dynamic Config" in line:
        applyDynamicConfigDefaults(_DEFAULT_GOOGLE_APP_ANDROID, result)
        result.append('\t\t<setString result="CASConfigResFile" value="cas_settings.json"/>\n')
        print('[Android] Reset dynamic config')
        return True

    elif "<!-- Begin Init Mediation" in line:
        applyAndroidConfig(mediation["simple"], result)
        applyAndroidConfig(mediation["adapters"], result)
        return True

    elif "<!-- Begin Dependencies" in line:
        applyAndroidDependencyItem("repository $S(PluginDir)/../ThirdParty/Android/repository", result)
        applyAndroidDependencyItem("com.cleveradssolutions:cas-unreal-plugin:release", result)
        applyAndroidDependencyItem("com.cleveradssolutions:cas-sdk:" + mediation["version"], result)
        applyAndroidDependency(mediation["simple"], result)
        applyAndroidDependency(mediation["adapters"], result)
        return True
    
    elif "<!-- Begin Repos" in line:
        for item in mediation['adapters']:
            if 'source' in item:
                result.append('\t\tmaven {\n')
                result.append('\t\t\tname = "' + item['name'] + 'AdsRepo"\n')
                result.append('\t\t\turl = "' + item['source'] + '"\n')
                if 'sourceGroups' in item:
                    result.append('\t\t\tcontent {\n')
                    for group in item['sourceGroups']:
                        result.append('\t\t\t\tit.includeGroup("' + group + '")\n')
                    result.append('\t\t\t}\n')
                else:
                    libs = item['libs']
                    if len(libs) == 2:
                        depGroup = libs[1]['name'].split(':')[0]
                        result.append('\t\t\tcontent { it.includeGroup("' + depGroup + '") }\n')
                    elif len(libs) > 2:
                        result.append('\t\t\tcontent {\n')
                        for lib in libs[1:]:
                            depGroup = lib['name'].split(':')[0]
                            result.append('\t\t\t\tit.includeGroup("' + depGroup + '")\n')
                        result.append('\t\t\t}\n')
                result.append('\t\t}\n')
                print('[Android] Add maven repository: ' + item['source'])
        return True
    return False

def embed_bridge_framework(name):
    framework = name + '.framework'
    embedded = name + '.embeddedframework'
    embeddedPath = os.path.join(_BRIDGE_IOS_DEST_DIR, embedded)
    embeddedZipPath = embeddedPath + '.zip'
    sourcePath = os.path.join(_BRIDGE_IOS_SOURCE_DIR, framework)

    if not os.path.exists(embeddedPath):
        os.mkdir(embeddedPath)
    if not move_source(sourcePath, os.path.join(embeddedPath, framework)):
        if os.path.exists(embeddedPath):
            shutil.rmtree(embeddedPath)
        return

    if os.path.exists(embeddedZipPath):
        os.remove(embeddedZipPath)
    
    archive = shutil.make_archive(
        base_name=embedded, 
        format='zip',
        root_dir=_BRIDGE_IOS_DEST_DIR, 
        base_dir=embedded
    )
    shutil.move(archive, embeddedPath + '.zip')
    shutil.rmtree(embeddedPath)
    print('[iOS] Updated: ' + os.path.relpath(archive, _PLUGIN_ROOT_DIR))

def handle_window_config_header(line, result):
    if "// Begin Adapters" in line:
        for adapter in mediation["adapters"]:
            adapterName = adapter['name']
            result.append('\tUPROPERTY(Config, EditDefaultsOnly, AdvancedDisplay, Category = "Mediation"')

            meta = []
            if (adapter['labels'] & 16) == 16:  # 16 - beta bit flag
                meta.append('DisplayName = "Include ' + adapterName + ' (beta)"')
            elif 'altName' in adapter:
                meta.append('DisplayName = "Include ' + adapterName + '/' + adapter['altName']  + '"')
            if 'comment' in adapter:
                meta.append('ToolTip = "' + adapter['comment'] + '"')
                
            includeOptimalIOS = adapterName in iosOptimalSolutionContains
            includeFamiliesIOS = adapterName in iosFamiliesSolutionContains
            includeOptimalAndroid = adapterName in androidOptimalSolutionContains
            includeFamiliesAndroid = adapterName in androidFamiliesSolutionContains
            if includeOptimalIOS or includeFamiliesIOS or includeOptimalAndroid or includeFamiliesAndroid:
                conditions = []
                if includeOptimalIOS and includeOptimalAndroid:
                    conditions.append("!IncludeOptimalAds")
                elif includeOptimalIOS:
                    conditions.append("!IncludeOptimalAds || ConfigPlatformId != 2")
                elif includeOptimalAndroid:
                    conditions.append("!IncludeOptimalAds || ConfigPlatformId != 1")

                if includeFamiliesIOS and includeFamiliesAndroid:
                    conditions.append("!IncludeFamiliesAds")
                elif includeFamiliesIOS:
                    conditions.append("!IncludeFamiliesAds || ConfigPlatformId != 2")
                elif includeFamiliesAndroid:
                    conditions.append("!IncludeFamiliesAds || ConfigPlatformId != 1")
                    
                meta.append('EditCondition = "' + ' && '.join(conditions) + '"')
            
            if meta:
                result.append(', meta = (' + ', '.join(meta)  + ')')
                
            result.append(')\n')
            result.append('\tbool Include' + adapterName + ' = false;\n\n')
        print('Updated: Default Config window header')
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
        apply_solutions_config_impl(androidOptimalSolutionContains, androidFamiliesSolutionContains, False, result)
        result.append('\t\t} else {\n')
        apply_solutions_config_impl(iosOptimalSolutionContains, iosFamiliesSolutionContains, False, result)
        result.append('\t\t}\n')
        print('Updated: Default Config window Families Ads')
        return True
    if "// Begin Optimal Ads" in line:
        result.append('\t\tif(ConfigPlatformId == 1) {\n')
        apply_solutions_config_impl(androidOptimalSolutionContains, androidFamiliesSolutionContains, True, result)
        result.append('\t\t} else {\n')
        apply_solutions_config_impl(iosOptimalSolutionContains, iosFamiliesSolutionContains, True, result)
        result.append('\t\t}\n')
        print('Updated: Default Config window Optimal Ads')
        return True
    return False

def move_source(source, destination):
    if not os.path.exists(source):
        print("[!] Source not found: " + source)
        return False
    if os.path.exists(destination):
        os.remove(destination)
    shutil.move(source, destination)
    print("Updated: " + os.path.relpath(destination, _PLUGIN_ROOT_DIR))
    return True

def copy_source_file(source, destination):
    if not os.path.exists(source):
        print("[!] Source not found: " + source)
        return False
    shutil.copyfile(source, destination)
    print("Updated: " + os.path.relpath(destination, _PLUGIN_ROOT_DIR))
    return True

iosOptimalSolutionContains = []
iosFamiliesSolutionContains = []
androidOptimalSolutionContains = []
androidFamiliesSolutionContains = []

# Update iOS build config
copy_source_file(_CONFIG_IOS_SOURCE, _CONFIG_IOS_DEST)
copy_source_file(_SKADNETWORK_SOURCE, _SKADNETWORK_DEST)
with open(_CONFIG_IOS_DEST, 'r') as file:
    mediation = json.load(file)
    
for solution in mediation['simple']:
    if solution['name'] == 'OptimalAds':
        iosOptimalSolutionContains = set(solution['contains'])
    elif solution['name'] == 'FamiliesAds':
        iosFamiliesSolutionContains = set(solution['contains'])

update_file(os.path.join(_PLUGIN_DIR, 'CAS_UPL_IOS.xml'), 
            isScopeHandler=handle_upl_ios)

# Update Android build config
copy_source_file(_CONFIG_ANDROID_SOURCE, _CONFIG_ANDROID_DEST)
with open(_CONFIG_ANDROID_DEST, 'r') as file:
    mediation = json.load(file)
for solution in mediation['simple']:
    if solution['name'] == 'OptimalAds':
        androidOptimalSolutionContains = set(solution['contains'])
    elif solution['name'] == 'FamiliesAds':
        androidFamiliesSolutionContains = set(solution['contains'])

update_file(os.path.join(_PLUGIN_DIR, 'CAS_UPL_Android.xml'), 
            isScopeHandler=handle_upl_android)

# Update Default Config window
update_file(os.path.join(_PLUGIN_DIR, 'Public', 'CASDefaultConfig.h'), 
            isScopeHandler=handle_window_config_header)

update_file(os.path.join(_PLUGIN_DIR, 'Private', 'CASDefaultConfig.cpp'), 
            isScopeHandler=handle_window_config_script)

# Update Bridge plugins
move_source(source=_BRIDGE_ANDROID_SOURCE, destination=_BRIDGE_ANDROID_DEST)

embed_bridge_framework('CASUnrealBridge')

# Update Unreal Plugin manifest
with open(_PLUGIN_MANIFEST, 'r') as file:
    data = json.load(file)
versionCodeArray = reversed(map(int, (mediation["version"].split("."))))
versionCodeArray = [(10 ** (index * 2) * ver) for index,ver in enumerate(versionCodeArray)] 
data["Version"] = sum(versionCodeArray)
print('Plugin version code: ' + str(data["Version"]))
data["VersionName"] = mediation["version"]
print('Plugin version: ' + data["VersionName"])
with open(_PLUGIN_MANIFEST, "w") as file:
    json.dump(data, file, indent=4, sort_keys=True)