// Does the build reuse what was prepared, and notice when it no longer fits?
using System;
using System.Collections.Generic;
using System.IO;
#if UE_5_0_OR_LATER
using EpicGames.Core;
#else
using Tools.DotNETCommon;
#endif

public static class ReuseConfig
{
    public static int Main(string[] Args)
    {
        var ConfigDir = Args[0];     // where CASXCConfig.temp is
        var NativeDir = Args[1];     // where Frameworks/ is

        var Config = CleverAdsSolutions.XCodeConfig.Read(new DirectoryReference(ConfigDir));
        if (Config == null)
        {
            // Nothing to reuse is an answer, not a failure: the build prepares again
            Console.WriteLine("  ok   nothing to reuse, the build would prepare again");
            return 0;
        }

        // The selection of the config is the one a project would be asking for, whatever
        // config was handed to this check
        var Selected = new List<string>(Config.Adapters);
        var OneMore = new List<string>(Selected) { "GoogleAds" };
        var Different = new List<string>(Selected);
        if (Different.Count > 0) Different[0] = "GoogleAds"; else Different.Add("GoogleAds");

        Check("prepared for what the project asks", Config, NativeDir, Config.Version,
            Selected, true);
        Check("one more adapter selected since", Config, NativeDir, Config.Version,
            OneMore, false);
        Check("another adapter selected instead", Config, NativeDir, Config.Version,
            Different, false);
        Check("the SDK was updated since", Config, NativeDir, Config.Version + ".1",
            Selected, false);
        Check("the frameworks are gone", Config, Path.Combine(Path.GetTempPath(), "nothing_here"),
            Config.Version, Selected, false);
        return Failures;
    }

    static int Failures = 0;

    static void Check(string Name, CleverAdsSolutions.XCodeConfig Config, string NativeDir,
        string Version, List<string> Adapters, bool Expected)
    {
        var Handler = new CleverAdsSolutions.IOSHandler();
        Handler.Version = Version;
        Handler.IncludedAdapters = Adapters;
        Handler.NativeDir = new DirectoryReference(NativeDir);
        Handler.FrameworksDir = DirectoryReference.Combine(Handler.NativeDir, "Frameworks");

        bool Actual = Config.IsSetupReady(Handler);
        bool Ok = Actual == Expected;
        if (!Ok) Failures++;
        Console.WriteLine((Ok ? "  ok   " : "  FAIL ") + Name
            + ": reuse=" + Actual + ", expected=" + Expected);
    }
}
