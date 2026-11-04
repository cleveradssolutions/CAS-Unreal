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
            Console.WriteLine("  FAIL nothing was read from " + ConfigDir);
            return 1;
        }

        Check("prepared for what the project asks", Config, NativeDir, "4.9.0-beta.2",
            new List<string> { "CASExchange", "TenjinMMP" }, true);
        Check("one more adapter selected since", Config, NativeDir, "4.9.0-beta.2",
            new List<string> { "CASExchange", "TenjinMMP", "GoogleAds" }, false);
        Check("another adapter selected instead", Config, NativeDir, "4.9.0-beta.2",
            new List<string> { "CASExchange", "GoogleAds" }, false);
        Check("the SDK was updated since", Config, NativeDir, "4.9.1",
            new List<string> { "CASExchange", "TenjinMMP" }, false);
        Check("the frameworks are gone", Config, Path.Combine(Path.GetTempPath(), "nothing_here"),
            "4.9.0-beta.2", new List<string> { "CASExchange", "TenjinMMP" }, false);
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
