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
		
		PrivateIncludePaths.Add("CleverAdsSolutions/Interfaces");

		if (Target.Platform == UnrealTargetPlatform.Android)
		{
			AdditionalPropertiesForReceipt.Add("AndroidPlugin", Path.Combine(PluginPath, "CAS_UPL_Android.xml"));
			
			PublicDependencyModuleNames.AddRange(
			new string[]
			{
				"Launch"
			});
		}

		if (Target.Platform == UnrealTargetPlatform.IOS)
		{
			AdditionalPropertiesForReceipt.Add("IOSPlugin", Path.Combine(PluginPath, "CAS_UPL_IOS.xml"));
			
			ConfigHierarchy EngineConfig = ConfigCache.ReadHierarchy(ConfigHierarchyType.Engine, DirectoryReference.FromFile(Target.ProjectFile), Target.Platform);
			
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
            
			EngineConfig.TryGetValue("/Script/CleverAdsSolutions.CASSettings", "EnableAppLovinIOS", out EnableAppLovin);
			EngineConfig.TryGetValue("/Script/CleverAdsSolutions.CASSettings", "EnableFacebookIOS", out EnableFacebook);
			EngineConfig.TryGetValue("/Script/CleverAdsSolutions.CASSettings", "EnableIronsourceIOS", out EnableIronsource);
			EngineConfig.TryGetValue("/Script/CleverAdsSolutions.CASSettings", "EnableGoogleAdMobIOS", out EnableGoogleAdMob);
			EngineConfig.TryGetValue("/Script/CleverAdsSolutions.CASSettings", "EnableAdColonyIOS", out EnableAdColony);
			EngineConfig.TryGetValue("/Script/CleverAdsSolutions.CASSettings", "EnableMintegralIOS", out EnableMintegral);
			EngineConfig.TryGetValue("/Script/CleverAdsSolutions.CASSettings", "EnableMyTargetIOS", out EnableMyTarget);
			EngineConfig.TryGetValue("/Script/CleverAdsSolutions.CASSettings", "EnableInMobiIOS", out EnableInMobi);
			EngineConfig.TryGetValue("/Script/CleverAdsSolutions.CASSettings", "EnablePangleIOS", out EnablePangle);
			EngineConfig.TryGetValue("/Script/CleverAdsSolutions.CASSettings", "EnableTapjoyIOS", out EnableTapjoy);
			EngineConfig.TryGetValue("/Script/CleverAdsSolutions.CASSettings", "EnableUnityAdsIOS", out EnableUnityAds);
			EngineConfig.TryGetValue("/Script/CleverAdsSolutions.CASSettings", "EnableVungleIOS", out EnableVungle);
			EngineConfig.TryGetValue("/Script/CleverAdsSolutions.CASSettings", "EnableYandexIOS", out EnableYandex);
			
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
					"DataDetection",
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
			
			Libraries.Add("CleverAdsSolutions", "");

			// Adapters -------

			if (EnableAppLovin)
			{
				Libraries.Add("AppLovinSDK", "AppLovinSDKResources");
				Libraries.Add("CASAdapterApplovin", "");
			}

			if (EnableFacebook)
			{
				Libraries.Add("FBAudienceNetwork", "");
				Libraries.Add("CASAdapterAudienceNetwork", "");
			}

			if (EnableIronsource)
			{
				Libraries.Add("IronSource", "");
				Libraries.Add("CASAdapterIronsource", "");
			}

			if (EnableGoogleAdMob)
			{
				Libraries.Add("GoogleMobileAds", "");
				Libraries.Add("CASAdapterGoogleAds", "");
				Libraries.Add("GoogleAppMeasurement", "");
				Libraries.Add("GoogleAppMeasurementIdentitySupport", "");
				Libraries.Add("GoogleUtilities", "");
				Libraries.Add("nanopb", "");
				Libraries.Add("PromisesObjC", "");
			}

			if (EnableAdColony)
			{
				Libraries.Add("CASmAdColonyAdapter", "");
				Libraries.Add("CASiAdColonyAdapter", "");
				Libraries.Add("CASAdapterAdColony", "");
				Libraries.Add("AdColony", "");
			}

			if (EnableMintegral)
			{
				Libraries.Add("CASmMintegralAdapter", "");
				Libraries.Add("CASAdapterMintegral", "");
				Libraries.Add("MTGSDK", "");
				Libraries.Add("MTGSDKBanner", "");
				Libraries.Add("MTGSDKBidding", "");
				Libraries.Add("MTGSDKCNAddition", "");
				Libraries.Add("MTGSDKInterstitial", "");
				Libraries.Add("MTGSDKInterstitialVideo", "");
				Libraries.Add("MTGSDKNativeAdvanced", "");
				Libraries.Add("MTGSDKNewInterstitial", "");
				Libraries.Add("MTGSDKReward", "");
				Libraries.Add("MTGSDKSplash", "");
			}

			if (EnableMyTarget)
			{
				Libraries.Add("CASmMyTargetAdapter", "");
				Libraries.Add("CASiMyTargetAdapter", "");
				Libraries.Add("CASAdapterMyTarget", "");
				Libraries.Add("MyTargetSDK", "");
				Libraries.Add("MyTrackerSDK", "");
			}

			if (EnableInMobi)
			{
				Libraries.Add("CASmInMobiAdapter", "");
				Libraries.Add("CASiInMobiAdapter", "");
				Libraries.Add("CASAdapterInMobi", "");
				Libraries.Add("InMobiSDK", "");
			}

			if (EnablePangle)
			{
				Libraries.Add("BUAdSDK", "");
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
				Libraries.Add("RARegisterKit-Pangle", "");
				Libraries.Add("RangersAPM-Pangle", "");
				Libraries.Add("OneKit-Pangle", "");
				Libraries.Add("CASAdapterPangle", "");
				Libraries.Add("CASiPangleAdapter", "");
			}

			if (EnableTapjoy)
			{
				Libraries.Add("CASmTapjoyAdapter", "");
				Libraries.Add("CASiTapjoyAdapter", "");
				Libraries.Add("CASAdapterTapjoy", "");
				Libraries.Add("Tapjoy", "");
			}

			if (EnableUnityAds)
			{
				Libraries.Add("CASiUnityAdsAdapter", "");
				Libraries.Add("CASAdapterUnityAds", "");
				Libraries.Add("UnityAds", "");
			}

			if (EnableVungle)
			{
				Libraries.Add("CASmVungleAdapter", "");
				Libraries.Add("CASiVungleAdapter", "");
				Libraries.Add("CASAdapterVungle", "");
				Libraries.Add("VungleSDK", "");
			}			

			if (EnableYandex)
			{
				Libraries.Add("CASAdapterYandexAds", "");
				Libraries.Add("YandexMobileAds", "YandexMobileAdsBundle.bundle");
				Libraries.Add("YandexMobileMetrica", "");
				Libraries.Add("YandexMobileMetricaCrashes", "");
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
