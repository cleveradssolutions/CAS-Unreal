// Copyright CleverAdsSolutions LTD, CAS.AI. All Rights Reserved.

// The frameworks are prepared by a script that can be run without the engine, by the
// publisher, so they live in the plugin directory rather than in the engine Intermediate.
//#define USE_ENGNIE_INTERMEDIATE

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

	private const string IOSMinimumVersion = "15.0";
	private const string IOSBridgeName = "CASUnrealBridge";
	private const string IOSPrepareScript = "cas_unreal_prepare.py";
	private const string EngineConfigSectionFormat = "CASPluginBuildConfig_{0}";

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

	private string GetPathToEngine()
	{
		// UE 5.8 make this property static.
		return EngineDirectory;
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
		public string EngineConfigSection;
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

			EngineConfigSection = string.Format(EngineConfigSectionFormat, PlatformName);
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
			
			// When building an application on a Windows machine, 
			// it is crucial to ensure that your project folder is located on the same drive 
			// as the Unreal Engine installation directory. (e.g., both on C: or both on D:)
			var EngineDirectory = Module.GetPathToEngine();
			if (ModuleDirectory[1] == ':' && ModuleDirectory[0] != EngineDirectory[0])
			{
				string ErrorMessage = "The project folder msut be located on the same drive (" +
					ModuleDirectory[0] + ":) as the Unreal Engine installation directory: " +
					EngineDirectory;
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
				Solutions[i].Configure(EngineConfig, EngineConfigSection);
			}
			for (int i = 0; i < Adapters.Length; i++)
			{
				Adapters[i].Configure(EngineConfig, EngineConfigSection);
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

			// Apply UPL after update
			LogDebug("Apply UPL: " + ModuleDirectoryRelativeUPLFilePath);
			Module.AdditionalPropertiesForReceipt.Add(PlatformName + "Plugin", ModuleDirectoryRelativeUPLFilePath);
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
			FrameworksDir = new DirectoryReference(Path.Combine(Module.GetPathToEngine(), "Intermediate", "UnzippedFrameworks"));
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
					IncludedAdapters.Add(Adapters[i].Name);
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

			PrepareFrameworks(Module);
		}

		/// <summary>
		/// Runs the preparation script, which downloads the frameworks of the included
		/// adapters from the Swift Package Manager dependencies and describes the result
		/// in <see cref="XCodeConfig"/>.
		///
		/// The same script can be run by the publisher before the build, in which case
		/// the build finds the result ready and does not run it again. Unreal calls this
		/// module several times in a row, and the written config is what keeps the
		/// preparation to once.
		/// </summary>
		private void PrepareFrameworks(CleverAdsSolutions Module)
		{
			var Script = Path.Combine(NativeDir.FullName, IOSPrepareScript);
			if (!File.Exists(Script))
				LostRequiredFile(Script);

			var Arguments = new List<string>
			{
				WrapInQuotes(Script),
				"--version", Version,
#if UE_5_0_OR_LATER
				"--format", "xml",
#else
				"--format", "json",
#endif
				"--platform", IOSMinimumVersion,
				"--adapters"
			};
			Arguments.AddRange(IncludedAdapters);

			LogDebug("Prepare the iOS frameworks of: " + string.Join(", ", IncludedAdapters));
			if (RunProcess(FindPython(Module), true, Arguments.ToArray()) != 0)
			{
				CancelBuild(
					"Preparation of the iOS frameworks failed. Fix the errors in the console logs and try again."
				);
			}

			XCodeConfig Config = null;
			try
			{
				Config = XCodeConfig.Read(NativeDir);
			}
			catch (Exception e)
			{
				// The file is written by the script that has just run, so an unreadable
				// one is a defect rather than something to recover from by running again.
				CancelBuild(
					"Preparation of the iOS frameworks wrote a " + XCodeConfig.FILE_NAME +
					" that this engine cannot read: " + e.Message
				);
			}
			if (Config == null)
				CancelBuild("Preparation of the iOS frameworks left no " + XCodeConfig.FILE_NAME);
			if (!Config.IsSetupReady(this))
			{
				CancelBuild(
					"Preparation of the iOS frameworks did not cover the selected adapters. " +
					"See the console logs above."
				);
			}

			Config.ApplyToModule(Module, this);
		}

		/// <summary>
		/// The engine ships a python of its own, which is there whether or not the
		/// machine has one on the PATH.
		/// </summary>
		private string FindPython(CleverAdsSolutions Module)
		{
			var EnginePython = Path.Combine(
				Module.GetPathToEngine(), "Binaries", "ThirdParty", "Python3", "Mac", "bin", "python3"
			);
			if (File.Exists(EnginePython))
				return EnginePython;
			return "python3";
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

		public void Configure(ConfigHierarchy EngineConfig, string Section)
		{
			EngineConfig.TryGetValue(Section, "Include" + Name, out Included);
		}
	}

	public class XCodeConfig
	{
		public const string FILE_NAME = "CASXCConfig.temp";

		public string Version;
		public List<XCodeBundle> Frameworks = new List<XCodeBundle>();
		public string[] Adapters;
		/// <summary>
		/// What the selection resolved to in the Swift package, for the logs: a solution
		/// is a set of adapters and the package names them its own way.
		/// </summary>
		public string[] Resolved;
		public string Script;
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
						bundle = b.Attribute("bundle").Value,
						dynamic = b.Attribute("dynamic").Value == "true"
					}
				).ToList(),
				Adapters = Xml.Element("adapters").Elements("adapter").Select(e => e.Value).ToArray(),
				Resolved = Xml.Element("resolved").Elements("adapter").Select(e => e.Value).ToArray(),
				SysLibs = Xml.Element("sysLibs").Elements("lib").Select(e => e.Value).ToArray(),
				SysFrameworks = Xml.Element("sysFrameworks").Elements("lib").Select(e => e.Value).ToArray(),
				SysWeakFrameworks = Xml.Element("sysWeakFrameworks").Elements("lib").Select(e => e.Value).ToArray(),
				AdditionalResources = Xml.Element("additRes").Elements("res").Select(e => e.Value).ToArray()
			};
#else
			return Json.Deserialize<XCodeConfig>(FileReference.ReadAllText(ConfigPath));
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

			// The SDK asks for the tracking authorization by class name, so the framework
			// has to be linked for that lookup to find anything, and the ad sources do not
			// all ask for it. Added here when nothing else did, and left alone when the
			// publisher excluded it on purpose.
			//
			// AdSupport is not added: the advertising id is the ad sources' own business
			// now, and whoever needs it declares it.
			bool UseAdvertisingId = true;
			if (!Handler.EngineConfig.TryGetValue(Handler.EngineConfigSection, "UseAdvertisingId", out UseAdvertisingId) || UseAdvertisingId)
			{
				const string TrackingFramework = "AppTrackingTransparency";
				bool IsExcluded = ExcludeFrameworks != null && ExcludeFrameworks.Contains(TrackingFramework);
				if (!IsExcluded
					&& Array.IndexOf(SysFrameworks, TrackingFramework) < 0
					&& Array.IndexOf(SysWeakFrameworks, TrackingFramework) < 0)
				{
					LogDebug("Add " + TrackingFramework + " for the tracking authorization request");
					Module.PublicFrameworks.Add(TrackingFramework);
				}
			}

			foreach (var Framework in Frameworks)
			{
				if (ExcludeFrameworks != null && ExcludeFrameworks.Contains(Framework.name))
				{
					LogDebug("[Build Config] Excluded Additional Framework: " + Framework.name);
					continue;
				}

				var Resources = string.IsNullOrEmpty(Framework.bundle) ? null : Framework.bundle;
				// A dynamic framework is embedded in the application, a static one is linked only
				bool CopyFramework = Framework.dynamic;
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
#else
			try
			{
				var arcProperty = Module.GetType().GetProperty("bEnableObjCAutomaticReferenceCounting");
				if (arcProperty != null)
				{
					arcProperty.SetValue(Module, true);
				}
			}
			catch (Exception e)
			{
				// ignore
			}
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

	}

	public class XCodeBundle
	{
		public string name;
		public string bundle;
		public bool dynamic;

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
			// Path.Combine(Module.GetPathToEngine(), "Intermediate", "UnzippedFrameworks", FrameworkRules.Name, Path.GetFileNameWithoutExtension(FrameworkRules.Path))
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
}
