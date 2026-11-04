// Minimal stand-ins for the UnrealBuildTool API that CleverAdsSolutions.Build.cs uses.
// They exist to compile the module rules outside the engine: a compile error here is an
// error in the module rules, not in the engine.
using System;
using System.Collections.Generic;
using System.IO;
#if UE_5_0_OR_LATER
using EpicGames.Core;
#else
using Tools.DotNETCommon;
#endif

namespace UnrealBuildTool
{
    public enum UnrealTargetPlatform { IOS, Android, Mac }
    public enum UnrealTargetConfiguration { Debug, Development, Shipping }

    public class Framework
    {
        public Framework(string Name, string Path, string CopyBundledAssets = null, bool bCopyFramework = false) { }
    }

    public class BundleResource
    {
        public BundleResource(string Path) { }
    }

    public class ReceiptProperty
    {
        public ReceiptProperty(string Name, string Value) { }
    }

    public class ReceiptProperties : List<ReceiptProperty>
    {
        public void Add(string Name, string Value) { base.Add(new ReceiptProperty(Name, Value)); }
    }

    public class ReadOnlyTargetRules
    {
        public UnrealTargetPlatform Platform;
        public UnrealTargetConfiguration Configuration;
        public FileReference ProjectFile;
        public string RelativeEnginePath;
    }

    public class ModuleRules
    {
        public ModuleRules(ReadOnlyTargetRules Target) { }
        public string ModuleDirectory = "";
        public string EngineDirectory = "";
        public List<string> PublicDefinitions = new List<string>();
        public List<string> PublicDependencyModuleNames = new List<string>();
        public List<string> PrivateDependencyModuleNames = new List<string>();
        public List<string> PublicIncludePaths = new List<string>();
        public List<string> PrivateIncludePaths = new List<string>();
        public List<string> PublicSystemLibraries = new List<string>();
        public List<string> PublicSystemLibraryPaths = new List<string>();
        public List<string> PublicFrameworks = new List<string>();
        public List<string> PublicWeakFrameworks = new List<string>();
        public List<Framework> PublicAdditionalFrameworks = new List<Framework>();
        public List<BundleResource> AdditionalBundleResources = new List<BundleResource>();
        public ReceiptProperties AdditionalPropertiesForReceipt = new ReceiptProperties();
        public bool bEnableObjCAutomaticReferenceCounting;
        public string PrivatePCHHeaderFile = "";
        public List<string> PrivateIncludePathModuleNames = new List<string>();
        public List<string> PrivateDefinitions = new List<string>();
        public ModuleRules.PCHUsageMode PCHUsage;
        public enum PCHUsageMode { UseExplicitOrSharedPCHs, NoSharedPCHs }
    }

    public static class Utils
    {
        public static string MakePathRelativeTo(string Path, string Directory) { return Path; }
        public static int RunLocalProcess(System.Diagnostics.Process Process) { return 0; }
        public static string RunLocalProcessAndReturnStdOut(string Command, string Args) { return ""; }
    }

    public enum ConfigHierarchyType { Engine, Game }

    public class ConfigHierarchy
    {
        public bool TryGetValue(string Section, string Key, out string Value) { Value = null; return false; }
        public bool TryGetValue(string Section, string Key, out bool Value) { Value = false; return false; }
        public bool GetArray(string Section, string Key, out List<string> Values) { Values = null; return false; }
        public bool GetString(string Section, string Key, out string Value) { Value = null; return false; }
    }

    public static class ConfigCache
    {
        public static ConfigHierarchy ReadHierarchy(ConfigHierarchyType Type, DirectoryReference Dir, UnrealTargetPlatform Platform)
        {
            return new ConfigHierarchy();
        }
    }
}
