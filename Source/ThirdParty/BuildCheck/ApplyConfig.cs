// Applies a config to a module and prints what the build would link, which is where the
// decisions about system frameworks are made.
using System;
using System.Collections.Generic;
using System.IO;
using UnrealBuildTool;
#if UE_5_0_OR_LATER
using EpicGames.Core;
#else
using Tools.DotNETCommon;
#endif

public static class ApplyConfig
{
    static int Failures = 0;

    public static int Main(string[] Args)
    {
        // The ad sources of this configuration ask for the tracking framework themselves
        Check("an ad source asked for the tracking framework",
            new string[] { "UIKit", "AppTrackingTransparency" }, new string[0],
            Expected: 1);

        // None of them did, and the SDK still looks the API up by name
        Check("no ad source asked for it",
            new string[] { "UIKit" }, new string[0],
            Expected: 1);

        // Asked for weakly by an SDK that is listed with its own package
        Check("asked for weakly",
            new string[] { "UIKit" }, new string[] { "AppTrackingTransparency" },
            Expected: 0, ExpectedWeak: 1);

        return Failures;
    }

    static void Check(string Name, string[] SysFrameworks, string[] SysWeakFrameworks,
        int Expected, int ExpectedWeak = 0)
    {
        // The constructor of the module does the whole plugin setup, which is not what
        // is under test here: the module is needed only as the thing that collects what
        // the build will link.
        var Module = (CleverAdsSolutions)System.Runtime.Serialization.FormatterServices
            .GetUninitializedObject(typeof(CleverAdsSolutions));
        foreach (var Field in typeof(ModuleRules).GetFields())
        {
            if (Field.FieldType == typeof(List<string>))
                Field.SetValue(Module, new List<string>());
            else if (Field.FieldType == typeof(List<Framework>))
                Field.SetValue(Module, new List<Framework>());
            else if (Field.FieldType == typeof(List<BundleResource>))
                Field.SetValue(Module, new List<BundleResource>());
            else if (Field.FieldType == typeof(ReceiptProperties))
                Field.SetValue(Module, new ReceiptProperties());
            else if (Field.FieldType == typeof(string))
                Field.SetValue(Module, "");
        }
        var Handler = new CleverAdsSolutions.IOSHandler();
        Handler.Version = "4.9.0";
        Handler.IncludedAdapters = new List<string> { "GoogleAds" };
        Handler.NativeDir = new DirectoryReference(Path.GetTempPath());
        Handler.FrameworksDir = DirectoryReference.Combine(Handler.NativeDir, "Frameworks");
        Handler.ResourcesDir = DirectoryReference.Combine(Handler.NativeDir, "Resources");
        Handler.BridgeFrameworkPath = "Bridge.embeddedframework.zip";
        Handler.EngineConfig = new ConfigHierarchy();

        var Config = new CleverAdsSolutions.XCodeConfig
        {
            Version = "4.9.0",
            Adapters = new string[] { "GoogleAds" },
            Resolved = new string[] { "GoogleAds" },
            SysLibs = new string[] { "z" },
            SysFrameworks = SysFrameworks,
            SysWeakFrameworks = SysWeakFrameworks,
            AdditionalResources = new string[0],
            Frameworks = new List<CleverAdsSolutions.XCodeBundle>(),
        };

        Config.ApplyToModule(Module, Handler);

        int Linked = Module.PublicFrameworks.FindAll(f => f == "AppTrackingTransparency").Count;
        int Weak = Module.PublicWeakFrameworks.FindAll(f => f == "AppTrackingTransparency").Count;
        bool Ok = Linked == Expected && Weak == ExpectedWeak;
        if (!Ok) Failures++;
        Console.WriteLine((Ok ? "  ok   " : "  FAIL ") + Name
            + ": linked " + Linked + " time(s), weak " + Weak
            + ", expected " + Expected + " and " + ExpectedWeak);
    }
}
