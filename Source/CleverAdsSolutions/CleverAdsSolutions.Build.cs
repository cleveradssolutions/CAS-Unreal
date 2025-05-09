// Copyright CleverAdsSolutions LTD, CAS.AI. All Rights Reserved.

#define USE_ENGNIE_INTERMEDIATE

//#define CAS_VERBOSE_LOGS

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
#if UE_5_3_OR_LATER
using Microsoft.Extensions.Logging;
#endif
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
		PrivateIncludePaths.Add("CleverAdsSolutions/Private");
		PrivateIncludePathModuleNames.Add("Settings");
		PublicDependencyModuleNames.Add("Core");
		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
				"CoreUObject",
				"Engine",
				"Projects"
			}
		);

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
#if UE_5_3_OR_LATER
		Log.Logger.LogInformation(LogTag + message);
#else
		Log.TraceInformation(LogTag + message);
#endif
	}

	private static void LogWarning(string message)
	{
		// Logger implementation not support collored warnings
		//Log.Logger.LogWarning(LogTag + message);
		Log.WriteLineOnce(LogEventType.Warning, LogTag + message);
	}

	private static void CancelBuild(string Error)
	{
		throw new BuildException(Error);
	}

	private static void LostRequiredFile(string File)
	{
		CancelBuild("Lost required plugin component. Try reinstal CAS plugin and check file exists: " + File);
	}

	private static string WrapInQuotes(string Line)
	{
		return "\"" + Line + "\"";
	}

	public abstract class BaseHandler
	{
		public string Version;
		public CASNetwork[] Solutions;
		public CASNetwork[] Adapters;

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

		public int RunProcess(string Name, bool Verbose, params string[] Args)
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
#if !CAS_VERBOSE_LOGS
			if (Verbose)
#endif
			{
				LocalProcess.OutputDataReceived += LocalProcessOutput;
			}
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
			string ModuleDirectory = Module.ModuleDirectory;
			PlatformName = Target.Platform.ToString();
			LogDebug(PlatformName + " Plugin build configuration");
			NativeDir = new DirectoryReference(Path.Combine(ModuleDirectory, "..", "ThirdParty", PlatformName));
			ShippingMode = Target.Configuration == UnrealTargetConfiguration.Shipping
				&& Target.ProjectFile.GetFileName() != "HostProject.uproject";

			string UPLFileName = "CAS_UPL_" + PlatformName + ".xml";
			string UPLFilePath = Path.Combine(ModuleDirectory, UPLFileName);
			if (!File.Exists(UPLFilePath))
				LostRequiredFile(UPLFilePath);

			var MediationListFile = FileReference.Combine(NativeDir, "CASMediation.list");
			if (!FileReference.Exists(MediationListFile))
				LostRequiredFile(MediationListFile.FullName);

			Module.PublicDefinitions.Add("WITH_CAS=1");

			string ModuleDirectoryRelative = Utils.MakePathRelativeTo(ModuleDirectory, Target.RelativeEnginePath);
			string ModuleDirectoryRelativeUPLFilePath = Path.Combine(ModuleDirectoryRelative, UPLFileName);
			LogDebug("Apply UPL: " + ModuleDirectoryRelativeUPLFilePath);
			Module.AdditionalPropertiesForReceipt.Add(PlatformName + "Plugin", ModuleDirectoryRelativeUPLFilePath);

			// When building an application on a Windows machine, 
			// it is crucial to ensure that your project folder is located on the same drive 
			// as the Unreal Engine installation directory. (e.g., both on C: or both on D:)
			if (ModuleDirectory[1] == ':' && ModuleDirectory[0] != Module.EngineDirectory[0])
			{
				string ErrorMessage = "The project folder msut be located on the same drive (" +
					Module.EngineDirectory[0] + ":) as the Unreal Engine installation directory: " +
					Module.EngineDirectory;
				if (ShippingMode)
				{
					CancelBuild(ErrorMessage);
				}
				else
				{
					LogWarning(ErrorMessage);
				}
			}

			JsonObject ConfigJson = JsonObject.Read(MediationListFile);
			Version = ConfigJson.GetStringField("version");
			Solutions = ParseNetworksList(ConfigJson, "simple");
			Adapters = ParseNetworksList(ConfigJson, "adapters");

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
			for (int i = 0; i < Solutions.Length; i++)
			{
				Solutions[i].Configure(EngineConfig);
			}
			for (int i = 0; i < Adapters.Length; i++)
			{
				Adapters[i].Configure(EngineConfig);
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

		public CASNetwork FindAdapter(string Name)
		{
			for (int i = 0; i < Solutions.Length; i++)
			{
				if (Solutions[i].Name == Name)
					return Solutions[i];
			}
			for (int i = 0; i < Adapters.Length; i++)
			{
				if (Adapters[i].Name == Name)
					return Adapters[i];
			}
			return null;
		}

		private void UpdateUPLFile(string UPLFilePath)
		{
			var UPLFileFullPath = Path.GetFullPath(UPLFilePath);
			LogDebug("Update UPL: " + UPLFileFullPath);
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
				var Config = FileReference.ReadAllText(CacheConfigFile);
				var AppId = FindPropertyString(Config, "admob_app_id");
				if (AppId != null && AppId.Contains('~'))
				{
					LogDebug("Apply Google App ID: " + AppId);
					string CustomGoogleAppId;
					if (EngineConfig.GetString(BuildConfigSection, BuildConfigGoogleAppId + PlatformName, out CustomGoogleAppId))
					{
						if (CustomGoogleAppId != AppId)
							CancelBuild("You have specified the incorrect Google App ID (" + CustomGoogleAppId +
								") in the Engine.ini file. Please ensure you have inserted the correct App ID: " + AppId);
					}
					return AppId;
				}
				if (FindAdapter("GoogleAds").Included)
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
				var lastModifiedTime = FileReference.GetLastWriteTime(CacheConfigFile);
				if (lastModifiedTime.AddHours(12) > DateTime.Now)
				{
					LogDebug("Configuration used from cache for " + ManagerID);
					return;
				}
				LogDebug("Configuration cache found but out of date. Modified " +
					lastModifiedTime.ToString() + ", Current " + DateTime.Now.ToString());

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
				FileReference.SetLastWriteTime(CacheConfigFile, DateTime.Now);
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
		public override void ApplyToModule(CleverAdsSolutions Module, ReadOnlyTargetRules Target)
		{
			Module.PublicDependencyModuleNames.Add("Launch"); // Includes Android JNI support
			base.ApplyToModule(Module, Target);
			string PluginLibrary = Path.Combine(NativeDir.FullName, "repository", "com", "cleveradssolutions", "cas-unreal-plugin", "release", "cas-unreal-plugin-release.aar");
			if (!File.Exists(PluginLibrary))
				LostRequiredFile(PluginLibrary);
		}
	}

	public class IOSHandler : BaseHandler
	{
		public DirectoryReference FrameworksDir;
		public DirectoryReference ResourcesDir;
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

#if USE_ENGNIE_INTERMEDIATE
			FrameworksDir = new DirectoryReference(Path.Combine(Module.EngineDirectory, "Intermediate", "UnzippedFrameworks"));
#else
			FrameworksDir = DirectoryReference.Combine(NativeDir, "Frameworks");
#endif
			ResourcesDir = DirectoryReference.Combine(NativeDir, "Resources");

			BridgeFrameworkPath = Path.Combine(NativeDir.FullName, "Plugin", IOSBridgeName + ".embeddedframework.zip");
			if (!File.Exists(BridgeFrameworkPath))
				LostRequiredFile(BridgeFrameworkPath);

			// AdditionalBundleResources not support json resources, but support files withut extension
			if (FileReference.Exists(CacheConfigFile))
			{
				var ConfigFilePathForBuild = CacheConfigFile.ChangeExtension(null);
				FileReference.Copy(CacheConfigFile, ConfigFilePathForBuild, true);
				Module.AdditionalBundleResources.Add(new BundleResource(ConfigFilePathForBuild.FullName));
			}

			for (int i = 0; i < Solutions.Length; i++)
			{
				if (Solutions[i].Included)
					IncludedAdapters.Add(Solutions[i].Name);
			}
			for (int i = 0; i < Adapters.Length; i++)
			{
				if (Adapters[i].Included)
				{
					IncludedAdapters.Add(Adapters[i].Name);
					if (Adapters[i].EmbedPath != null)
					{
						foreach (var embedLib in Adapters[i].EmbedPath)
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
			catch (NullReferenceException)
			{
				LogDebug("XCode config cache is deprecated.");
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
			FileUtils.ForceDeleteDirectory(ResourcesDir);
			FileUtils.CreateDirectoryTree(ResourcesDir);
#if !USE_ENGNIE_INTERMEDIATE
			FileUtils.ForceDeleteDirectory(FrameworksDir);
			FileUtils.CreateDirectoryTree(FrameworksDir);
#endif

			var Config = new XCodeConfig();
			Config.Version = Version;
			Config.Adapters = IncludedAdapters.ToArray();

			CreatePodfile(Config);

			string PodTool = CocoaPods.FindPodTool();
			RunProcess(PodTool, true, "update");

			var XCWorkspacePath = Path.Combine(NativeDir.FullName, IOSFameworksWorkspace);
			if (!Directory.Exists(XCWorkspacePath))
				CancelBuild("Pod installation failed. Fix errors in console logs and try again.");

			BuildPods(BuildDir);

			Config.FindDependencies(this, BuildDir);
			Config.ApplyToModule(Module, this);
			try
			{
				Config.Save(NativeDir);
			}
			catch (Exception e)
			{
				LogWarning(e.ToString());
			}

			RunProcess(PodTool, false, "deintegrate");
			FileUtils.ForceDeleteDirectory(XCWorkspacePath);
			FileUtils.ForceDeleteFile(PodfileLock);
			FileUtils.ForceDeleteDirectory(BuildDir);
		}

		private void BuildPods(string BuildDir)
		{
			RunProcess("xcodebuild", false, "build",
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
				//f.WriteLine("warn_for_unused_master_specs_repo => false");
				f.WriteLine();

				f.WriteLine("target '" + IOSFrameworksName + "' do");
				f.WriteLine("  pod 'CleverAdsSolutions-Base', '" + Version + "'");
				for (int adapterIndex = 0; adapterIndex < Config.Adapters.Length; adapterIndex++)
				{
					var adapter = FindAdapter(Config.Adapters[adapterIndex]);
					for (int libIndex = 0; libIndex < adapter.Libs.Length; libIndex++)
					{
						f.WriteLine("  pod '" + adapter.Libs[libIndex].Name + "', '" + adapter.Libs[libIndex].Version + "'");
					}
				}
				f.WriteLine("end");
			}
			LogDebug("Created " + Path);
		}
	}

	public class CASLib
	{
		public string Name;
		public string Version;

		public CASLib(JsonObject Json)
		{
			Name = Json.GetStringField("name");
			Version = Json.GetStringField("version");
		}
	}

	public class CASNetwork
	{
		public string Name;
		public string Version;
		public CASLib[] Libs;
		public string[] EmbedPath;
		public bool Included;

		public CASNetwork(JsonObject Json)
		{
			Name = Json.GetStringField("name");
			Json.TryGetStringField("version", out Version);
			Json.TryGetStringArrayField("embedPath", out EmbedPath);

			JsonObject[] RawLibs;
			if (Json.TryGetObjectArrayField("libs", out RawLibs))
			{
				Libs = new CASLib[RawLibs.Length];
				for (int i = 0; i < RawLibs.Length; i++)
					Libs[i] = new CASLib(RawLibs[i]);
			}
			else
			{
				Libs = new CASLib[0];
			}

			Included = false;
		}

		public void Configure(ConfigHierarchy EngineConfig)
		{
			EngineConfig.TryGetValue(EngineConfigSection, "Include" + Name, out Included);
		}
	}

	public class XCodeConfig
	{
		private const string FILE_NAME = "CASXCConfig.temp";

		public string Version;
		public List<XCodeBundle> Frameworks = new List<XCodeBundle>();
		public string[] Adapters;
		public string[] SysLibs;
		public string[] SysFrameworks;
		public string[] SysWeakFrameworks;
		public string[] AdditionalResources;

		public static XCodeConfig Read(DirectoryReference NativePath)
		{
			var ConfigPath = FileReference.Combine(NativePath, FILE_NAME);
			if (!FileReference.Exists(ConfigPath))
				return null;
#if UE_5_0_OR_LATER
			XElement Xml = XElement.Load(ConfigPath.FullName);
			return new XCodeConfig
			{
				Version = Xml.Attribute("version").Value,
				Frameworks = Xml.Element("bundles").Elements("bundle").Select(b =>
					new XCodeBundle(b.Attribute("name").Value)
					{
						bundle = b.Attribute("bundle").Value
					}
				).ToList(),
				Adapters = Xml.Element("adapters").Elements("adapter").Select(e => e.Value).ToArray(),
				SysLibs = Xml.Element("sysLibs").Elements("lib").Select(e => e.Value).ToArray(),
				SysFrameworks = Xml.Element("sysFrameworks").Elements("lib").Select(e => e.Value).ToArray(),
				SysWeakFrameworks = Xml.Element("sysWeakFrameworks").Elements("lib").Select(e => e.Value).ToArray(),
				AdditionalResources = Xml.Element("additRes").Elements("res").Select(e => e.Value).ToArray()
			};
#else
			return Json.Deserialize<XCodeConfig>(FileReference.ReadAllText(ConfigPath));
#endif
		}

		public void Save(DirectoryReference NativePath)
		{
			var ConfigPath = FileReference.Combine(NativePath, FILE_NAME);
#if UE_5_0_OR_LATER
			var Xml = new XElement("XCodeConfig",
				new XAttribute("version", Version),
				new XElement("bundles", Frameworks.Select(b =>
					new XElement("bundle",
						new XAttribute("name", b.name),
						new XAttribute("bundle", b.bundle)
					)
				)),
				new XElement("adapters", Adapters.Select(a => new XElement("adapter", a))),
				new XElement("sysLibs", SysLibs.Select(l => new XElement("lib", l))),
				new XElement("sysFrameworks", SysFrameworks.Select(f => new XElement("lib", f))),
				new XElement("sysWeakFrameworks", SysWeakFrameworks.Select(w => new XElement("lib", w))),
				new XElement("additRes", AdditionalResources.Select(w => new XElement("res", w)))
			);
			Xml.Save(ConfigPath.FullName);
#else
			FileReference.WriteAllText(ConfigPath, Json.Serialize(this, JsonSerializeOptions.None));
#endif
		}

		public bool IsSetupReady(IOSHandler Handler)
		{
			const string LogPrefix = "XCode Config ";
			if (Version != Handler.Version)
			{
				LogDebug(LogPrefix + "version not mach");
				return false;
			}
			if (Adapters.Length != Handler.IncludedAdapters.Count)
			{
				LogDebug(LogPrefix + "adapters not mach");
				return false;
			}
			var ReadyAdapters = new HashSet<string>(Adapters);
			for (int i = 0; i < Handler.IncludedAdapters.Count; i++)
			{
				if (!ReadyAdapters.Contains(Handler.IncludedAdapters[i]))
				{
					LogDebug(LogPrefix + Handler.IncludedAdapters[i] + " adapter missing");
					return false;
				}
			}
			foreach (var Bundle in Frameworks)
			{
				if (!Directory.Exists(Bundle.GetFrameworkPath(Handler, false)))
				{
					LogDebug(LogPrefix + Bundle.name + ".framework missing");
					return false;
				}
				if (!string.IsNullOrEmpty(Bundle.bundle)
					&& !Directory.Exists(Bundle.GetResourcesPath(Handler)))
				{
					LogDebug(LogPrefix + Bundle.bundle + " missing");
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

			foreach (var Item in FindBundles(".framework", DataPath))
			{
				if (Item.EndsWith(IOSFrameworksName + ".framework")) continue;
				var Bundle = new XCodeBundle(Path.GetFileNameWithoutExtension(Item));
				LogDebug("Framework found: " + Bundle.name);
				Directory.Move(Item, Bundle.GetFrameworkPath(Handler, true));
				Frameworks.Add(Bundle);
			}

			foreach (var Item in FindBundles(".a", DataPath, SkipExt: ".framework"))
			{
				var Bundle = new XCodeBundle(Path.GetFileNameWithoutExtension(Item).Substring(3));
				LogDebug("Library found: " + Bundle.name);
				string Destination = Bundle.GetFrameworkPath(Handler, true);
				Directory.CreateDirectory(Destination);
				File.Move(Item, Path.Combine(Destination, Bundle.name));
				Frameworks.Add(Bundle);
			}

			var PodsXCConfig = FileReference.Combine(Handler.NativeDir, "Pods", "Target Support Files", "Pods-" + IOSFrameworksName, "Pods-" + IOSFrameworksName + ".release.xcconfig");
			if (!FileReference.Exists(PodsXCConfig))
				CancelBuild("Build XCProject corrupted. Not found required build file: " + PodsXCConfig);
			FindSystemDependencies(PodsXCConfig.FullName);

			var AdditResSet = new HashSet<string>();
			foreach (var Item in FindBundles(".bundle", BuildMainDir))
			{
				var Resource = Path.GetFileName(Item);
				LogDebug("Resources found: " + Resource);
				XCodeBundle Bundle = null;
				foreach (var Framework in Frameworks)
				{
					if (Resource.StartsWith(Framework.name))
					{
						Bundle = Framework;
						break;
					}
				}
				if (Bundle == null || Bundle.bundle != null)
				{
					AdditResSet.Add(Resource);
					Directory.Move(Item, Path.Combine(Handler.ResourcesDir.FullName, Resource));
				}
				else
				{
					Bundle.bundle = Resource;
					Directory.Move(Item, Bundle.GetResourcesPath(Handler));
				}
			}
			AdditionalResources = AdditResSet.ToArray();
		}

		public void ApplyToModule(CleverAdsSolutions Module, IOSHandler Handler)
		{
			Module.PublicAdditionalFrameworks.Add(
				new Framework(IOSBridgeName, Handler.BridgeFrameworkPath)
			);

			Module.PublicSystemLibraries.AddRange(SysLibs);

			HashSet<string> ExcludeFrameworks = null;
			List<string> ExcludeFrameworksList;
			if (Handler.EngineConfig.GetArray(BuildConfigSection, BuildConfigExcludeFrameworks, out ExcludeFrameworksList))
			{
				ExcludeFrameworks = new HashSet<string>(ExcludeFrameworksList);
				foreach (var Framework in SysFrameworks)
				{
					if (ExcludeFrameworks.Contains(Framework))
						LogDebug("[Build Config] Excluded System Framework: " + Framework);
					else
						Module.PublicFrameworks.Add(Framework);
				}
				foreach (var Framework in SysWeakFrameworks)
				{
					if (ExcludeFrameworks.Contains(Framework))
						LogDebug("[Build Config] Excluded Weak Framework: " + Framework);
					else
						Module.PublicWeakFrameworks.Add(Framework);
				}
			}
			else
			{
				Module.PublicFrameworks.AddRange(SysFrameworks);
				Module.PublicWeakFrameworks.AddRange(SysWeakFrameworks);
			}

			bool UseAdvertisingId = true;
			if (!Handler.EngineConfig.TryGetValue(EngineConfigSection, "UseAdvertisingId", out UseAdvertisingId) || UseAdvertisingId)
			{
				if (Array.IndexOf(SysFrameworks, "AdSupport") > -1)
					Module.PublicFrameworks.Add("AdSupport");
				if (Array.IndexOf(SysFrameworks, "AppTrackingTransparency") > -1)
					Module.PublicFrameworks.Add("AppTrackingTransparency");
			}

			foreach (var Framework in Frameworks)
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

			foreach (var Resource in AdditionalResources)
			{
				Module.AdditionalBundleResources.Add(
					new BundleResource(Path.Combine(Handler.ResourcesDir.FullName, Resource))
				);
			}

#if UE_5_2_OR_LATER
			Module.bEnableObjCAutomaticReferenceCounting = true;
#endif

			AddSwiftSupport(Module);
		}

		private void AddSwiftSupport(CleverAdsSolutions Module)
		{
			// Source: /Engine/Source/ThirdParty/Apple/Swift/Swift.Build.cs
#if UE_5_2_OR_LATER
			// Add support for linking with Swift frameworks
			Module.PrivateDependencyModuleNames.Add("Swift");
#else
			// XcodeRoot = /Applications/Xcode.app/Contents/Developer
			string XcodeRoot = Utils.RunLocalProcessAndReturnStdOut("/usr/bin/xcode-select", "--print-path");
			Module.PublicSystemLibraryPaths.AddRange(
				new string[] {
					XcodeRoot + "/Toolchains/XcodeDefault.xctoolchain/usr/lib/swift/iphoneos",
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

		private void FindSystemDependencies(string PodsXCConfig)
		{
			string[] OtherLDFlags = null;
			using (var Reader = File.OpenText(PodsXCConfig))
			{
				string Line;
				while ((Line = Reader.ReadLine()) != null)
				{
					if (Line.StartsWith("OTHER_LDFLAGS"))
					{
						OtherLDFlags = Line.Split('-');
						break;
					}
				}
			}
			if (OtherLDFlags == null)
				CancelBuild("Build XCProject corrupted. Not found OTHER_LDFLAGS field in build file: " + PodsXCConfig);

			var SysLibs = new List<string>();
			var SysFrameworks = new HashSet<string>();
			var SysWeakFrameworks = new List<string>();
			var FrameworkSet = new HashSet<string>();
			for (int i = 0; i < Frameworks.Count; i++)
				FrameworkSet.Add(Frameworks[i].name);

			for (int i = 0; i < OtherLDFlags.Length; i++)
			{
				var Lib = OtherLDFlags[i].Trim(' ');
				if (Lib.StartsWith("l"))
				{
					Lib = Lib.Substring(2, Lib.Length - 1 - 2);
					if (!FrameworkSet.Contains(Lib))
					{
						SysLibs.Add(Lib);
					}
				}
				else if (Lib.StartsWith("framework"))
				{
					// Substring name from: '-framework "CleverAdsSolutions"'
					// 11 = "framework \"".Length
					Lib = Lib.Substring(11, Lib.Length - 1 - 11);
					if (!FrameworkSet.Contains(Lib))
					{
						SysFrameworks.Add(Lib);
					}
				}
			}
			for (int i = 0; i < OtherLDFlags.Length; i++)
			{
				var Lib = OtherLDFlags[i].Trim(' ');
				if (Lib.StartsWith("weak_framework"))
				{
					// Substring name from: '-weak_framework "AdSupport"'
					// 16 = "weak_framework \"".Length
					Lib = Lib.Substring(16, Lib.Length - 1 - 16);
					if (!FrameworkSet.Contains(Lib) && !SysFrameworks.Contains(Lib))
						SysWeakFrameworks.Add(Lib);
				}
			}
			this.SysLibs = SysLibs.ToArray();
			this.SysFrameworks = SysFrameworks.ToArray();
			this.SysWeakFrameworks = SysWeakFrameworks.ToArray();
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
			// FrameworkRules.Path = "FrameworkName.embeddedframework.zip"
			// Path.Combine(Module.EngineDirectory, "Intermediate", "UnzippedFrameworks", FrameworkRules.Name, Path.GetFileNameWithoutExtension(FrameworkRules.Path))
#if USE_ENGNIE_INTERMEDIATE
			return Path.Combine(Handler.FrameworksDir.FullName, name, name + ".embeddedframework");
#else
			return Path.Combine(Handler.FrameworksDir.FullName, name);
#endif
		}

		public string GetFrameworkPath(IOSHandler Handler, bool Create)
		{
			string Dir = GetPath(Handler);
			if (Create)
			{
#if USE_ENGNIE_INTERMEDIATE
				FileUtils.ForceDeleteDirectory(Path.GetDirectoryName(Dir));
#endif
				if (!Directory.Exists(Dir))
					Directory.CreateDirectory(Dir);
			}
			return Path.Combine(Dir, name + ".framework");
		}

		public string GetResourcesPath(IOSHandler Handler)
		{
			return Path.Combine(GetPath(Handler), bundle);
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
			foreach (string Root in SearchPaths)
			{
				string PodPath = Path.Combine(Root, POD_EXECUTABLE);
				if (File.Exists(PodPath))
					return PodPath;
			}
			var Environment = ReadGemsEnvironment();
			if (Environment != null)
			{
				const string ExecutableDir = "EXECUTABLE DIRECTORY";
				foreach (string environmentVariable in new[] { ExecutableDir, "GEM PATHS" })
				{
					List<string> Paths;
					if (Environment.TryGetValue(environmentVariable, out Paths))
					{
						foreach (var Root in Paths)
						{
							var BinPath = Root.EndsWith("bin") ? Root : Path.Combine(Root, "bin");
							var PodPath = Path.Combine(BinPath, POD_EXECUTABLE);
							if (File.Exists(PodPath))
								return PodPath;
						}
					}
				}
			}
			LogWarning("Pod command not found. You can install CocoaPods with the Ruby gem package manager: " +
					" > sudo gem install -n /usr/local/bin cocoapods");
			return POD_EXECUTABLE;
		}

		private static Dictionary<string, List<string>> ReadGemsEnvironment()
		{
			int ExitCode;
			var Result = Utils.RunLocalProcessAndReturnStdOut("gem", "environment", out ExitCode);
			if (ExitCode != 0) return null;
			// gem environment outputs YAML for all config variables.  Perform some very rough YAML
			// parsing to get the environment into a usable form.
			var GEMEnvironment = new Dictionary<string, List<string>>();
			const string ListItemPrefix = "- ";
			int PreviousIndentSize = 0;
			List<string> CurrentList = null;
			char[] ListToken = new char[] { ':' };
			foreach (var Line in Result.Split(new char[] { '\r', '\n' }))
			{
				var TrimmedLine = Line.Trim();
				var IndentSize = Line.Length - TrimmedLine.Length;
				if (IndentSize < PreviousIndentSize) CurrentList = null;

				if (TrimmedLine.StartsWith(ListItemPrefix))
				{
					TrimmedLine = TrimmedLine.Substring(ListItemPrefix.Length).Trim();
					if (CurrentList == null)
					{
						var Vokens = TrimmedLine.Split(ListToken);
						CurrentList = new List<string>();
						GEMEnvironment[Vokens[0].Trim()] = CurrentList;
						var Value = Vokens.Length == 2 ? Vokens[1].Trim() : null;
						if (!string.IsNullOrEmpty(Value))
						{
							CurrentList.Add(Value);
							CurrentList = null;
						}
					}
					else if (IndentSize >= PreviousIndentSize)
					{
						CurrentList.Add(TrimmedLine);
					}
				}
				else
				{
					CurrentList = null;
				}
				PreviousIndentSize = IndentSize;
			}
			return GEMEnvironment;
		}
	}
}
