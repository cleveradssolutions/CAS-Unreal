// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;
using System.IO;
using Tools.DotNETCommon;

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
			bool EnableTapjoy = false;
			bool EnableUnityAds = false;
			bool EnableVungle = false;
            
			EngineConfig.TryGetValue("/Script/CleverAdsSolutions.CASSettings", "EnableAppLovinIOS", out EnableAppLovin);
			EngineConfig.TryGetValue("/Script/CleverAdsSolutions.CASSettings", "EnableFacebookIOS", out EnableFacebook);
			EngineConfig.TryGetValue("/Script/CleverAdsSolutions.CASSettings", "EnableIronsourceIOS", out EnableIronsource);
			EngineConfig.TryGetValue("/Script/CleverAdsSolutions.CASSettings", "EnableGoogleAdMobIOS", out EnableGoogleAdMob);
			EngineConfig.TryGetValue("/Script/CleverAdsSolutions.CASSettings", "EnableAdColonyIOS", out EnableAdColony);
			EngineConfig.TryGetValue("/Script/CleverAdsSolutions.CASSettings", "EnableMintegralIOS", out EnableMintegral);
			EngineConfig.TryGetValue("/Script/CleverAdsSolutions.CASSettings", "EnableMyTargetIOS", out EnableMyTarget);
			EngineConfig.TryGetValue("/Script/CleverAdsSolutions.CASSettings", "EnableInMobiIOS", out EnableInMobi);
			EngineConfig.TryGetValue("/Script/CleverAdsSolutions.CASSettings", "EnableTapjoyIOS", out EnableTapjoy);
			EngineConfig.TryGetValue("/Script/CleverAdsSolutions.CASSettings", "EnableUnityAdsIOS", out EnableUnityAds);
			EngineConfig.TryGetValue("/Script/CleverAdsSolutions.CASSettings", "EnableVungleIOS", out EnableVungle);
			
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
					"DataDetection"
				}
			);
			
			PublicSystemLibraries.Add("c++");
			PublicSystemLibraries.Add("z");
			PublicSystemLibraries.Add("sqlite3");
			
			PublicAdditionalFrameworks.Add(
				new Framework(
					"CleverAdsSolutions",
					"External/IOS/CleverAdsSolutions.embeddedframework.zip"
				)
			);
			
			// Adapters -------
			
			// AppLovin --

			if (EnableAppLovin)
			{
				PublicAdditionalFrameworks.Add(
					new Framework(
						"AppLovinSDK",
						"External/iOS/AppLovinSDK.embeddedframework.zip",
						"AppLovinSDKResources.bundle"
					)
				);
			}
			
			// Facebook --

			if (EnableFacebook)
			{
				PublicAdditionalFrameworks.Add(
					new Framework(
						"FBAudienceNetwork",
						"External/iOS/FBAudienceNetwork.embeddedframework.zip"
					)
				);
			}

			// Ironsource --

			if (EnableIronsource)
			{
				PublicAdditionalFrameworks.Add(
					new Framework(
						"IronSource",
						"External/iOS/IronSource.embeddedframework.zip"
					)
				);
			}

			// Google AdMob --

			if (EnableGoogleAdMob)
			{
				PublicAdditionalFrameworks.Add(
					new Framework(
						"GoogleMobileAds",
						"External/iOS/GoogleMobileAds.embeddedframework.zip"
					)
				);		
			 
				PublicAdditionalFrameworks.Add(
					new Framework(
						"GoogleAppMeasurement",
						"External/iOS/GoogleAppMeasurement.embeddedframework.zip"
					)
				);		
			 
				PublicAdditionalFrameworks.Add(
					new Framework(
						"GoogleAppMeasurementIdentitySupport",
						"External/iOS/GoogleAppMeasurementIdentitySupport.embeddedframework.zip"
					)
				);
			 
				PublicAdditionalFrameworks.Add(
					new Framework(
						"GoogleUtilities",
						"External/iOS/GoogleUtilities.embeddedframework.zip"
					)
				);		
			 
				PublicAdditionalFrameworks.Add(
					new Framework(
						"nanopb",
						"External/iOS/nanopb.embeddedframework.zip"
					)
				);
			 
				PublicAdditionalFrameworks.Add(
					new Framework(
						"PromisesObjC",
						"External/iOS/PromisesObjC.embeddedframework.zip"
					)
				);
			}

			// AdColony --

			if (EnableAdColony)
			{
				PublicAdditionalFrameworks.Add(
					new Framework(
						"CASmAdColonyAdapter",
						"External/iOS/CASmAdColonyAdapter.embeddedframework.zip"
					)
				);
			 
				PublicAdditionalFrameworks.Add(
					new Framework(
						"CASiAdColonyAdapter",
						"External/iOS/CASiAdColonyAdapter.embeddedframework.zip"
					)
				);
			 
				PublicAdditionalFrameworks.Add(
					new Framework(
						"AdColony",
						"External/iOS/AdColony.embeddedframework.zip"
					)
				);
			}

			// Mintegral --

			if (EnableMintegral)
			{
				PublicAdditionalFrameworks.Add(
					new Framework(
						"CASmMintegralAdapter",
						"External/iOS/CASmMintegralAdapter.embeddedframework.zip"
					)
				);
			 
				PublicAdditionalFrameworks.Add(
					new Framework(
						"MTGSDK",
						"External/iOS/MTGSDK.embeddedframework.zip"
					)
				);
				PublicAdditionalFrameworks.Add(
					new Framework(
						"MTGSDKBanner",
						"External/iOS/MTGSDKBanner.embeddedframework.zip"
					)
				);
				PublicAdditionalFrameworks.Add(
					new Framework(
						"MTGSDKBidding",
						"External/iOS/MTGSDKBidding.embeddedframework.zip"
					)
				);
				PublicAdditionalFrameworks.Add(
					new Framework(
						"MTGSDKCNAddition",
						"External/iOS/MTGSDKCNAddition.embeddedframework.zip"
					)
				);
				PublicAdditionalFrameworks.Add(
					new Framework(
						"MTGSDKInterstitial",
						"External/iOS/MTGSDKInterstitial.embeddedframework.zip"
					)
				);
				PublicAdditionalFrameworks.Add(
					new Framework(
						"MTGSDKInterstitialVideo",
						"External/iOS/MTGSDKInterstitialVideo.embeddedframework.zip"
					)
				);
				PublicAdditionalFrameworks.Add(
					new Framework(
						"MTGSDKNativeAdvanced",
						"External/iOS/MTGSDKNativeAdvanced.embeddedframework.zip"
					)
				);			 
				PublicAdditionalFrameworks.Add(
					new Framework(
						"MTGSDKNewInterstitial",
						"External/iOS/MTGSDKNewInterstitial.embeddedframework.zip"
					)
				);
				PublicAdditionalFrameworks.Add(
					new Framework(
						"MTGSDKReward",
						"External/iOS/MTGSDKReward.embeddedframework.zip"
					)
				);
				PublicAdditionalFrameworks.Add(
					new Framework(
						"MTGSDKSplash",
						"External/iOS/MTGSDKSplash.embeddedframework.zip"
					)
				);
			}

			// MyTarget

			if (EnableMyTarget)
			{
				PublicAdditionalFrameworks.Add(
					new Framework(
						"CASmMyTargetAdapter",
						"External/iOS/CASmMyTargetAdapter.embeddedframework.zip"
					)
				);
			 
				PublicAdditionalFrameworks.Add(
					new Framework(
						"CASiMyTargetAdapter",
						"External/iOS/CASiMyTargetAdapter.embeddedframework.zip"
					)
				);
			 
				PublicAdditionalFrameworks.Add(
					new Framework(
						"MyTargetSDK",
						"External/iOS/MyTargetSDK.embeddedframework.zip"
					)
				);	
			 
				PublicAdditionalFrameworks.Add(
					new Framework(
						"MyTrackerSDK",
						"External/iOS/MyTrackerSDK.embeddedframework.zip"
					)
				);
			}

			// InMobi --

			if (EnableInMobi)
			{
				PublicAdditionalFrameworks.Add(
					new Framework(
						"CASmInMobiAdapter",
						"External/iOS/CASmInMobiAdapter.embeddedframework.zip"
					)
				);
			
				PublicAdditionalFrameworks.Add(
					new Framework(
						"CASiInMobiAdapter",
						"External/iOS/CASiInMobiAdapter.embeddedframework.zip"
					)
				);
			
				PublicAdditionalFrameworks.Add(
					new Framework(
						"InMobiSDK",
						"External/iOS/InMobiSDK.embeddedframework.zip"
					)
				);
			}

			// Tapjoy --

			if (EnableTapjoy)
			{
				PublicAdditionalFrameworks.Add(
					new Framework(
						"CASmTapjoyAdapter",
						"External/iOS/CASmTapjoyAdapter.embeddedframework.zip"
					)
				);
			
				PublicAdditionalFrameworks.Add(
					new Framework(
						"CASiTapjoyAdapter",
						"External/iOS/CASiTapjoyAdapter.embeddedframework.zip"
					)
				);
			
				PublicAdditionalFrameworks.Add(
					new Framework(
						"Tapjoy",
						"External/iOS/Tapjoy.embeddedframework.zip"
					)
				);
			}

			// UnityAds --

			if (EnableUnityAds)
			{
				PublicAdditionalFrameworks.Add(
					new Framework(
						"CASiUnityAdsAdapter",
						"External/iOS/CASiUnityAdsAdapter.embeddedframework.zip"
					)
				);
			 
				PublicAdditionalFrameworks.Add(
					new Framework(
						"UnityAds",
						"External/iOS/UnityAds.embeddedframework.zip"
					)
				);
			}

			// VungleSDK --

			if (EnableVungle)
			{
				PublicAdditionalFrameworks.Add(
					new Framework(
						"CASmVungleAdapter",
						"External/iOS/CASmVungleAdapter.embeddedframework.zip"
					)
				);
			
				PublicAdditionalFrameworks.Add(
					new Framework(
						"CASiVungleAdapter",
						"External/iOS/CASiVungleAdapter.embeddedframework.zip"
					)
				);
			
				PublicAdditionalFrameworks.Add(
					new Framework(
						"VungleSDK",
						"External/iOS/VungleSDK.embeddedframework.zip"
					)
				);
			}

			string SDKROOT = Utils.RunLocalProcessAndReturnStdOut("/usr/bin/xcrun", "--sdk iphoneos --show-sdk-path");
			PublicSystemLibraryPaths.Add(SDKROOT + "/usr/lib/swift");
			PublicSystemLibraryPaths.Add(SDKROOT + "../../../../../../Toolchains/XcodeDefault.xctoolchain/usr/lib/swift/iphoneos");
			PublicSystemLibraryPaths.Add(SDKROOT + "../../../../../../Toolchains/XcodeDefault.xctoolchain/usr/lib/swift-5.0/iphoneos");
			PublicSystemLibraryPaths.Add(SDKROOT + "../../../../../../Toolchains/XcodeDefault.xctoolchain/usr/lib/swift-5.5/iphoneos");
		}
	}
}
