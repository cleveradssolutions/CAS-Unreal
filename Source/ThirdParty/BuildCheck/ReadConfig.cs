// Reads the config the preparation script wrote, with the very code the plugin build uses.
using System;
using System.IO;
#if UE_5_0_OR_LATER
using EpicGames.Core;
#else
using Tools.DotNETCommon;
#endif

public static class ReadConfig
{
    public static int Main(string[] Args)
    {
        var Source = Args[0];
        var Dir = Path.Combine(Path.GetTempPath(), "cas_config_check");
        Directory.CreateDirectory(Dir);
        File.Copy(Source, Path.Combine(Dir, "CASXCConfig.temp"), true);

        var Config = CleverAdsSolutions.XCodeConfig.Read(new DirectoryReference(Dir));
        if (Config == null)
        {
            Console.WriteLine("FAILED: nothing was read");
            return 1;
        }

        Console.WriteLine("  version: " + Config.Version);
        Console.WriteLine("  adapters: " + string.Join(", ", Config.Adapters));
        Console.WriteLine("  resolved: " + (Config.Resolved == null ? "-" : string.Join(", ", Config.Resolved)));
        foreach (var Framework in Config.Frameworks)
        {
            Console.WriteLine("  framework: " + Framework.name
                + " bundle='" + Framework.bundle + "'"
                + " dynamic=" + Framework.dynamic);
        }
        Console.WriteLine("  sysLibs: " + string.Join(", ", Config.SysLibs));
        Console.WriteLine("  sysFrameworks: " + Config.SysFrameworks.Length + " of them");
        Console.WriteLine("  sysWeakFrameworks: " + string.Join(", ", Config.SysWeakFrameworks));
        Console.WriteLine("  additionalResources: " + string.Join(", ", Config.AdditionalResources));
        return 0;
    }
}
