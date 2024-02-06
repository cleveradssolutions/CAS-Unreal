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
	private const string LOG_PREFIX = "[CAS.AI] ";
	private const string MINIMUM_IOS_VERSION = "13.0";

	/// <summary>
	/// Add [CASPluginBuildConfig] section to the Config/DefaultEngine.ini file 
	/// to configure CAS plugin build logic
	/// </summary>
	private const string BuildConfigName = "CASPluginBuildConfig";

	/// <summary>
	/// Array of framework names to exclude from module rules: 
	/// PublicAdditionalFrameworks, PublicFrameworks, PublicWeakFrameworks.
	/// For example Config/DefaultEngine.ini:
	/// [CASPluginBuildConfig]
	/// +ExcludeFrameworks="TestFrameworkNameWithoutExtension"
	/// </summary>
	private const string BuildConfigExcludeFrameworks = "ExcludeFrameworks";

	private const string EngineConfigName = "/Script/CleverAdsSolutions.CASDefaultConfig";
	private const string IOS_BRIDGE_NAME = "CASUnrealBridge";
	private const string IOS_FRAMEWORKS_BUILD_DIR = "Temp";
	private const string IOS_FRAMEWORKS_PROJECT = "CASFrameworks";
	private const string XCWorkspace = IOS_FRAMEWORKS_PROJECT + ".xcworkspace";

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
		Log.Logger.LogInformation(LOG_PREFIX + message);
#else
		Log.TraceInformation(LOG_PREFIX + message);
#endif
	}

	private static void LogWarning(string message)
	{
		// Logger implementation not support collored warnings
		//Log.Logger.LogWarning(LOG_PREFIX + message);
		Log.WriteLineOnce(LogEventType.Warning, LOG_PREFIX + message);
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

		public string NativePath;
		public string ManagerID;
		public bool TestAdMode;
		public bool ShippingMode;
		public string CacheConfigFileName;

		public int RunProcess(string Name, params string[] Args)
		{
			var ArgsLine = string.Join(" ", Args);
			LogDebug("Run process: " + Name + " " + ArgsLine + " ...");
			var Info = new ProcessStartInfo(Name, ArgsLine);
			Info.WorkingDirectory = NativePath;
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
			// Platform name: Android, IOS
			string PlatformName = Target.Platform.ToString();
			LogDebug(PlatformName + " Plugin build configuration");
			NativePath = Path.GetFullPath(Path.Combine(Module.ModuleDirectory, "..", "ThirdParty", PlatformName));
			ShippingMode = Target.Configuration == UnrealTargetConfiguration.Shipping;

			string ModuleDirectoryRelative = Utils.MakePathRelativeTo(Module.ModuleDirectory, Target.RelativeEnginePath);
			string UPLFileName = "CAS_UPL_" + PlatformName + ".xml";
			string UPLFilePath = Path.Combine(Module.ModuleDirectory, UPLFileName);
			if (!File.Exists(UPLFilePath))
				LostRequiredFile(UPLFilePath);

			string listPlatform = Target.Platform == UnrealTargetPlatform.IOS ? "iOS" : PlatformName;
			string MediationPath = Path.Combine(NativePath, "CAS" + listPlatform + "Mediation.list");
			if (!File.Exists(MediationPath))
				LostRequiredFile(MediationPath);

			Module.PublicDefinitions.Add("WITH_CAS=1");
			Module.AdditionalPropertiesForReceipt.Add(PlatformName + "Plugin", Path.Combine(ModuleDirectoryRelative, UPLFileName));

			JsonObject ConfigJson = JsonObject.Read(new FileReference(MediationPath));
			version = ConfigJson.GetStringField("version");
			solutions = ParseNetworksList(ConfigJson, "simple");
			adapters = ParseNetworksList(ConfigJson, "adapters");

			var ProjectDirRef = DirectoryReference.FromFile(Target.ProjectFile);
			EngineConfig = ConfigCache.ReadHierarchy(ConfigHierarchyType.Engine, ProjectDirRef, Target.Platform);

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
				solutions[i].Configure(EngineConfig);
			}
			for (int i = 0; i < adapters.Length; i++)
			{
				adapters[i].Configure(EngineConfig);
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
			using (var Reader = File.OpenText(UPLFileFullPath))
			{
				const string StringLine = "<setString result=";
				string GoogleAppIdKey = StringLine + WrapInQuotes("GoogleAppId");
				string ConfigResKey = StringLine + WrapInQuotes("CASConfigResFile");
				string Line;
				while ((Line = Reader.ReadLine()) != null)
				{
					var TrimedLine = Line.TrimStart();
					if (TrimedLine.StartsWith(GoogleAppIdKey))
					{
						string GoogleAppId = null;
						string CacheConfigFilePath = GetResourcesFilePath();
						if (File.Exists(CacheConfigFilePath))
						{
							var Config = File.ReadAllText(CacheConfigFilePath);
							var AppId = FindPropertyString(Config, "admob_app_id");
							if (AppId != null && AppId.Contains('~'))
							{
								GoogleAppId = AppId;
								LogDebug("Apply Google App ID: " + GoogleAppId);
							}
							else if (FindAdapter("GoogleAds").included)
							{
								string Warning = "Configuration for '" + ManagerID + "' is not valid, please contact support for additional information.";
								if (ShippingMode)
									CancelBuild(Warning);
								else
									LogWarning(Warning);
							}
						}

						if (GoogleAppId == null)
							UPLFile.Add(Line);
						else
							UPLFile.Add("\t\t" + GoogleAppIdKey + " value=" + WrapInQuotes(GoogleAppId) + "/>");
					}
					else if (TrimedLine.StartsWith(ConfigResKey))
					{
						UPLFile.Add("\t\t" + ConfigResKey + " value=" + WrapInQuotes(GetResourcesFileName()) + "/>");
					}
					else
					{
						UPLFile.Add(Line);
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

		public string GetResourcesFileName()
		{
			string suffix = "";
			if (!string.IsNullOrEmpty(ManagerID))
			{
				string SuffixChar = char.ToLower(ManagerID[ManagerID.Length - 1]).ToString();
				suffix = ManagerID.Length.ToString() + SuffixChar;
			}
			return "cas_settings" + suffix + ".json";
		}

		public string GetResourcesFilePath()
		{
			return Path.Combine(NativePath, CacheConfigFileName);
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

			string CacheConfigFilePath = GetResourcesFilePath();
			string TempConfigFilePath = CacheConfigFilePath + ".temp";
			if (File.Exists(CacheConfigFilePath))
			{
				if (File.GetLastWriteTime(CacheConfigFilePath).AddHours(12) > DateTime.Now)
				{
					LogDebug("Configuration used from cache for " + ManagerID);
					return;
				}
				File.Move(CacheConfigFilePath, TempConfigFilePath);
			}

			var RequestUrl = "https://psvpromo.psvgamestudio.com/cas-settings.php?apply=config&platform=" +
				(Target.Platform == UnrealTargetPlatform.Android ? "0" : "1") +
				"&bundle=" + ManagerID;

			var Response = RunProcessForResult("curl",
				"-w", WrapInQuotes("%{http_code}"),
				"-o", WrapInQuotes(CacheConfigFilePath),
				WrapInQuotes(RequestUrl)
			);
			Response = Response.Trim(' ', '"');

			if (Response.StartsWith("200") || Response.EndsWith("200"))
			{
				LogDebug("Configuration loaded for " + ManagerID);
				if (File.Exists(TempConfigFilePath))
					File.Delete(TempConfigFilePath);
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
					LogWarning(InvalidIdMessage);
					return;
				}
				CancelBuild(InvalidIdMessage);
			}

			LogWarning("Failed to load configuration: " + Response);
			if (File.Exists(TempConfigFilePath))
			{
				File.Move(TempConfigFilePath, CacheConfigFilePath);
				LogDebug("Configuration restored from cache for " + ManagerID);
				return;
			}

			if (ShippingMode)
			{
				CancelBuild(
					"Configuration file not found, please contact support for additional information. " +
					"If you have a configuration file, move it along the path: " + CacheConfigFilePath);
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
			string PluginLibrary = Path.Combine(NativePath, "repository", "com", "cleveradssolutions", "cas-unreal-plugin", "release", "cas-unreal-plugin-release.aar");
			if (!File.Exists(PluginLibrary))
				LostRequiredFile(PluginLibrary);
		}
	}

	public class IOSHandler : BaseHandler
	{
		public string FrameworksPath;
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
				var ResourceFile = GetResourcesFilePath();
				if (File.Exists(ResourceFile))
				{
					LogWarning("The Modern XCode build does not copy additional resources correctly.");
					LogWarning("Please add a configuration file in XCode project `App Target > Build Phases > Copy Bundle Resources > +`: " + ResourceFile);
				}
			}
#endif

			// Work only for not Modern Xcode Build UE 5.3+
			// Same as <copy file> in UPL.xml
			//Module.AdditionalBundleResources.Add(new BundleResource(GetResourcesFilePath()));

			FrameworksPath = Path.Combine(NativePath, "Frameworks");

			BridgeFrameworkPath = Path.Combine(NativePath, "Plugin", IOS_BRIDGE_NAME + ".embeddedframework.zip");
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
				var ConfigCache = XCodeConfig.Read(NativePath);
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
			string FrameworkProj = Path.Combine(NativePath, IOS_FRAMEWORKS_PROJECT + ".xcodeproj");
			if (!Directory.Exists(FrameworkProj))
				LostRequiredFile(FrameworkProj);

			string BuildDir = Path.Combine(NativePath, IOS_FRAMEWORKS_BUILD_DIR);
			string PodfileLock = Path.Combine(NativePath, "Podfile.lock");
			FileUtils.ForceDeleteFile(PodfileLock);
			FileUtils.ForceDeleteDirectory(BuildDir);
			FileUtils.ForceDeleteDirectory(FrameworksPath);
			Directory.CreateDirectory(FrameworksPath);

			var Config = new XCodeConfig();
			Config.version = version;
			Config.adapters = IncludedAdapters.ToArray();

			CreatePodfile(Config);

			string PodTool = CocoaPods.FindPodTool();
			RunProcess(PodTool, "update");

			string XCWorkspacePath = Path.Combine(NativePath, XCWorkspace);
			if (!Directory.Exists(XCWorkspacePath))
				CancelBuild("Pod installation failed. Fix errors in console logs and try again.");

			BuildPods(BuildDir);

			Config.FindDependencies(this, BuildDir);
			Config.ApplyToModule(Module, this);
			Config.Save(NativePath);

			RunProcess(PodTool, "deintegrate");
			FileUtils.ForceDeleteDirectory(XCWorkspacePath);
			FileUtils.ForceDeleteFile(PodfileLock);
			FileUtils.ForceDeleteDirectory(BuildDir);
		}

		private void BuildPods(string BuildDir)
		{
			RunProcess("xcodebuild", "build",
				"-workspace", XCWorkspace,
				"-scheme", IOS_FRAMEWORKS_PROJECT,
				"-derivedDataPath", WrapInQuotes(BuildDir),
				"-sdk", "iphoneos",
				"-destination", WrapInQuotes("generic/platform=iOS"),
				"-configuration", "Release",
				"IPHONEOS_DEPLOYMENT_TARGET=" + MINIMUM_IOS_VERSION,
				"GCC_GENERATE_DEBUGGING_SYMBOLS=NO"
			);
		}

		private void CreatePodfile(XCodeConfig Config)
		{
			var Path = System.IO.Path.Combine(NativePath, "Podfile");
			using (StreamWriter f = new StreamWriter(Path))
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
			EngineConfig.TryGetValue(EngineConfigName, "Include" + name, out included);
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
				bundles = xml.Element("bundles").Elements("bundle").Select(b =>
					new XCodeBundle(b.Attribute("name").Value, b.Attribute("bundle").Value)
				).ToList(),
				adapters = xml.Element("adapters").Elements("adapter").Select(e => e.Value).ToArray(),
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
			xml.Save(ConfigPath);
#else
			File.WriteAllText(ConfigPath, fastJSON.JSON.Instance.ToJSON(this));
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
				if (!Directory.Exists(Path.Combine(Handler.FrameworksPath, bundle.name + ".framework")))
				{
					LogDebug(LogPrefix + bundle.name + ".framework missing");
					return false;
				}
				if (!string.IsNullOrEmpty(bundle.bundle)
					&& !Directory.Exists(Path.Combine(Handler.FrameworksPath, bundle.bundle)))
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

		public void ApplyToModule(CleverAdsSolutions Module, IOSHandler Handler)
		{
			Module.PublicAdditionalFrameworks.Add(
				new Framework(IOS_BRIDGE_NAME, Handler.BridgeFrameworkPath)
			);

			Module.PublicSystemLibraries.AddRange(sysLibs);

			HashSet<string> ExcludeFrameworks = null;
			List<string> ExcludeFrameworksList;
			if (Handler.EngineConfig.GetArray(BuildConfigName, BuildConfigExcludeFrameworks, out ExcludeFrameworksList))
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
					new Framework(Framework.name, Handler.FrameworksPath, Resources, CopyFramework)
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
			CancelBuild(BundleName + " is not associated with any framework. Please contact support.");
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
