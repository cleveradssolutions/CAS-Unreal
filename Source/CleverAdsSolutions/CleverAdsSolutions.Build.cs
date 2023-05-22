// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;
using System.IO;
using Tools.DotNETCommon;
using System.Collections.Generic;

public class CleverAdsSolutions : ModuleRules
{
	public CleverAdsSolutions(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;
		string PluginPath = Utils.MakePathRelativeTo(ModuleDirectory, Target.RelativeEnginePath);
		
		PublicDependencyModuleNames.AddRange(
			new string[]
			{
				"Core",
				// ... add other public dependencies that you statically link with here ...
			}
		);
			
		
		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
				"CoreUObject",
				"Engine",
				"Slate",
				"SlateCore"
				// ... add private dependencies that you statically link with here ...	
			}
		);
		
		PrivateIncludePathModuleNames.Add("Settings");
		
//		PublicIncludePaths.Add("CleverAdsSolutions/Interfaces");
		PublicIncludePaths.Add(Path.Combine(ModuleDirectory, "Public"));
		PublicIncludePaths.Add(Path.Combine(ModuleDirectory, "Interfaces"));

		PublicDefinitions.Add("WITH_CAS=1");
		
		bool EnableCrossPromo = false;
		
		bool EnableAppLovin = false;
		bool EnableFacebook = false;
		bool EnableIronsource = false;
		bool EnableGoogleAdMob = false;
		bool EnableAdColony = false;
		bool EnableMintegral = false;
		bool EnableMyTarget = false;
		bool EnableInMobi = false;
		bool EnablePangle = false;
		bool EnableTapjoy = false;
		bool EnableUnityAds = false;
		bool EnableVungle = false;
		bool EnableYandex = false;

		ConfigHierarchy EngineConfig = ConfigCache.ReadHierarchy(ConfigHierarchyType.Engine, DirectoryReference.FromFile(Target.ProjectFile), Target.Platform);

		if (Target.Platform == UnrealTargetPlatform.Android)
		{
			AdditionalPropertiesForReceipt.Add("AndroidPlugin", Path.Combine(PluginPath, "CAS_UPL_Android.xml"));
			
			PublicDependencyModuleNames.AddRange(
			new string[]
			{
				"Launch"
			});

			EngineConfig.TryGetValue("/Script/CleverAdsSolutions.CASSettingsAndroid", "EnableCrossPromo", out EnableCrossPromo);
			
			EngineConfig.TryGetValue("/Script/CleverAdsSolutions.CASSettingsAndroid", "EnableAppLovin", out EnableAppLovin);
			EngineConfig.TryGetValue("/Script/CleverAdsSolutions.CASSettingsAndroid", "EnableFacebook", out EnableFacebook);
			EngineConfig.TryGetValue("/Script/CleverAdsSolutions.CASSettingsAndroid", "EnableIronsource", out EnableIronsource);
			EngineConfig.TryGetValue("/Script/CleverAdsSolutions.CASSettingsAndroid", "EnableGoogleAdMob", out EnableGoogleAdMob);
			EngineConfig.TryGetValue("/Script/CleverAdsSolutions.CASSettingsAndroid", "EnableAdColony", out EnableAdColony);
			EngineConfig.TryGetValue("/Script/CleverAdsSolutions.CASSettingsAndroid", "EnableMintegral", out EnableMintegral);
			EngineConfig.TryGetValue("/Script/CleverAdsSolutions.CASSettingsAndroid", "EnableMyTarget", out EnableMyTarget);
			EngineConfig.TryGetValue("/Script/CleverAdsSolutions.CASSettingsAndroid", "EnableInMobi", out EnableInMobi);
			EngineConfig.TryGetValue("/Script/CleverAdsSolutions.CASSettingsAndroid", "EnablePangle", out EnablePangle);
			EngineConfig.TryGetValue("/Script/CleverAdsSolutions.CASSettingsAndroid", "EnableTapjoy", out EnableTapjoy);
			EngineConfig.TryGetValue("/Script/CleverAdsSolutions.CASSettingsAndroid", "EnableUnityAds", out EnableUnityAds);
			EngineConfig.TryGetValue("/Script/CleverAdsSolutions.CASSettingsAndroid", "EnableVungle", out EnableVungle);
			EngineConfig.TryGetValue("/Script/CleverAdsSolutions.CASSettingsAndroid", "EnableYandex", out EnableYandex);

			PublicDefinitions.Add("WITH_CAS_CROSSPROMO="+(EnableCrossPromo?"1":"0"));
			
			PublicDefinitions.Add("WITH_CAS_APPLOVIN="+(EnableAppLovin?"1":"0"));
			PublicDefinitions.Add("WITH_CAS_FACEBOOK="+(EnableFacebook?"1":"0"));
			PublicDefinitions.Add("WITH_CAS_IRONSOURCE="+(EnableIronsource?"1":"0"));
			PublicDefinitions.Add("WITH_CAS_ADCOLONY="+(EnableAdColony?"1":"0"));
			PublicDefinitions.Add("WITH_CAS_MINTEGRAL="+(EnableMintegral?"1":"0"));
			PublicDefinitions.Add("WITH_CAS_MYTARGET="+(EnableMyTarget?"1":"0"));
			PublicDefinitions.Add("WITH_CAS_INMOBI="+(EnableInMobi?"1":"0"));
			PublicDefinitions.Add("WITH_CAS_PANGLE="+(EnablePangle?"1":"0"));
			PublicDefinitions.Add("WITH_CAS_TAPJOY="+(EnableTapjoy?"1":"0"));
			PublicDefinitions.Add("WITH_CAS_UNITYADS="+(EnableUnityAds?"1":"0"));
			PublicDefinitions.Add("WITH_CAS_GOOGLEADS="+(EnableGoogleAdMob?"1":"0"));
			PublicDefinitions.Add("WITH_CAS_VUNGLE="+(EnableVungle?"1":"0"));
			PublicDefinitions.Add("WITH_CAS_YANDEXADS="+(EnableYandex?"1":"0"));
		}

		if (Target.Platform == UnrealTargetPlatform.IOS)
		{
			EngineConfig.TryGetValue("/Script/CleverAdsSolutions.CASSettingsIOS", "EnableCrossPromo", out EnableCrossPromo);
			
			EngineConfig.TryGetValue("/Script/CleverAdsSolutions.CASSettingsIOS", "EnableAppLovin", out EnableAppLovin);
			EngineConfig.TryGetValue("/Script/CleverAdsSolutions.CASSettingsIOS", "EnableFacebook", out EnableFacebook);
			EngineConfig.TryGetValue("/Script/CleverAdsSolutions.CASSettingsIOS", "EnableIronsource", out EnableIronsource);
			EngineConfig.TryGetValue("/Script/CleverAdsSolutions.CASSettingsIOS", "EnableGoogleAdMob", out EnableGoogleAdMob);
			EngineConfig.TryGetValue("/Script/CleverAdsSolutions.CASSettingsIOS", "EnableAdColony", out EnableAdColony);
			EngineConfig.TryGetValue("/Script/CleverAdsSolutions.CASSettingsIOS", "EnableMintegral", out EnableMintegral);
			EngineConfig.TryGetValue("/Script/CleverAdsSolutions.CASSettingsIOS", "EnableMyTarget", out EnableMyTarget);
			EngineConfig.TryGetValue("/Script/CleverAdsSolutions.CASSettingsIOS", "EnableInMobi", out EnableInMobi);
			EngineConfig.TryGetValue("/Script/CleverAdsSolutions.CASSettingsIOS", "EnablePangle", out EnablePangle);
			EngineConfig.TryGetValue("/Script/CleverAdsSolutions.CASSettingsIOS", "EnableTapjoy", out EnableTapjoy);
			EngineConfig.TryGetValue("/Script/CleverAdsSolutions.CASSettingsIOS", "EnableUnityAds", out EnableUnityAds);
			EngineConfig.TryGetValue("/Script/CleverAdsSolutions.CASSettingsIOS", "EnableVungle", out EnableVungle);
			EngineConfig.TryGetValue("/Script/CleverAdsSolutions.CASSettingsIOS", "EnableYandex", out EnableYandex);

			PublicDefinitions.Add("WITH_CAS_CROSSPROMO="+(EnableCrossPromo?"1":"0"));
			
			PublicDefinitions.Add("WITH_CAS_APPLOVIN="+(EnableAppLovin?"1":"0"));
			PublicDefinitions.Add("WITH_CAS_FACEBOOK="+(EnableFacebook?"1":"0"));
			PublicDefinitions.Add("WITH_CAS_IRONSOURCE="+(EnableIronsource?"1":"0"));
			PublicDefinitions.Add("WITH_CAS_ADCOLONY="+(EnableAdColony?"1":"0"));
			PublicDefinitions.Add("WITH_CAS_MINTEGRAL="+(EnableMintegral?"1":"0"));
			PublicDefinitions.Add("WITH_CAS_MYTARGET="+(EnableMyTarget?"1":"0"));
			PublicDefinitions.Add("WITH_CAS_INMOBI="+(EnableInMobi?"1":"0"));
			PublicDefinitions.Add("WITH_CAS_PANGLE="+(EnablePangle?"1":"0"));
			PublicDefinitions.Add("WITH_CAS_TAPJOY="+(EnableTapjoy?"1":"0"));
			PublicDefinitions.Add("WITH_CAS_UNITYADS="+(EnableUnityAds?"1":"0"));
			PublicDefinitions.Add("WITH_CAS_GOOGLEADS="+(EnableGoogleAdMob?"1":"0"));
			PublicDefinitions.Add("WITH_CAS_VUNGLE="+(EnableVungle?"1":"0"));
			PublicDefinitions.Add("WITH_CAS_YANDEXADS="+(EnableYandex?"1":"0"));
			
			AdditionalPropertiesForReceipt.Add("IOSPlugin", Path.Combine(PluginPath, "CAS_UPL_IOS.xml"));

			PublicFrameworks.AddRange(
				new string[] {
					"AdSupport",
					"AdServices",
					"StoreKit",
					"SystemConfiguration",
					"CoreGraphics",
					"CoreTelephony",
					"AVFoundation",
					"CoreMedia",
					"CoreData",
					"AdServices",
					"AppTrackingTransparency",
					"UIKit",
					"iAd",
					"StoreKit",
					"CoreMotion",
					"SafariServices",
					"FileProvider",
					"JavaScriptCore"
				}
			);
			
			PublicSystemLibraries.Add("c++");
			PublicSystemLibraries.Add("z");
			PublicSystemLibraries.Add("bz2");
			PublicSystemLibraries.Add("c++abi");
			PublicSystemLibraries.Add("resolv");
			PublicSystemLibraries.Add("sqlite3");
			PublicSystemLibraries.Add("xml2");
			PublicSystemLibraries.Add("iconv");
			
			string SDKROOT = Utils.RunLocalProcessAndReturnStdOut("/usr/bin/xcrun", "--sdk iphoneos --show-sdk-path");
			PublicSystemLibraryPaths.Add(SDKROOT + "/usr/lib/swift");
			PublicSystemLibraryPaths.Add(SDKROOT + "../../../../../../Toolchains/XcodeDefault.xctoolchain/usr/lib/swift/iphoneos");
			PublicSystemLibraryPaths.Add(SDKROOT + "../../../../../../Toolchains/XcodeDefault.xctoolchain/usr/lib/swift-5.0/iphoneos");
			PublicSystemLibraryPaths.Add(SDKROOT + "../../../../../../Toolchains/XcodeDefault.xctoolchain/usr/lib/swift-5.5/iphoneos");

			Dictionary<string, string> Libraries = new Dictionary<string, string>{};
			
			Libraries.Add("CleverAdsSolutions", "CASBaseResources.bundle");
			
			if (EnableCrossPromo)
			{
				Libraries.Add("CASPromo", "");
			}

			// Adapters -------

			if (EnableAppLovin)
			{
				Libraries.Add("AppLovinSDK", "AppLovinSDKResources.bundle");
				Libraries.Add("CASAppLovin", "");
			}

			if (EnableFacebook)
			{
				Libraries.Add("FBAudienceNetwork", "");
				Libraries.Add("CASAudienceNetwork", "");
			}

			if (EnableIronsource)
			{
				Libraries.Add("IronSource", "");
				Libraries.Add("CASIronsource", "");
			}

			if (EnableGoogleAdMob)
			{
				Libraries.Add("GoogleMobileAds", "");
				Libraries.Add("CASGoogleAds", "");
				Libraries.Add("GoogleAppMeasurement", "");
				Libraries.Add("GoogleAppMeasurementIdentitySupport", "");
				Libraries.Add("GoogleUtilities", "");
				Libraries.Add("nanopb", "");
				Libraries.Add("FBLPromises", "");
			}

			if (EnableAdColony)
			{
				Libraries.Add("CASAdColony", "");
				Libraries.Add("AdColony", "");
			}

			if (EnableMintegral)
			{
				Libraries.Add("CASMintegral", "");
				Libraries.Add("MTGSDK", "");
				Libraries.Add("MTGSDKBanner", "");
				Libraries.Add("MTGSDKBidding", "");
				Libraries.Add("MTGSDKInterstitialVideo", "");
				Libraries.Add("MTGSDKNewInterstitial", "");
				Libraries.Add("MTGSDKReward", "");
				Libraries.Add("MTGSDKSplash", "");
			}

			if (EnableMyTarget)
			{
				Libraries.Add("CASMyTarget", "");
				Libraries.Add("MyTargetSDK", "");
				Libraries.Add("MyTrackerSDK", "");
			}

			if (EnableInMobi)
			{
				Libraries.Add("CASInMobi", "");
				Libraries.Add("InMobiSDK", "");
			}

			if (EnablePangle)
			{
				Libraries.Add("BURelyAdSDK", "");
				Libraries.Add("PAGAdSDK", "PAGAdSDK.bundle");
				Libraries.Add("BURelyFoundationGlobalAFN", "");
				Libraries.Add("BURelyFoundationGlobalFoundation", "");
				Libraries.Add("BURelyFoundationGlobalGecko", "");
				Libraries.Add("BURelyFoundationGlobalHeader", "");
				Libraries.Add("BURelyFoundationGlobalNETWork", "");
				Libraries.Add("BURelyFoundationGlobalSDWebImage", "");
				Libraries.Add("BURelyFoundationGlobalYYModel", "");
				Libraries.Add("BURelyFoundationGlobalZFPlayer", "");
				Libraries.Add("BURelyFoundationGlobalZip", "");
				Libraries.Add("CASPangle", "");
			}

			if (EnableTapjoy)
			{
				Libraries.Add("CASTapjoy", "");
				Libraries.Add("Tapjoy", "");
			}

			if (EnableUnityAds)
			{
				Libraries.Add("CASUnityAds", "");
				Libraries.Add("UnityAds", "");
			}

			if (EnableVungle)
			{
				Libraries.Add("CASVungle", "");
				Libraries.Add("VungleSDK", "");
			}			

			if (EnableYandex)
			{
				Libraries.Add("CASYandexAds", "");
				Libraries.Add("YandexMobileAds", "YandexMobileAdsBundle.bundle");
				Libraries.Add("YandexMobileMetrica", "");
				Libraries.Add("YandexMobileMetricaCrashes", "");
				
				Libraries.Add("Base", "");
				Libraries.Add("BaseTiny", "");
				Libraries.Add("BaseUI", "");
				Libraries.Add("CommonCore", "");
				Libraries.Add("DivKit", "");
				Libraries.Add("LayoutKit", "");
				Libraries.Add("LayoutKitInterface", "");
				Libraries.Add("Networking", "");
				Libraries.Add("Serialization", "");
				Libraries.Add("TemplatesSupport", "");
			}
			
			string LibsPath = "External/iOS/";

			foreach (KeyValuePair<string, string> Lib in Libraries)
			{
				if (Lib.Value != "")
				{
					PublicAdditionalFrameworks.Add(
						new Framework(
							Lib.Key,
							LibsPath+Lib.Key+".embeddedframework.zip",
							Lib.Value
						)
					);
				}
				else
				{
					PublicAdditionalFrameworks.Add(
						new Framework(
							Lib.Key,
							LibsPath+Lib.Key+".embeddedframework.zip"
						)
					);
				}
			}
		}
	}
}
