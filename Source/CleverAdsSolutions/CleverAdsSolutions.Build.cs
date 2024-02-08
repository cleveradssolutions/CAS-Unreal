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
	private const string LogTag = "[CAS.AI] ";

	/// <summary>
	/// Add [CASPluginBuildConfig] section to any Config Engine.ini file 
	/// to configure CAS plugin build behaviour.
	/// </summary>
	private const string BuildConfigSection = "CASPluginBuildConfig";

	/// <summary>
	/// Array of framework names to exclude from module rules: 
	/// PublicAdditionalFrameworks, PublicFrameworks, PublicWeakFrameworks.
	/// For example Config/DefaultEngine.ini:
	/// [CASPluginBuildConfig]
	/// +ExcludeFrameworks="TestFrameworkNameWithoutExtension"
	/// </summary>
	private const string BuildConfigExcludeFrameworks = "ExcludeFrameworks";

	/// <summary>
	/// If you have problems auto-populating the Google App ID by CAS Plugin, 
	/// then you can define Google App IDs for each platform in the Engine.ini file.
	/// For example Config/DefaultEngine.ini:
	/// [CASPluginBuildConfig]
	/// GoogleAppIdIOS=ca-app-pub-0000000000000000~0000000000
	/// GoogleAppIdAndroid=ca-app-pub-0000000000000000~0000000000
	/// </summary>
	private const string BuildConfigGoogleAppId = "GoogleAppId"; // + PlatformName;

	private const string IOSMinimumVersion = "13.0";
	private const string IOSBridgeName = "CASUnrealBridge";
	private const string IOSFrameworksName = "CASFrameworks";
	private const string IOSFameworksWorkspace = IOSFrameworksName + ".xcworkspace";
	private const string EngineConfigSection = "/Script/CleverAdsSolutions.CASDefaultConfig";

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
				"Projects"
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
			new AndroidHandler().ApplyToModule(this, Target);
		}
		else if (Target.Platform == UnrealTargetPlatform.IOS)
		{
			new IOSHandler().ApplyToModule(this, Target);
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
		Log.Logger.LogInformation(LogTag + message);
#else
		Log.TraceInformation(LOG_PREFIX + message);
#endif
	}

	private static void LogWarning(string message)
	{
		// Logger implementation not support collored warnings
		//Log.Logger.LogWarning(LOG_PREFIX + message);
		Log.WriteLineOnce(LogEventType.Warning, LogTag + message);
	}

	private static void CancelBuild(string Error)
	{
		throw new BuildException(Error);
	}

	private static void LostRequiredFile(string File)
	{
		throw new BuildException("Lost required plugin component. Try reinstal CAS plugin and check file exists: " + File);
	}

	private static string WrapInQuotes(string Line)
	{
		return "\"" + Line + "\"";
	}

	public abstract class BaseHandler
	{
		public string version;
		public CASNetwork[] solutions;
		public CASNetwork[] adapters;

		public ConfigHierarchy EngineConfig;

		/// <summary>
		/// Platform names: Android, IOS
		/// </summary>
		public string PlatformName;
		public DirectoryReference NativeDir;
		public string ManagerID;
		public bool TestAdMode;
		public bool ShippingMode;
		public string CacheConfigFileName;
		public FileReference CacheConfigFile;

		public int RunProcess(string Name, params string[] Args)
		{
			var ArgsLine = string.Join(" ", Args);
			LogDebug("Run process: " + Name + " " + ArgsLine + " ...");
			var Info = new ProcessStartInfo(Name, ArgsLine);
			Info.WorkingDirectory = NativeDir.FullName;
			// Warning: CocoaPods requires your terminal to be using UTF-8 encoding.
			Info.StandardOutputEncoding = Encoding.UTF8;
			Info.StandardErrorEncoding = Encoding.UTF8;
			Info.Environment["LANG"] = "en_US.UTF-8";

			var LocalProcess = new Process();
			LocalProcess.StartInfo = Info;
			LocalProcess.OutputDataReceived += LocalProcessOutput;
			// Some minor warnings will be received as errors. 
			LocalProcess.ErrorDataReceived += LocalProcessOutput;
			return Utils.RunLocalProcess(LocalProcess);
		}

		public string RunProcessForResult(string Name, params string[] Args)
		{
			var ArgsLine = string.Join(" ", Args);
			LogDebug("Run process: " + Name + " " + ArgsLine + " ...");
			return Utils.RunLocalProcessAndReturnStdOut(Name, ArgsLine);
		}

		private static void LocalProcessOutput(object Sender, DataReceivedEventArgs Args)
		{
			if (Args != null && Args.Data != null)
			{
				LogDebug(Args.Data.TrimEnd());
			}
		}

		public virtual void ApplyToModule(CleverAdsSolutions Module, ReadOnlyTargetRules Target)
		{
			// module.ModuleDirectory = Plugins/CleverAdsSolutions/Source/CleverAdsSolutions
			PlatformName = Target.Platform.ToString();
			LogDebug(PlatformName + " Plugin build configuration");
			NativeDir = new DirectoryReference(Path.Combine(Module.ModuleDirectory, "..", "ThirdParty", PlatformName));
			ShippingMode = Target.Configuration == UnrealTargetConfiguration.Shipping;

			string ModuleDirectoryRelative = Utils.MakePathRelativeTo(Module.ModuleDirectory, Target.RelativeEnginePath);
			string UPLFileName = "CAS_UPL_" + PlatformName + ".xml";
			string UPLFilePath = Path.Combine(Module.ModuleDirectory, UPLFileName);
			if (!File.Exists(UPLFilePath))
				LostRequiredFile(UPLFilePath);


			string ListPlatform = Target.Platform == UnrealTargetPlatform.IOS ? "iOS" : PlatformName;
			var MediationListFile = FileReference.Combine(NativeDir, "CAS" + ListPlatform + "Mediation.list");
			if (!FileReference.Exists(MediationListFile))
				LostRequiredFile(MediationListFile.FullName);

			Module.PublicDefinitions.Add("WITH_CAS=1");
			Module.AdditionalPropertiesForReceipt.Add(PlatformName + "Plugin", Path.Combine(ModuleDirectoryRelative, UPLFileName));

			JsonObject ConfigJson = JsonObject.Read(MediationListFile);
			version = ConfigJson.GetStringField("version");
			solutions = ParseNetworksList(ConfigJson, "simple");
			adapters = ParseNetworksList(ConfigJson, "adapters");

			var ProjectDirRef = DirectoryReference.FromFile(Target.ProjectFile);
			EngineConfig = ConfigCache.ReadHierarchy(ConfigHierarchyType.Engine, ProjectDirRef, Target.Platform);

			EngineConfig.TryGetValue(EngineConfigSection, "CASAppID", out ManagerID);
			if (string.IsNullOrEmpty(ManagerID))
			{
				ManagerID = "demo";
			}
			if (!EngineConfig.TryGetValue(EngineConfigSection, "TestAdsMode", out TestAdMode))
			{
				TestAdMode = true;
			}
			for (int i = 0; i < solutions.Length; i++)
			{
				solutions[i].Configure(EngineConfig);
			}
			for (int i = 0; i < adapters.Length; i++)
			{
				adapters[i].Configure(EngineConfig);
			}

			if (string.IsNullOrEmpty(CacheConfigFileName))
			{
				string SuffixChar = char.ToLower(ManagerID[ManagerID.Length - 1]).ToString();
				string suffix = ManagerID.Length.ToString() + SuffixChar;
				CacheConfigFileName = "cas_settings" + suffix + ".json";
			}
			CacheConfigFile = FileReference.Combine(NativeDir, CacheConfigFileName);

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
			using (var Reader = File.OpenText(UPLFileFullPath))
			{
				const string StringLine = "<setString result=";
				string GoogleAppIdKey = StringLine + WrapInQuotes("GoogleAppId");
				string ConfigResKey = StringLine + WrapInQuotes("CASConfigResFile");
				bool GoogleAppIdUpdate = true;
				bool ConfigResUpdate = true;
				string Line;
				while ((Line = Reader.ReadLine()) != null)
				{
					var TrimedLine = Line.TrimStart();
					if (GoogleAppIdUpdate && TrimedLine.StartsWith(GoogleAppIdKey))
					{
						string GoogleAppId = GetGoogleAppIdFromConfig();
						if (GoogleAppId == null)
							UPLFile.Add(Line);
						else
							UPLFile.Add("\t\t" + GoogleAppIdKey + " value=" + WrapInQuotes(GoogleAppId) + "/>");
						GoogleAppIdUpdate = false;
					}
					else if (ConfigResUpdate && TrimedLine.StartsWith(ConfigResKey))
					{
						UPLFile.Add("\t\t" + ConfigResKey + " value=" + WrapInQuotes(CacheConfigFileName) + "/>");
						ConfigResUpdate = false;
					}
					else
					{
						UPLFile.Add(Line);
					}
				}
			}

			using (var Writer = File.CreateText(UPLFileFullPath))
			{
				for (int i = 0; i < UPLFile.Count; i++)
					Writer.WriteLine(UPLFile[i]);
			}
		}

		private string GetGoogleAppIdFromConfig()
		{
			if (FileReference.Exists(CacheConfigFile))
			{
				var Config = FileUtils.ReadAllText(CacheConfigFile);
				var AppId = FindPropertyString(Config, "admob_app_id");
				if (AppId != null && AppId.Contains('~'))
				{
					LogDebug("Apply Google App ID: " + AppId);
					string CustomGoogleAppId;
					if (EngineConfig.GetString(BuildConfigSection, BuildConfigGoogleAppId + PlatformName, out CustomGoogleAppId))
					{
						if (CustomGoogleAppId != AppId)
							CancelBuild("You have specified the incorrect Google App ID in the Engine.ini file. Please ensure you have inserted the correct App ID: " + AppId);
					}
					return AppId;
				}
				if (FindAdapter("GoogleAds").included)
				{
					string Warning = "Configuration for '" + ManagerID + "' is not valid, please contact support for additional information.";
					if (ShippingMode)
						CancelBuild(Warning);
					else
						LogWarning(Warning);
				}
			}
			return null;
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

		private void DownloadConfig(ReadOnlyTargetRules Target)
		{
			var InvalidIdMessage = "CAS App Id '" + ManagerID + "' is not registered in CAS.AI. " +
				"Please try using a real CAS identifier in the `Project Settings > CAS.AI` menu or contact support.";
			if (!ShippingMode)
				InvalidIdMessage += "\nIf you haven't created an CAS account and registered an app yet, use Test Ads mode to continue build (If not Shipping build only).";

			if (string.IsNullOrEmpty(ManagerID) || ManagerID == "demo")
			{
				if (!TestAdMode || ShippingMode)
					CancelBuild(InvalidIdMessage);
				else
					LogDebug("Configuration not loaded for invalid CAS App Id and Test Ad Mode.");
				return;
			}

			var TempConfigFilePath = new FileReference(CacheConfigFile.FullName + ".temp");
			if (FileReference.Exists(CacheConfigFile))
			{
				if (FileReference.GetLastWriteTime(CacheConfigFile).AddHours(12) > DateTime.Now)
				{
					LogDebug("Configuration used from cache for " + ManagerID);
					return;
				}
				FileUtils.ForceMoveFile(CacheConfigFile, TempConfigFilePath);
			}

			var RequestUrl = "https://psvpromo.psvgamestudio.com/cas-settings.php?apply=config&platform=" +
				(Target.Platform == UnrealTargetPlatform.Android ? "0" : "1") +
				"&bundle=" + ManagerID;

			var Response = RunProcessForResult("curl",
				"-w", WrapInQuotes("%{http_code}"),
				"-o", WrapInQuotes(CacheConfigFile.FullName),
				WrapInQuotes(RequestUrl)
			);
			Response = Response.Trim(' ', '"');

			if (Response.StartsWith("200") || Response.EndsWith("200"))
			{
				LogDebug("Configuration loaded for " + ManagerID);
				if (FileReference.Exists(TempConfigFilePath))
					FileUtils.ForceDeleteFile(TempConfigFilePath);
				return;
			}
			try
			{
				FileUtils.ForceDeleteFile(CacheConfigFile);
			}
			catch (Exception e)
			{
				LogWarning("Failed to remove invalid configuration:" + e.ToString());
			}
			if (Response.StartsWith("204") || Response.EndsWith("204"))
			{
				if (TestAdMode)
				{
					LogWarning(InvalidIdMessage);
					return;
				}
				CancelBuild(InvalidIdMessage);
			}

			LogWarning("Failed to load configuration: " + Response);
			if (FileReference.Exists(TempConfigFilePath))
			{
				FileUtils.ForceMoveFile(TempConfigFilePath, CacheConfigFile);
				LogDebug("Configuration restored from cache for " + ManagerID);
				return;
			}

			if (ShippingMode)
			{
				CancelBuild(
					"Configuration file not found, please contact support for additional information. " +
					"If you have a configuration file, move it along the path: " + CacheConfigFile);
			}
		}

		private static string FindPropertyString(string Source, string Property)
		{
			int Index = Source.IndexOf(Property);
			if (Index < 0) return null;
			Index += Property.Length + 1;
			while (Source[Index] != '"')
				Index++;
			Index++;
			return Source.Substring(Index, Source.IndexOf('"', Index) - Index);
		}
	}

	private class AndroidHandler : BaseHandler
	{
		public override void ApplyToModule(CleverAdsSolutions module, ReadOnlyTargetRules Target)
		{
			module.PublicDependencyModuleNames.Add("Launch"); // Includes Android JNI support
			base.ApplyToModule(module, Target);
			string PluginLibrary = Path.Combine(NativeDir.FullName, "repository", "com", "cleveradssolutions", "cas-unreal-plugin", "release", "cas-unreal-plugin-release.aar");
			if (!File.Exists(PluginLibrary))
				LostRequiredFile(PluginLibrary);
		}
	}

	public class IOSHandler : BaseHandler
	{
		public DirectoryReference FrameworksDir;
		public string BridgeFrameworkPath;
		public List<string> IncludedAdapters = new List<string>();
		public HashSet<string> SupportedDynamicFrameworks = new HashSet<string>();

		public override void ApplyToModule(CleverAdsSolutions Module, ReadOnlyTargetRules Target)
		{
			base.ApplyToModule(Module, Target);

#if !UE_5_2_OR_LATER
			// Starting from version 5.2, Unreal Engine no longer utilizes Bitcode in iOS builds.
			const string ConfigRuntimeSettings = "/Script/IOSRuntimeSettings.IOSRuntimeSettings";
			bool BuildWithBitcode;
			if (!EngineConfig.TryGetValue(ConfigRuntimeSettings, "bShipForBitcode", out BuildWithBitcode) || BuildWithBitcode)
			{
				CancelBuild("Building with bitcode is no longer supported. " +
					"Disable Bitcode in iOS Runtime Settings window.");
			}
#endif

#if UE_5_3_OR_LATER
			const string XCodeSettings = "/Script/MacTargetPlatform.XcodeProjectSettings";
			bool BuildModernXCode;
			if (!EngineConfig.TryGetValue(XCodeSettings, "bUseModernXcode", out BuildModernXCode) || BuildModernXCode)
			{
				if (FileReference.Exists(CacheConfigFile))
				{
					LogWarning("The Modern XCode build does not copy additional resources correctly.");
					LogWarning("Please add a configuration file in XCode project `App Target > Build Phases > Copy Bundle Resources > +`: " + CacheConfigFile.FullName);
				}
			}
#endif

			// Work only for not Modern Xcode Build UE 5.3+
			// Same as <copy file> in UPL.xml
			//Module.AdditionalBundleResources.Add(new BundleResource(GetResourcesFilePath()));

			FrameworksDir = DirectoryReference.Combine(NativeDir, "Frameworks");

			BridgeFrameworkPath = Path.Combine(NativeDir.FullName, "Plugin", IOSBridgeName + ".embeddedframework.zip");
			if (!File.Exists(BridgeFrameworkPath))
				LostRequiredFile(BridgeFrameworkPath);

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

			try
			{
				var ConfigCache = XCodeConfig.Read(NativeDir);
				if (ConfigCache != null && ConfigCache.IsSetupReady(this))
				{
					ConfigCache.ApplyToModule(Module, this);
					return;
				}
			}
			catch (Exception e)
			{
				LogWarning(e.ToString());
			}

			InstallPods(Module);
		}

		private void InstallPods(CleverAdsSolutions Module)
		{
			var FrameworkProj = Path.Combine(NativeDir.FullName, IOSFrameworksName + ".xcodeproj");
			if (!Directory.Exists(FrameworkProj))
				LostRequiredFile(FrameworkProj);

			var BuildDir = Path.Combine(NativeDir.FullName, "Temp");
			var PodfileLock = FileReference.Combine(NativeDir, "Podfile.lock");
			FileUtils.ForceDeleteFile(PodfileLock);
			FileUtils.ForceDeleteDirectory(BuildDir);
			FileUtils.ForceDeleteDirectory(FrameworksDir);
			FileUtils.CreateDirectoryTree(FrameworksDir);

			var Config = new XCodeConfig();
			Config.version = version;
			Config.adapters = IncludedAdapters.ToArray();

			CreatePodfile(Config);

			string PodTool = CocoaPods.FindPodTool();
			RunProcess(PodTool, "update");

			var XCWorkspacePath = Path.Combine(NativeDir.FullName, IOSFameworksWorkspace);
			if (!Directory.Exists(XCWorkspacePath))
				CancelBuild("Pod installation failed. Fix errors in console logs and try again.");

			BuildPods(BuildDir);

			Config.FindDependencies(this, BuildDir);
			Config.ApplyToModule(Module, this);
			Config.Save(NativeDir);

			RunProcess(PodTool, "deintegrate");
			FileUtils.ForceDeleteDirectory(XCWorkspacePath);
			FileUtils.ForceDeleteFile(PodfileLock);
			FileUtils.ForceDeleteDirectory(BuildDir);
		}

		private void BuildPods(string BuildDir)
		{
			RunProcess("xcodebuild", "build",
				"-workspace", IOSFameworksWorkspace,
				"-scheme", IOSFrameworksName,
				"-derivedDataPath", WrapInQuotes(BuildDir),
				"-sdk", "iphoneos",
				"-destination", WrapInQuotes("generic/platform=iOS"),
				"-configuration", "Release",
				"IPHONEOS_DEPLOYMENT_TARGET=" + IOSMinimumVersion,
				"GCC_GENERATE_DEBUGGING_SYMBOLS=NO"
			);
		}

		private void CreatePodfile(XCodeConfig Config)
		{
			var Path = FileReference.Combine(NativeDir, "Podfile");
			using (StreamWriter f = File.CreateText(Path.FullName))
			{
				//f.WriteLine("source 'https://cdn.cocoapods.org/'");
				f.WriteLine("source 'https://github.com/CocoaPods/Specs.git'");
				f.WriteLine("source 'https://github.com/cleveradssolutions/CAS-Specs.git'");
				f.WriteLine("platform :ios, '" + IOSMinimumVersion + "'");
				f.WriteLine("use_frameworks! :linkage => :static");
				f.WriteLine("$cas_version = '" + version + "'");
				f.WriteLine();

				f.WriteLine("target '" + IOSFrameworksName + "' do");
				f.WriteLine("  pod 'CleverAdsSolutions-Base', $cas_version");
				for (int i = 0; i < Config.adapters.Length; i++)
				{
					f.WriteLine("  pod '" + FindAdapter(Config.adapters[i]).dependency + "', $cas_version");
				}
				f.WriteLine("end");
			}
			LogDebug("Created " + Path);
		}
	}

	public class CASNetwork
	{
		public bool included = false;
		public string name;
		public string version;
		public string dependency;
		public string embedLib;

		public CASNetwork(string name)
		{
			this.name = name;
			version = "";
			dependency = "";
		}

		public CASNetwork(JsonObject json)
		{
			this.name = json.GetStringField("name");
			json.TryGetStringField("version", out this.version);
			json.TryGetStringField("dependency", out this.dependency);
			json.TryGetStringField("embedLib", out this.embedLib);
		}

		public void Configure(ConfigHierarchy EngineConfig)
		{
			EngineConfig.TryGetValue(EngineConfigSection, "Include" + name, out included);
		}
	}

	public class XCodeConfig
	{
		private const string FILE_NAME = "CASXCConfig.temp";

		public string version;
		public List<XCodeBundle> bundles = new List<XCodeBundle>();
		public string[] adapters;
		public string[] sysLibs;
		public string[] sysFrameworks;
		public string[] sysWeakFrameworks;

		public static XCodeConfig Read(DirectoryReference NativePath)
		{
			var ConfigPath = FileReference.Combine(NativePath, FILE_NAME);
			if (!FileReference.Exists(ConfigPath))
				return null;
#if UE_5_0_OR_LATER
			XElement xml = XElement.Load(ConfigPath.FullName);
			return new XCodeConfig
			{
				version = xml.Attribute("version").Value,
				bundles = xml.Element("bundles").Elements("bundle").Select(b =>
					new XCodeBundle(b.Attribute("name").Value)
					{
						bundle = b.Attribute("bundle").Value
					}
				).ToList(),
				adapters = xml.Element("adapters").Elements("adapter").Select(e => e.Value).ToArray(),
				sysLibs = xml.Element("sysLibs").Elements("lib").Select(e => e.Value).ToArray(),
				sysFrameworks = xml.Element("sysFrameworks").Elements("lib").Select(e => e.Value).ToArray(),
				sysWeakFrameworks = xml.Element("sysWeakFrameworks").Elements("lib").Select(e => e.Value).ToArray()
			};
#else
			return Json.Deserialize<XCodeConfig>(FileUtils.ReadAllText(ConfigPath));
#endif
		}

		public void Save(DirectoryReference NativePath)
		{
			var ConfigPath = FileReference.Combine(NativePath, FILE_NAME);
#if UE_5_0_OR_LATER
			var xml = new XElement("XCodeConfig",
				new XAttribute("version", version),
				new XElement("bundles", bundles.Select(b =>
					new XElement("bundle",
						new XAttribute("name", b.name),
						new XAttribute("bundle", b.bundle)
					)
				)),
				new XElement("adapters", adapters.Select(a => new XElement("adapter", a))),
				new XElement("sysLibs", sysLibs.Select(l => new XElement("lib", l))),
				new XElement("sysFrameworks", sysFrameworks.Select(f => new XElement("lib", f))),
				new XElement("sysWeakFrameworks", sysWeakFrameworks.Select(w => new XElement("lib", w)))
			);
			xml.Save(ConfigPath.FullName);
#else
			FileReference.WriteAllText(ConfigPath, Json.Serialize(this, JsonSerializeOptions.None));
#endif
		}

		public bool IsSetupReady(IOSHandler Handler)
		{
			const string LogPrefix = "XCode Config ";
			if (version != Handler.version)
			{
				LogDebug(LogPrefix + "version not mach");
				return false;
			}
			if (adapters.Length != Handler.IncludedAdapters.Count)
			{
				LogDebug(LogPrefix + "adapters not mach");
				return false;
			}
			var readyAdapters = new HashSet<string>(adapters);
			for (int i = 0; i < Handler.IncludedAdapters.Count; i++)
			{
				if (!readyAdapters.Contains(Handler.IncludedAdapters[i]))
				{
					LogDebug(LogPrefix + Handler.IncludedAdapters[i] + " adapter missing");
					return false;
				}
			}
			foreach (var bundle in bundles)
			{
				if (!Directory.Exists(bundle.GetFrameworkPath(Handler)))
				{
					LogDebug(LogPrefix + bundle.name + ".framework missing");
					return false;
				}
				if (!string.IsNullOrEmpty(bundle.bundle)
					&& !Directory.Exists(bundle.GetResourcesPath(Handler)))
				{
					LogDebug(LogPrefix + bundle.bundle + " missing");
					return false;
				}
			}

			LogDebug(LogPrefix + "apply from cache");
			return true;
		}

		public void FindDependencies(IOSHandler Handler, string BuildDir)
		{
			string DataPath = Path.Combine(BuildDir, "Build", "Products", "Release-iphoneos");
			string BuildMainDir = Path.Combine(DataPath, IOSFrameworksName + ".framework");
			if (!Directory.Exists(BuildMainDir))
				CancelBuild("Build XCProject corrupted. Fix errors in console and try again.\n" + BuildMainDir);

			foreach (var item in FindBundles(".framework", DataPath))
			{
				if (item.EndsWith(IOSFrameworksName + ".framework")) continue;
				var bundle = new XCodeBundle(Path.GetFileNameWithoutExtension(item));
				LogDebug("Framework found: " + bundle.name);
				Directory.Move(item, bundle.GetFrameworkPath(Handler));
				bundles.Add(bundle);
			}

			foreach (var item in FindBundles(".a", DataPath, SkipExt: ".framework"))
			{
				var bundle = new XCodeBundle(Path.GetFileNameWithoutExtension(item).Substring(3));
				LogDebug("Library found: " + bundle.name);
				string destination = bundle.GetFrameworkPath(Handler);
				Directory.CreateDirectory(destination);
				File.Move(item, Path.Combine(destination, bundle.name));
				bundles.Add(bundle);
			}

			var podsXCConfig = FileReference.Combine(Handler.NativeDir, "Pods", "Target Support Files", "Pods-" + IOSFrameworksName, "Pods-" + IOSFrameworksName + ".release.xcconfig");
			if (!FileReference.Exists(podsXCConfig))
				CancelBuild("Build XCProject corrupted. Not found required build file: " + podsXCConfig);
			FindSystemDependencies(podsXCConfig.FullName);

			foreach (var item in FindBundles(".bundle", BuildMainDir))
			{
				var bundle = FindResourcesOwner(Path.GetFileName(item));
				LogDebug("Resources found: " + bundle.bundle);
				Directory.Move(item, bundle.GetResourcesPath(Handler));
			}
		}

		public void ApplyToModule(CleverAdsSolutions Module, IOSHandler Handler)
		{
			Module.PublicAdditionalFrameworks.Add(
				new Framework(IOSBridgeName, Handler.BridgeFrameworkPath)
			);

			Module.PublicSystemLibraries.AddRange(sysLibs);

			HashSet<string> ExcludeFrameworks = null;
			List<string> ExcludeFrameworksList;
			if (Handler.EngineConfig.GetArray(BuildConfigSection, BuildConfigExcludeFrameworks, out ExcludeFrameworksList))
			{
				ExcludeFrameworks = new HashSet<string>(ExcludeFrameworksList);
				foreach (var Framework in sysFrameworks)
				{
					if (ExcludeFrameworks.Contains(Framework))
						LogDebug("[Build Config] Excluded System Framework: " + Framework);
					else
						Module.PublicFrameworks.Add(Framework);
				}
				foreach (var Framework in sysWeakFrameworks)
				{
					if (ExcludeFrameworks.Contains(Framework))
						LogDebug("[Build Config] Excluded Weak Framework: " + Framework);
					else
						Module.PublicWeakFrameworks.Add(Framework);
				}
			}
			else
			{
				Module.PublicFrameworks.AddRange(sysFrameworks);
				Module.PublicWeakFrameworks.AddRange(sysWeakFrameworks);
			}

			foreach (var Framework in bundles)
			{
				if (ExcludeFrameworks != null && ExcludeFrameworks.Contains(Framework.name))
				{
					LogDebug("[Build Config] Excluded Additional Framework: " + Framework.name);
					continue;
				}

				var Resources = string.IsNullOrEmpty(Framework.bundle) ? null : Framework.bundle;
				bool CopyFramework = Handler.SupportedDynamicFrameworks.Contains(Framework.name);
				Module.PublicAdditionalFrameworks.Add(
					new Framework(Framework.name, Framework.GetPath(Handler), Resources, CopyFramework)
				);
			}

#if UE_5_0_OR_LATER
			Module.bEnableObjCAutomaticReferenceCounting = true;
#endif

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


		private XCodeBundle FindResourcesOwner(string BundleName)
		{
			if (BundleName.StartsWith("CASBase"))
				BundleName = "CleverAdsSolutions";
			if (BundleName.StartsWith("MobileAdsBundle"))
				BundleName = "YandexMobileAds";
			foreach (var item in bundles)
			{
				if (BundleName.StartsWith(item.name))
				{
					item.bundle = BundleName;
					return item;
				}
			}
			CancelBuild(BundleName + " is not associated with any framework. Please contact support.");
			return null;
		}

		private void FindSystemDependencies(string PodsXCConfig)
		{
			string[] otherLDFlags = null;
			using (var Reader = File.OpenText(PodsXCConfig))
			{
				string line;
				while ((line = Reader.ReadLine()) != null)
				{
					if (line.StartsWith("OTHER_LDFLAGS"))
					{
						otherLDFlags = line.Split('-');
						break;
					}
				}
			}
			if (otherLDFlags == null)
				CancelBuild("Build XCProject corrupted. Not found OTHER_LDFLAGS field in build file: " + PodsXCConfig);

			var sysLibs = new List<string>();
			var sysFrameworks = new HashSet<string>();
			var sysWeakFrameworks = new List<string>();
			var FrameworkSet = new HashSet<string>();
			for (int i = 0; i < bundles.Count; i++)
				FrameworkSet.Add(bundles[i].name);

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

		// Serialization constructor
		public XCodeBundle() { }

		public XCodeBundle(string name)
		{
			this.name = name;
			bundle = "";
		}

		public string GetPath(IOSHandler Handler)
		{
			return Path.Combine(Handler.FrameworksDir.FullName, name);
		}

		public string GetFrameworkPath(IOSHandler Handler)
		{
			var dir = GetPath(Handler);
			if (!Directory.Exists(dir))
				Directory.CreateDirectory(dir);
			return Path.Combine(dir, name + ".framework");
		}

		public string GetResourcesPath(IOSHandler Handler)
		{
			return Path.Combine(Handler.FrameworksDir.FullName, name, bundle);
		}
	}

	public static class CocoaPods
	{
		public static string FindPodTool()
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
					return podPath;
			}
			var environment = ReadGemsEnvironment();
			if (environment != null)
			{
				const string executableDir = "EXECUTABLE DIRECTORY";
				foreach (string environmentVariable in new[] { executableDir, "GEM PATHS" })
				{
					List<string> paths;
					if (environment.TryGetValue(environmentVariable, out paths))
					{
						foreach (var path in paths)
						{
							var binPath = path.EndsWith("bin") ? path : Path.Combine(path, "bin");
							var podPath = Path.Combine(binPath, POD_EXECUTABLE);
							if (File.Exists(podPath))
								return podPath;
						}
					}
				}
			}
			LogWarning("Pod command not found. You can install CocoaPods with the Ruby gem package manager: " +
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
	}
}
