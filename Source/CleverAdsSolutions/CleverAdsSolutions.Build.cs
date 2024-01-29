// Copyright CleverAdsSolutions LTD, CAS.AI. All Rights Reserved.

using UnrealBuildTool;
using System;
using System.IO;
using System.Linq;
using System.Collections.Generic;
using System.Diagnostics;
using System.Text;

#if UE_5_0_OR_LATER
using System.Xml.Linq;
using EpicGames.Core;
using Microsoft.Extensions.Logging;
#else
using Tools.DotNETCommon;
#endif

public class CleverAdsSolutions : ModuleRules
{
	public const string LOG_PREFIX = "[CAS.AI] ";
	public const string MINIMUM_IOS_VERSION = "13.0";
	public const string IOS_BRIDGE_NAME = "CASUnrealBridge";
	private const string IOS_FRAMEWORKS_PROJECT = "CASFrameworks";

	public CleverAdsSolutions(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.NoSharedPCHs;

		PrivatePCHHeaderFile = "Private/CleverAdsSolutionsPrivatePCH.h";

		PublicDependencyModuleNames.Add("Core");

		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
				"CoreUObject",
				"Engine",
				//"Slate",
				//"SlateCore",
				"Projects"
				// ... add private dependencies that you statically link with here ...	
			}
		);

		PrivateIncludePathModuleNames.Add("Settings");

		PrivateIncludePaths.Add(Path.Combine(ModuleDirectory, "Private"));
		PublicIncludePaths.Add(Path.Combine(ModuleDirectory, "Public"));
		PublicIncludePaths.Add(Path.Combine(ModuleDirectory, "PublicSubsystem"));

#if UE_5_3_OR_LATER
		PrivateDefinitions.Add("UE_5_3_OR_LATER=1");
#else
		PrivateDefinitions.Add("UE_5_3_OR_LATER=0");
#endif

		if (Target.Platform == UnrealTargetPlatform.Android)
		{
			new CASAndroidConfig().Apply(this, Target);
		}
		else if (Target.Platform == UnrealTargetPlatform.IOS)
		{
			new CASIOSConfig().Apply(this, Target);
		}
		else
		{
			LogDebug("CAS.AI Disabled for Not supported platform: " + Target.Platform.ToString());
			PublicDefinitions.Add("WITH_CAS=0");
		}
	}

	private static void LogDebug(string message)
	{
#if UE_5_0_OR_LATER
		System.Console.WriteLine(LOG_PREFIX + message);
#else
		Log.TraceLog(LOG_PREFIX + message);
#endif
	}

	public static void CancelBuild(string Error)
	{
		throw new BuildException(Error);
	}

	public static void LostRequiredFile(string File)
	{
		throw new BuildException("Lost required plugin file. Try reinstal CAS plugin and check file exists: " + File);
	}

	public abstract class CASMediationConfig
	{
		public const string EngineConfigName = "/Script/CleverAdsSolutions.CASDefaultConfig";

		public string version;
		public CASNetwork[] solutions;
		public CASNetwork[] adapters;

		public ConfigHierarchy EngineConfig;
		public string NativePath;
		public string ManagerID;
		public bool TestAdMode;
		public string CacheConfigFileName;// = "cas_config_res.casconfig";

		public void LogWarning(string message)
		{
#if UE_5_0_OR_LATER
			Log.Logger.LogWarning(LOG_PREFIX + message);
#else
			Log.TraceWarning(LOG_PREFIX + message);
#endif
		}

		public int RunProcess(string Name, params string[] Args)
		{
			var argsLine = string.Join(" ", Args);
			LogDebug("Run process: " + Name + " " + argsLine + " ...");
			var info = new ProcessStartInfo(Name, argsLine);
			info.WorkingDirectory = NativePath;
			// Warning: CocoaPods requires your terminal to be using UTF-8 encoding.
			info.StandardOutputEncoding = Encoding.UTF8;
			info.StandardErrorEncoding = Encoding.UTF8;
			//info.StandardInputEncoding = Encoding.UTF8;
			info.Environment["LANG"] = "en_US.UTF-8";
#if UE_5_0_OR_LATER
			return Utils.RunLocalProcessAndLogOutput(info, Log.Logger);
#else
			return Utils.RunLocalProcessAndLogOutput(info);
#endif
		}

		public string RunProcessForResult(string Name, params string[] Args)
		{
			var argsLine = string.Join(" ", Args);
			LogDebug("Run process: " + Name + " " + argsLine + " ...");
			// #if UE_5_0_OR_LATER
			// 	return Utils.RunLocalProcessAndReturnStdOut(Name, argsLine, Logger);
			// #else
			return Utils.RunLocalProcessAndReturnStdOut(Name, argsLine);
			// #endif
		}

		public virtual void Apply(CleverAdsSolutions module, ReadOnlyTargetRules Target)
		{
			// module.ModuleDirectory = Plugins/CleverAdsSolutions/Source/CleverAdsSolutions
			// Platform name: Android, IOS
			string PlatformName = Target.Platform.ToString();
			LogDebug(PlatformName + " Configuration");
			NativePath = Path.GetFullPath(Path.Combine(module.ModuleDirectory, "..", "ThirdParty", PlatformName));

			string ModuleDirectoryRelative = Utils.MakePathRelativeTo(module.ModuleDirectory, Target.RelativeEnginePath);
			string UPLFileName = "CAS_UPL_" + PlatformName + ".xml";
			string UPLFilePath = Path.Combine(module.ModuleDirectory, UPLFileName);
			if (!File.Exists(UPLFilePath))
				LostRequiredFile(UPLFilePath);

			string listPlatform = Target.Platform == UnrealTargetPlatform.IOS ? "iOS" : PlatformName;
			string MediationPath = Path.Combine(NativePath, "CAS" + listPlatform + "Mediation.list");
			if (!File.Exists(MediationPath))
				LostRequiredFile(MediationPath);

			module.PublicDefinitions.Add("WITH_CAS=1");
			module.AdditionalPropertiesForReceipt.Add(PlatformName + "Plugin", Path.Combine(ModuleDirectoryRelative, UPLFileName));

			JsonObject ConfigJson = JsonObject.Read(new FileReference(MediationPath));
			version = ConfigJson.GetStringField("version");
			solutions = ParseNetworksList(ConfigJson, "simple");
			adapters = ParseNetworksList(ConfigJson, "adapters");

			EngineConfig = ConfigCache.ReadHierarchy(ConfigHierarchyType.Engine, DirectoryReference.FromFile(Target.ProjectFile), Target.Platform);

			EngineConfig.TryGetValue(EngineConfigName, "CASAppID", out ManagerID);
			if (string.IsNullOrEmpty(ManagerID))
			{
				ManagerID = "demo";
			}
			if (!EngineConfig.TryGetValue(EngineConfigName, "TestAdsMode", out TestAdMode))
			{
				TestAdMode = true;
			}
			for (int i = 0; i < solutions.Length; i++)
			{
				solutions[i].Configure(EngineConfig, EngineConfigName, module);
			}
			for (int i = 0; i < adapters.Length; i++)
			{
				adapters[i].Configure(EngineConfig, EngineConfigName, module);
			}

			if (string.IsNullOrEmpty(CacheConfigFileName))
				CacheConfigFileName = GetResourcesFileName();

			DownloadConfig(Target);
			UpdateUPLFile(UPLFilePath);
		}

		public CASNetwork FindAdapter(string name)
		{
			for (int i = 0; i < solutions.Length; i++)
			{
				if (solutions[i].name == name)
					return solutions[i];
			}
			for (int i = 0; i < adapters.Length; i++)
			{
				if (adapters[i].name == name)
					return adapters[i];
			}
			return null;
		}

		private void UpdateUPLFile(string UPLFilePath)
		{
			var UPLFileFullPath = Path.GetFullPath(UPLFilePath);
			var UPLFile = new List<string>(256);
			using (var Stream = File.OpenRead(UPLFileFullPath))
			{
				using (var Reader = new StreamReader(Stream, Encoding.UTF8, true))
				{
					const string googleAppIdKey = "<setString result=\"GoogleAppId\"";
					const string casConfigResKey = "<setString result=\"CASConfigResFile\"";
					string line;
					while ((line = Reader.ReadLine()) != null)
					{
						var trimedLine = line.TrimStart();
						if (trimedLine.StartsWith(googleAppIdKey))
						{
							var googleAppId = ReadGoogleAppIdFromConfigFile();
							if (!string.IsNullOrEmpty(googleAppId))
							{
								UPLFile.Add("\t\t" + googleAppIdKey + " value=\"" + googleAppId + "\"/>");
								continue;
							}
						}
						else if (trimedLine.StartsWith(casConfigResKey))
						{
							UPLFile.Add("\t\t" + casConfigResKey + " value=\"" + GetResourcesFileName() + "\"/>");
							continue;
						}
						UPLFile.Add(line);
					}
				}
			}
			using (StreamWriter Writer = new StreamWriter(UPLFileFullPath))
			{
				for (int i = 0; i < UPLFile.Count; i++)
					Writer.WriteLine(UPLFile[i]);
			}
		}

		private static CASNetwork[] ParseNetworksList(JsonObject ConfigJson, string Field)
		{
			JsonObject[] ResultJson = ConfigJson.GetObjectArrayField(Field);
			CASNetwork[] Result = new CASNetwork[ResultJson.Length];
			for (int i = 0; i < ResultJson.Length; i++)
			{
				Result[i] = new CASNetwork(ResultJson[i]);
			}
			return Result;
		}

		private string GetFileSuffix(string casId)
		{
			if (string.IsNullOrEmpty(casId))
				return "";
			string suffixChar = char.ToLower(casId[casId.Length - 1]).ToString();
			return casId.Length.ToString() + suffixChar;
		}

		public string GetResourcesFileName()
		{
			return "cas_settings" + GetFileSuffix(ManagerID) + ".json";
		}

		public string GetResourcesFilePath()
		{
			return Path.Combine(NativePath, CacheConfigFileName);
		}

		private string WrapInDoubleQuotes(string line)
		{
			return "\"" + line + "\"";
		}

		public void DownloadConfig(ReadOnlyTargetRules Target)
		{
			const string HelpMessage = "\nPlease try using a real CAS identifier in the `Project Settings > CAS.AI` menu else contact support." +
							"\nIf you haven't created an CAS account and registered an app yet, use Test Ads mode to continue build (If not Shipping build only).";

			var invalidManagerId = "CAS.AI '" + ManagerID + "' is not registered in CAS.AI." + HelpMessage;
			if (string.IsNullOrEmpty(ManagerID) || ManagerID == "demo")
			{
				if (!TestAdMode || Target.Configuration == UnrealTargetConfiguration.Shipping)
					CancelBuild(invalidManagerId);
				return;
			}

			string CacheConfigFilePath = GetResourcesFilePath();
			if (File.Exists(CacheConfigFilePath) && File.GetLastWriteTime(CacheConfigFilePath).AddHours(12) > DateTime.Now)
				return;

			var RequestUrl = "https://psvpromo.psvgamestudio.com/cas-settings.php?apply=config&platform=" +
				(Target.Platform == UnrealTargetPlatform.Android ? "0" : "1") +
				"&bundle=" + ManagerID;

			var Response = RunProcessForResult("curl",
				"-w", WrapInDoubleQuotes("%{http_code}"),
				"-o", WrapInDoubleQuotes(CacheConfigFilePath),
				WrapInDoubleQuotes(RequestUrl)
			);
			Response = Response.Trim(' ', '"');

			if (Response.StartsWith("200") || Response.EndsWith("200"))
			{
				LogDebug("Configuration loaded for " + ManagerID);
				return;
			}
			try
			{
				File.Delete(CacheConfigFilePath);
			}
			catch (Exception e)
			{
				LogWarning("Failed to remove invalid configuration:" + e.ToString());
			}
			if (Response.StartsWith("204") || Response.EndsWith("204"))
			{
				if (TestAdMode)
				{
					LogWarning(invalidManagerId);
					return;
				}
				CancelBuild(invalidManagerId);
			}
			LogWarning("Failed to load configuration: " + Response);
		}

		public string ReadGoogleAppIdFromConfigFile()
		{
			string CacheConfigFilePath = GetResourcesFilePath();
			if (!File.Exists(CacheConfigFilePath)) return null;
			var Config = File.ReadAllText(CacheConfigFilePath);
			var appId = FindPropertyString(Config, "admob_app_id");
			if (appId != null && appId.Contains('~'))
				return appId;
			return null;
		}

		private static string FindPropertyString(string Source, string Property)
		{
			int index = Source.IndexOf(Property);
			if (index < 0) return null;
			index += Property.Length + 1;
			while (Source[index] != '"')
				index++;
			index++;
			return Source.Substring(index, Source.IndexOf('"', index) - index);
		}
	}

	public class CASNetwork
	{
		public bool included = false;
		public string name;
		public string version;
		public string dependency;
		//public string source;
		public string embedLib;
		//public string[] contains;

		public CASNetwork(string name)
		{
			this.name = name;
			version = "";
			dependency = "";
			//source = "";
			//contains = new string[0];
		}

		public CASNetwork(JsonObject json)
		{
			this.name = json.GetStringField("name");
			json.TryGetStringField("version", out this.version);
			json.TryGetStringField("dependency", out this.dependency);
			//json.TryGetStringField("source", out this.source);
			json.TryGetStringField("embedLib", out this.embedLib);
			//json.TryGetStringArrayField("contains", out this.contains);
		}

		public void Configure(ConfigHierarchy EngineConfig, string ConfigName, CleverAdsSolutions module)
		{
			EngineConfig.TryGetValue(ConfigName, "Include" + name, out included);
			//module.PublicDefinitions.Add("WITH_CAS_" + name.ToUpper() + "=" + (included ? "1" : "0"););
		}
	}

	private class CASAndroidConfig : CASMediationConfig
	{
		//private const string ConfigRuntimeSettings = "/Script/AndroidRuntimeSettings.AndroidRuntimeSettings";

		public override void Apply(CleverAdsSolutions module, ReadOnlyTargetRules Target)
		{
			module.PublicDependencyModuleNames.Add("Launch"); // Includes Android JNI support
			base.Apply(module, Target);
			string PluginLibrary = Path.Combine(NativePath, "repository", "com", "cleveradssolutions", "cas-unreal-plugin", "release", "cas-unreal-plugin-release.aar");
			if (!File.Exists(PluginLibrary))
				LostRequiredFile(PluginLibrary);
		}
	}

	public class CASIOSConfig : CASMediationConfig
	{
#pragma warning disable IDE0051
		private const string ConfigRuntimeSettings = "/Script/IOSRuntimeSettings.IOSRuntimeSettings";
#pragma warning restore IDE0051

		public string FrameworksPath;
		public List<string> IncludedAdapters = new List<string>();
		public HashSet<string> SupportedDynamicFrameworks = new HashSet<string>();

		public override void Apply(CleverAdsSolutions Module, ReadOnlyTargetRules Target)
		{
			base.Apply(Module, Target);

#if !UE_5_2_OR_LATER
			// Starting from version 5.2, Unreal Engine no longer utilizes Bitcode in iOS builds.
			bool BuildWithBitcode;
			if (!EngineConfig.TryGetValue(ConfigRuntimeSettings, "bShipForBitcode", out BuildWithBitcode) || BuildWithBitcode)
			{
				CancelBuild("Bitcode is deprecated and no longer required for AppStore. " +
					"Disable Bitcode check in iOS Runtime Settings window.");
			}
#endif

#if UE_5_3_OR_LATER
			const string XCodeSettings = "/Script/MacTargetPlatform.XcodeProjectSettings";
			bool BuildModernXCode;
			if (!EngineConfig.TryGetValue(XCodeSettings, "bUseModernXcode", out BuildModernXCode) || BuildModernXCode)
			{
				LogWarning("The Modern XCode build does not copy additional resources correctly.");
				LogWarning("Please add a configuration file in XCode project > App Target > Build Phases > Copy Bundle Resources > + " + GetResourcesFilePath());
			}
#endif

			// Work only for not Modern Xcode Build UE 5.3+
			// Same as <copy file> in UPL.xml
			//Module.AdditionalBundleResources.Add(new BundleResource(GetResourcesFilePath()));

#if UE_5_0_OR_LATER
			Module.bEnableObjCAutomaticReferenceCounting = true;
#endif

			FrameworksPath = Path.Combine(NativePath, "Frameworks");
			for (int i = 0; i < solutions.Length; i++)
			{
				if (solutions[i].included)
					IncludedAdapters.Add(solutions[i].name);
			}
			for (int i = 0; i < adapters.Length; i++)
			{
				if (adapters[i].included)
				{
					IncludedAdapters.Add(adapters[i].name);

					var embedLib = adapters[i].embedLib;
					if (!string.IsNullOrEmpty(embedLib))
					{
						SupportedDynamicFrameworks.Add(Path.GetFileNameWithoutExtension(embedLib));
					}
				}
			}

			var XCodeCache = XCodeConfig.Read(NativePath);
			if (XCodeCache != null && XCodeCache.IsSetupReady(FrameworksPath, version, IncludedAdapters))
			{
				LogDebug("Apply XCode Config from cache");
				XCodeCache.Apply(this, Module);
				return;
			}

			InstallPods(Module);
		}

		private void InstallPods(CleverAdsSolutions Module)
		{
			string FrameworkProj = Path.Combine(NativePath, IOS_FRAMEWORKS_PROJECT + ".xcodeproj");
			if (!Directory.Exists(FrameworkProj))
				LostRequiredFile(FrameworkProj);

			string BuildDir = Path.Combine(NativePath, "Temp");
			string PodfileLock = Path.Combine(NativePath, "Podfile.lock");
			FileUtils.ForceDeleteFile(PodfileLock);
			FileUtils.ForceDeleteDirectory(BuildDir);
			FileUtils.ForceDeleteDirectory(FrameworksPath);
			Directory.CreateDirectory(FrameworksPath);

			string PodTool = FindPodTool();

			var Config = new XCodeConfig()
			{
				version = version,
				adapters = IncludedAdapters.ToArray(),
				bundles = new List<XCodeBundle>()
			};

			CreatePodfile(Config);

			RunProcess(PodTool, "update");

			const string XCWorkspace = IOS_FRAMEWORKS_PROJECT + ".xcworkspace";
			string XCWorkspacePath = Path.Combine(NativePath, XCWorkspace);
			if (!Directory.Exists(XCWorkspacePath))
				CancelBuild("Pod installation failed. Fix errors in console logs and try again.");

			RunProcess("xcodebuild", "build",
				"-workspace", XCWorkspace,
				"-scheme", IOS_FRAMEWORKS_PROJECT,
				"-derivedDataPath", BuildDir,
				"-sdk", "iphoneos",
				"-destination", "generic/platform=iOS",
				"-configuration", "Release",
				"IPHONEOS_DEPLOYMENT_TARGET=" + MINIMUM_IOS_VERSION,
				"GCC_GENERATE_DEBUGGING_SYMBOLS=NO"
			);

			Config.FindDependencies(this, BuildDir);
			Config.Apply(this, Module);
			Config.Save(NativePath);

			RunProcess(PodTool, "deintegrate");
			FileUtils.ForceDeleteDirectory(XCWorkspacePath);
			FileUtils.ForceDeleteFile(PodfileLock);
			FileUtils.ForceDeleteDirectory(BuildDir);
		}

		private string FindPodTool()
		{
			const string POD_EXECUTABLE = "pod";
			var SearchPaths = new string[] {
				"/opt/homebrew/bin",
				"/usr/local/bin",
				"/usr/bin",
			};
			foreach (string path in SearchPaths)
			{
				string podPath = Path.Combine(path, POD_EXECUTABLE);
				if (File.Exists(podPath))
				{
					LogDebug("Found CocoaPods tool in " + podPath);
					return podPath;
				}
			}
			var environment = ReadGemsEnvironment();
			if (environment == null) return null;
			const string executableDir = "EXECUTABLE DIRECTORY";
			foreach (string environmentVariable in new[] { executableDir, "GEM PATHS" })
			{
				List<string> paths;
				if (environment.TryGetValue(environmentVariable, out paths))
				{
					foreach (var path in paths)
					{
						var binPath = environmentVariable == executableDir ? path :
							Path.Combine(path, "bin");
						var podPath = Path.Combine(binPath, POD_EXECUTABLE);
						if (File.Exists(podPath))
						{
							LogDebug("Found CocoaPods tool in " + podPath);
							return podPath;
						}
					}
				}
			}
			LogWarning("Pod command not found. " +
					"You can install CocoaPods with the Ruby gem package manager:\n" +
					" > sudo gem install -n /usr/local/bin cocoapods");
			return POD_EXECUTABLE;
		}

		/// <summary>
		/// Read the Gems environment.
		/// </summary>
		/// <returns>Dictionary of environment properties or null if there was a problem reading
		/// the environment.</returns>
		private static Dictionary<string, List<string>> ReadGemsEnvironment()
		{
			int ExitCode;
			var result = Utils.RunLocalProcessAndReturnStdOut("gem", "environment", out ExitCode);
			if (ExitCode != 0) return null;
			// gem environment outputs YAML for all config variables.  Perform some very rough YAML
			// parsing to get the environment into a usable form.
			var gemEnvironment = new Dictionary<string, List<string>>();
			const string listItemPrefix = "- ";
			int previousIndentSize = 0;
			List<string> currentList = null;
			char[] listToken = new char[] { ':' };
			foreach (var line in result.Split(new char[] { '\r', '\n' }))
			{
				var trimmedLine = line.Trim();
				var indentSize = line.Length - trimmedLine.Length;
				if (indentSize < previousIndentSize) currentList = null;

				if (trimmedLine.StartsWith(listItemPrefix))
				{
					trimmedLine = trimmedLine.Substring(listItemPrefix.Length).Trim();
					if (currentList == null)
					{
						var tokens = trimmedLine.Split(listToken);
						currentList = new List<string>();
						gemEnvironment[tokens[0].Trim()] = currentList;
						var value = tokens.Length == 2 ? tokens[1].Trim() : null;
						if (!string.IsNullOrEmpty(value))
						{
							currentList.Add(value);
							currentList = null;
						}
					}
					else if (indentSize >= previousIndentSize)
					{
						currentList.Add(trimmedLine);
					}
				}
				else
				{
					currentList = null;
				}
				previousIndentSize = indentSize;
			}
			return gemEnvironment;
		}

		private void CreatePodfile(XCodeConfig Config)
		{
			using (StreamWriter f = new StreamWriter(Path.Combine(NativePath, "Podfile")))
			{
				//f.WriteLine("source 'https://cdn.cocoapods.org/'");
				f.WriteLine("source 'https://github.com/CocoaPods/Specs.git'");
				f.WriteLine("source 'https://github.com/cleveradssolutions/CAS-Specs.git'");
				f.WriteLine("platform :ios, '" + MINIMUM_IOS_VERSION + "'");
				f.WriteLine("use_frameworks! :linkage => :static");
				f.WriteLine("$cas_version = '" + version + "'");
				f.WriteLine();

				f.WriteLine("target '" + IOS_FRAMEWORKS_PROJECT + "' do");
				f.WriteLine("  pod 'CleverAdsSolutions-Base', $cas_version");
				for (int i = 0; i < Config.adapters.Length; i++)
				{
					f.WriteLine("  pod '" + FindAdapter(Config.adapters[i]).dependency + "', $cas_version");
				}
				f.WriteLine("end");
			}
			LogDebug("Podfile created");
		}
	}

	public class XCodeConfig
	{
		private const string FILE_NAME = "CASXCConfig.cascache";
		public string version;
		public string[] adapters;
		public List<XCodeBundle> bundles;
		public string[] sysLibs;
		public string[] sysFrameworks;
		public string[] sysWeakFrameworks;

		public static XCodeConfig Read(string NativePath)
		{
			var ConfigPath = Path.Combine(NativePath, FILE_NAME);
			if (!File.Exists(ConfigPath))
				return null;
#if UE_5_0_OR_LATER
			XElement xml = XElement.Load(ConfigPath);
			return new XCodeConfig
			{
				version = xml.Attribute("version").Value,
				adapters = xml.Element("adapters").Elements("adapter").Select(e => e.Value).ToArray(),
				bundles = xml.Element("bundles").Elements("bundle").Select(b =>
					new XCodeBundle(b.Attribute("name").Value, b.Attribute("bundle").Value)
				).ToList(),
				sysLibs = xml.Element("sysLibs").Elements("lib").Select(e => e.Value).ToArray(),
				sysFrameworks = xml.Element("sysFrameworks").Elements("lib").Select(e => e.Value).ToArray(),
				sysWeakFrameworks = xml.Element("sysWeakFrameworks").Elements("lib").Select(e => e.Value).ToArray()
			};
#else
			var FileName = new FileReference(ConfigPath);
			string Text = FileReference.ReadAllText(FileName);
			return fastJSON.JSON.Instance.ToObject<XCodeConfig>(Text);
#endif
		}

		public void Save(string NativePath)
		{
			var ConfigPath = Path.Combine(NativePath, FILE_NAME);
#if UE_5_0_OR_LATER
			var xml = new XElement("XCodeConfig",
				new XAttribute("version", version),
				new XElement("adapters", adapters.Select(a => new XElement("adapter", a))),
				new XElement("bundles", bundles.Select(b =>
					new XElement("bundle",
						new XAttribute("name", b.name),
						new XAttribute("bundle", b.bundle)
					)
				)),
				new XElement("sysLibs", sysLibs.Select(l => new XElement("lib", l))),
				new XElement("sysFrameworks", sysFrameworks.Select(f => new XElement("lib", f))),
				new XElement("sysWeakFrameworks", sysWeakFrameworks.Select(w => new XElement("lib", w)))
			);
			xml.Save(ConfigPath);
#else
			File.WriteAllText(ConfigPath, fastJSON.JSON.Instance.ToJSON(this));
#endif
		}

		public bool IsSetupReady(string FrameworksPath, string Version, List<string> IncludedAdapters)
		{
			if (version != Version)
			{
				LogDebug("Config framework version not mach");
				return false;
			}
			if (adapters.Length != IncludedAdapters.Count)
			{
				LogDebug("Config framework for adapters not mach");
				return false;
			}
			var readyAdapters = new HashSet<string>(adapters);
			for (int i = 0; i < IncludedAdapters.Count; i++)
			{
				if (!readyAdapters.Contains(IncludedAdapters[i]))
				{
					LogDebug("Config framework for adapter " + IncludedAdapters[i] + " missing");
					return false;
				}
			}
			foreach (var bundle in bundles)
			{
				if (!Directory.Exists(Path.Combine(FrameworksPath, bundle.name + ".framework")))
				{
					LogDebug("Config framework " + bundle.name + " missing");
					return false;
				}
				if (!string.IsNullOrEmpty(bundle.bundle)
					&& !Directory.Exists(Path.Combine(FrameworksPath, bundle.bundle)))
				{
					LogDebug("Config framework " + bundle.name + " missing");
					return false;
				}
			}

			return true;
		}

		public void FindDependencies(CASIOSConfig Handler, string BuildDir)
		{
			string DataPath = Path.Combine(BuildDir, "Build", "Products", "Release-iphoneos");
			string BuildMainDir = Path.Combine(DataPath, IOS_FRAMEWORKS_PROJECT + ".framework");
			if (!Directory.Exists(BuildMainDir))
				CancelBuild("Build XCProject corrupted. Fix errors in console and try again.\n" + BuildMainDir);

			var ActiveFrameworkSet = new HashSet<string>();
			foreach (var item in FindBundles(".framework", DataPath))
			{
				if (item.EndsWith(IOS_FRAMEWORKS_PROJECT + ".framework")) continue;
				string frameworkName = Path.GetFileNameWithoutExtension(item);
				LogDebug("Framework found: " + frameworkName);
				string destination = Path.Combine(Handler.FrameworksPath, frameworkName + ".framework");
				Directory.Move(item, destination);
				ActiveFrameworkSet.Add(frameworkName);
			}

			foreach (var item in FindBundles(".a", DataPath, SkipExt: ".framework"))
			{
				string libName = Path.GetFileNameWithoutExtension(item).Substring(3);
				LogDebug("Library found: " + libName);
				string destination = Path.Combine(Handler.FrameworksPath, libName + ".framework");
				Directory.CreateDirectory(destination);
				destination = Path.Combine(destination, libName);
				File.Move(item, destination);
				ActiveFrameworkSet.Add(libName);
			}


			var podsXCConfig = Path.Combine(Handler.NativePath, "Pods", "Target Support Files", "Pods-" + IOS_FRAMEWORKS_PROJECT, "Pods-" + IOS_FRAMEWORKS_PROJECT + ".release.xcconfig");
			if (!File.Exists(podsXCConfig))
				CancelBuild("Build XCProject corrupted. Not found required build file: " + podsXCConfig);
			FindSystemDependencies(ActiveFrameworkSet, podsXCConfig);

			// Call last with removed bundles from ActiveFrameworkSet
			foreach (var item in FindBundles(".bundle", BuildMainDir))
			{
				string bundleName = Path.GetFileName(item);
				string frameworkName = FindResourcesOwner(bundleName, ActiveFrameworkSet);
				ActiveFrameworkSet.Remove(frameworkName);
				LogDebug("Resources found: " + bundleName);
				string destination = Path.Combine(Handler.FrameworksPath, bundleName);
				Directory.Move(item, destination);
				bundles.Add(new XCodeBundle(frameworkName, bundleName));
			}

			foreach (var item in ActiveFrameworkSet)
			{
				bundles.Add(new XCodeBundle(item, ""));
			}
		}

		public void Apply(CASIOSConfig Handler, CleverAdsSolutions Module)
		{
			string BridgePath = Path.Combine(Handler.NativePath, "Plugin", IOS_BRIDGE_NAME + ".embeddedframework.zip");
			if (!File.Exists(BridgePath))
			{
				throw new FileNotFoundException("CAS.AI iOS Plugin corrupted try install again.", BridgePath);
			}

			Module.PublicAdditionalFrameworks.Add(
				new Framework(IOS_BRIDGE_NAME, BridgePath)
			);

			foreach (var bundle in bundles)
			{
				Module.PublicAdditionalFrameworks.Add(bundle.ToUFramework(Handler));
			}

			Module.PublicSystemLibraries.AddRange(sysLibs);
			Module.PublicFrameworks.AddRange(sysFrameworks);
			Module.PublicWeakFrameworks.AddRange(sysWeakFrameworks);

			AddSwiftSupport(Module);
		}

		private void AddSwiftSupport(CleverAdsSolutions Module)
		{
			// Source: /Engine/Source/ThirdParty/Apple/Swift/Swift.Build.cs
#if UE_5_0_OR_LATER
			// Add support for linking with Swift frameworks
			Module.PrivateDependencyModuleNames.Add("Swift");
#else
			string XcodeRoot = Utils.RunLocalProcessAndReturnStdOut("/usr/bin/xcode-select", "--print-path");
			string SwiftStandardLibraryLinkPath = XcodeRoot + "/Toolchains/XcodeDefault.xctoolchain/usr/lib/swift/iphoneos";

			// Enable Swift
			Module.PublicSystemLibraryPaths.AddRange(
				new string[] {
					SwiftStandardLibraryLinkPath,
					"/usr/lib/swift"
				}
			);
#endif
		}

		private IEnumerable<string> FindBundles(string Extension, string dir, bool Recursive = true, string SkipExt = "..")
		{
			string[] files = Directory.GetFiles(dir);
			foreach (string file in files)
			{
				if (file.EndsWith(Extension))
					yield return file;
			}

			string[] dirs = Directory.GetDirectories(dir);
			foreach (string subdirectory in dirs)
			{
				if (subdirectory.EndsWith(Extension))
				{
					yield return subdirectory;
				}
				else if (Recursive && !subdirectory.EndsWith(SkipExt))
				{
					foreach (var item in FindBundles(Extension, subdirectory, Recursive, SkipExt))
						yield return item;
				}
			}
		}


		private string FindResourcesOwner(string BundleName, HashSet<string> ActiveFrameworkSet)
		{
			if (BundleName.StartsWith("CASBase"))
				BundleName = "CleverAdsSolutions";
			if (BundleName.StartsWith("MobileAdsBundle"))
				BundleName = "YandexMobileAds";
			foreach (var item in ActiveFrameworkSet)
			{
				if (BundleName.StartsWith(item))
					return item;
			}
			CancelBuild(BundleName + " is not associated with any framework. Please update Build logic to fix it.");
			return null;
		}

		private void FindSystemDependencies(HashSet<string> FrameworkSet, string podsXCConfig)
		{
			string[] otherLDFlags = null;
			using (StreamReader reader = new StreamReader(podsXCConfig))
			{
				string line;
				while ((line = reader.ReadLine()) != null)
				{
					if (line.StartsWith("OTHER_LDFLAGS"))
					{
						otherLDFlags = line.Split('-');
						break;
					}
				}
			}
			if (otherLDFlags == null)
				CancelBuild("Build XCProject corrupted. Not found OTHER_LDFLAGS field in build file: " + podsXCConfig);

			var sysLibs = new List<string>();
			var sysFrameworks = new HashSet<string>();
			var sysWeakFrameworks = new List<string>();
			for (int i = 0; i < otherLDFlags.Length; i++)
			{
				var lib = otherLDFlags[i].Trim(' ');
				if (lib.StartsWith("l"))
				{
					lib = lib.Substring(2, lib.Length - 1 - 2);
					if (!FrameworkSet.Contains(lib))
					{
						sysLibs.Add(lib);
					}
				}
				else if (lib.StartsWith("framework"))
				{
					// Substring name from: '-framework "CleverAdsSolutions"'
					// 11 = "framework \"".Length
					lib = lib.Substring(11, lib.Length - 1 - 11);
					if (!FrameworkSet.Contains(lib))
					{
						sysFrameworks.Add(lib);
					}
				}
			}
			for (int i = 0; i < otherLDFlags.Length; i++)
			{
				var lib = otherLDFlags[i].Trim(' ');
				if (lib.StartsWith("weak_framework"))
				{
					// Substring name from: '-weak_framework "AdSupport"'
					// 16 = "weak_framework \"".Length
					lib = lib.Substring(16, lib.Length - 1 - 16);
					if (!FrameworkSet.Contains(lib) && !sysFrameworks.Contains(lib))
						sysWeakFrameworks.Add(lib);
				}
			}
			this.sysLibs = sysLibs.ToArray();
			this.sysFrameworks = sysFrameworks.ToArray();
			this.sysWeakFrameworks = sysWeakFrameworks.ToArray();
		}

	}
	public class XCodeBundle
	{
		public string name;
		public string bundle;

		public XCodeBundle(string name, string bundle)
		{
			this.name = name;
			this.bundle = bundle;
		}

		public Framework ToUFramework(CASIOSConfig Handler)
		{
			string CopyBundle = null;
			if (!string.IsNullOrEmpty(bundle))
				CopyBundle = bundle;
			bool CopyFramework = Handler.SupportedDynamicFrameworks.Contains(name);
			return new Framework(name, Handler.FrameworksPath, CopyBundle, CopyFramework);
		}
	}
}
