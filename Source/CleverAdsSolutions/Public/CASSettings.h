// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "CASSettings.generated.h"

/** Ads solution preset, which determines which ad networks will be included in build */ 
UENUM()
enum class ECASAdsSolution : uint8
{
	Custom,
	Optimal,
	Families
};

// TODO: One base config class

/** Clever Ads Solutions Android config object for Project Settings tab */
UCLASS(Config = Engine, DefaultConfig)
class CLEVERADSSOLUTIONS_API UCASSettingsAndroid : public UObject
{
	GENERATED_BODY()
	
public:

	/** CAS Settings file path. Project relative.
	 * It is recommended to place it in Config folder, so your path would look like 'Config/cas_settings***.json'
	 */
	UPROPERTY(Config, EditAnywhere, Category="Integration")
	FString SettingsFile;

	/** CAS Settings file id */
	UPROPERTY(Config, VisibleAnywhere, Category="Integration")
	FString SettingsFileID;
	
	/** CAS App ID. */
	UPROPERTY(Config, EditAnywhere, Category="Integration")
	FString CASAppID;
	
	/** Enable Cross Promo ads */
	UPROPERTY(Config, EditAnywhere, Category="Integration")
	bool EnableCrossPromo;

	UPROPERTY(Config, EditAnywhere, Category="AdTypes")
	bool BannerAds;

	UPROPERTY(Config, EditAnywhere, Category="AdTypes")
	bool InterstitialAds;

	UPROPERTY(Config, EditAnywhere, Category="AdTypes")
	bool RewardedAds;

	UPROPERTY(Config, EditAnywhere, Category="AdTypes")
	bool ReturnAds;

	UPROPERTY(Config, EditAnywhere, Category="AdSettings", meta = (UIMin = 10, ClampMin = 10))
	float BannerDefaultRefreshInterval = 30.f;

	UPROPERTY(Config, EditAnywhere, Category="AdSettings", meta = (UIMin = 0, ClampMin = 0))
	float MinimumInterstitialInterval = 0.f;

	UPROPERTY(Config, EditAnywhere, Category="AdSettings")
	bool AllowInterstitialWhenRewardsCostAreLower;
	
	UPROPERTY(Config, EditAnywhere, Category="Privacy")
	bool ConsentFlow;

	/** Removes permission to use Advertisement ID (AD_ID) */
	UPROPERTY(Config, EditAnywhere, Category="Privacy", meta=(EditCondition="AdsSolution == ECASAdsSolution::Families"))
	bool RemoveAdIdPermission;
	
	// Enable Networks -----

	UPROPERTY(Config, EditAnywhere, Category="Networks")
	ECASAdsSolution AdsSolution;

	// IOS ---
	
	UPROPERTY(Config, EditAnywhere, Category="Networks|AppLovin", meta=(EditCondition="AdsSolution == ECASAdsSolution::Custom"))
	bool EnableAppLovin;

	UPROPERTY(Config, EditAnywhere, Category="Networks|Facebook", meta=(EditCondition="AdsSolution == ECASAdsSolution::Custom"))
	bool EnableFacebook;
	
	UPROPERTY(Config, EditAnywhere, Category="Networks|Ironsource", meta=(EditCondition="AdsSolution == ECASAdsSolution::Custom"))
	bool EnableIronsource;
	
	UPROPERTY(Config, EditAnywhere, Category="Networks|GoogleAdMob", meta=(EditCondition="AdsSolution == ECASAdsSolution::Custom"))
	bool EnableGoogleAdMob;
	
	UPROPERTY(Config, EditAnywhere, Category="Networks|AdColony", meta=(EditCondition="AdsSolution == ECASAdsSolution::Custom"))
	bool EnableAdColony;
	
	UPROPERTY(Config, EditAnywhere, Category="Networks|Mintegral", meta=(EditCondition="AdsSolution == ECASAdsSolution::Custom"))
	bool EnableMintegral;
	
	UPROPERTY(Config, EditAnywhere, Category="Networks|MyTarget", meta=(EditCondition="AdsSolution == ECASAdsSolution::Custom"))
	bool EnableMyTarget;
	
	UPROPERTY(Config, EditAnywhere, Category="Networks|InMobi", meta=(EditCondition="AdsSolution == ECASAdsSolution::Custom"))
	bool EnableInMobi;

	UPROPERTY(Config, EditAnywhere, Category="Networks|Pangle", meta=(EditCondition="AdsSolution == ECASAdsSolution::Custom"))
	bool EnablePangle;
	
	UPROPERTY(Config, EditAnywhere, Category="Networks|Tapjoy", meta=(EditCondition="AdsSolution == ECASAdsSolution::Custom"))
	bool EnableTapjoy;
	
	UPROPERTY(Config, EditAnywhere, Category="Networks|UnityAds", meta=(EditCondition="AdsSolution == ECASAdsSolution::Custom"))
	bool EnableUnityAds;
		
	UPROPERTY(Config, EditAnywhere, Category="Networks|Vungle", meta=(EditCondition="AdsSolution == ECASAdsSolution::Custom"))
	bool EnableVungle;

	UPROPERTY(Config, EditAnywhere, Category="Networks|Yandex", meta=(EditCondition="AdsSolution == ECASAdsSolution::Custom"))
	bool EnableYandex;

	// Network settings  -------

	// Admob ---

	/** Admob App ID for Android */
	UPROPERTY(Config, EditAnywhere, Category="Networks|GoogleAdMob", meta=(EditCondition="EnableGoogleAdMob"))
	FString AdMobAppID;

	// Debug  -------
	
	/** Toggle test ads mode. When enabled, test ads will be served instead of real ones */
	UPROPERTY(Config, EditAnywhere, Category="Debug")
	bool TestAds;

	/** Toggle debug mode. When enabled, additional information will be printed to logs */
	UPROPERTY(Config, EditAnywhere, Category="Debug")
	bool DebugMode;

	/** List of test devices to show tests ads to, even with TestAds disabled */
	UPROPERTY(Config, EditAnywhere, Category="Debug")
	TArray<FString> TestDevices;

	/** Disables TestAds and DebugMode when building shipping builds. */
	UPROPERTY(Config, EditAnywhere, Category="Debug")
	bool DisableDevModesInShipping = true;

	// Last Page Ad -------

	//** The message that you want users to see */
	UPROPERTY(Config, EditAnywhere, Category="LastPageAd")
	FString Headline;

	//** A description for the app being promoted */
	UPROPERTY(Config, EditAnywhere, Category="LastPageAd")
	FString AdText;

	//** The URL that CAS will direct users to when they click the ad */
	UPROPERTY(Config, EditAnywhere, Category="LastPageAd")
	FString DestinationURL;

	//** The direct URL of the image to be used as the ad file */
	UPROPERTY(Config, EditAnywhere, Category="LastPageAd")
	FString ImageURL;

	//** The direct URL of the icon or logo (Small square picture) */
	UPROPERTY(Config, EditAnywhere, Category="LastPageAd")
	FString IconURL;

#if WITH_EDITOR
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override
	{
		Super::PostEditChangeProperty(PropertyChangedEvent);

		if (PropertyChangedEvent.Property->GetFName() == GET_MEMBER_NAME_CHECKED(UCASSettingsAndroid, SettingsFile))
		{
			FString StrRef;
			SettingsFile.Split(".json", &SettingsFileID, &StrRef, ESearchCase::IgnoreCase);
			SettingsFileID.Split("cas_settings", &StrRef, &SettingsFileID, ESearchCase::IgnoreCase);
			UpdateSinglePropertyInConfigFile(GetClass()->FindPropertyByName(GET_MEMBER_NAME_CHECKED(UCASSettingsAndroid, SettingsFileID)), GetDefaultConfigFilename());
		}
		
		// Android Ads Solution preset 
		if (PropertyChangedEvent.Property->GetFName() == GET_MEMBER_NAME_CHECKED(UCASSettingsAndroid, AdsSolution))
		{
			switch (AdsSolution)
			{
			case ECASAdsSolution::Optimal:
				EnableAppLovin = true;
				EnableFacebook = true;
				EnableIronsource = true;
				EnableGoogleAdMob = true;
				EnableAdColony = true;
				EnableMintegral = true;
				EnableMyTarget = true;
				EnableInMobi = true;
				EnablePangle = true;
				EnableTapjoy = true;
				EnableUnityAds = true;
				EnableVungle = true;
				EnableYandex = true;
				break;
			case ECASAdsSolution::Families:
				EnableAppLovin = true;
				EnableFacebook = false;
				EnableIronsource = true;
				EnableGoogleAdMob = true;
				EnableAdColony = true;
				EnableMintegral = false;
				EnableMyTarget = false;
				EnableInMobi = true;
				EnablePangle = false;
				EnableTapjoy = false;
				EnableUnityAds = true;
				EnableVungle= true;
				EnableYandex = false;

				RemoveAdIdPermission = false;
				break;
			default: break;
			}

			if(AdsSolution != ECASAdsSolution::Custom)
			{
				UpdateSinglePropertyInConfigFile(GetClass()->FindPropertyByName(GET_MEMBER_NAME_CHECKED(UCASSettingsAndroid, EnableAppLovin)), GetDefaultConfigFilename());
				UpdateSinglePropertyInConfigFile(GetClass()->FindPropertyByName(GET_MEMBER_NAME_CHECKED(UCASSettingsAndroid, EnableFacebook)), GetDefaultConfigFilename());
				UpdateSinglePropertyInConfigFile(GetClass()->FindPropertyByName(GET_MEMBER_NAME_CHECKED(UCASSettingsAndroid, EnableIronsource)), GetDefaultConfigFilename());
				UpdateSinglePropertyInConfigFile(GetClass()->FindPropertyByName(GET_MEMBER_NAME_CHECKED(UCASSettingsAndroid, EnableGoogleAdMob)), GetDefaultConfigFilename());
				UpdateSinglePropertyInConfigFile(GetClass()->FindPropertyByName(GET_MEMBER_NAME_CHECKED(UCASSettingsAndroid, EnableAdColony)), GetDefaultConfigFilename());
				UpdateSinglePropertyInConfigFile(GetClass()->FindPropertyByName(GET_MEMBER_NAME_CHECKED(UCASSettingsAndroid, EnableMintegral)), GetDefaultConfigFilename());
				UpdateSinglePropertyInConfigFile(GetClass()->FindPropertyByName(GET_MEMBER_NAME_CHECKED(UCASSettingsAndroid, EnableMyTarget)), GetDefaultConfigFilename());
				UpdateSinglePropertyInConfigFile(GetClass()->FindPropertyByName(GET_MEMBER_NAME_CHECKED(UCASSettingsAndroid, EnableInMobi)), GetDefaultConfigFilename());
				UpdateSinglePropertyInConfigFile(GetClass()->FindPropertyByName(GET_MEMBER_NAME_CHECKED(UCASSettingsAndroid, EnablePangle)), GetDefaultConfigFilename());
				UpdateSinglePropertyInConfigFile(GetClass()->FindPropertyByName(GET_MEMBER_NAME_CHECKED(UCASSettingsAndroid, EnableTapjoy)), GetDefaultConfigFilename());
				UpdateSinglePropertyInConfigFile(GetClass()->FindPropertyByName(GET_MEMBER_NAME_CHECKED(UCASSettingsAndroid, EnableUnityAds)), GetDefaultConfigFilename());
				UpdateSinglePropertyInConfigFile(GetClass()->FindPropertyByName(GET_MEMBER_NAME_CHECKED(UCASSettingsAndroid, EnableVungle)), GetDefaultConfigFilename());
				UpdateSinglePropertyInConfigFile(GetClass()->FindPropertyByName(GET_MEMBER_NAME_CHECKED(UCASSettingsAndroid, EnableYandex)), GetDefaultConfigFilename());
				
				UpdateSinglePropertyInConfigFile(GetClass()->FindPropertyByName(GET_MEMBER_NAME_CHECKED(UCASSettingsAndroid, RemoveAdIdPermission)), GetDefaultConfigFilename());
			}
		}
	}
#endif
};

/** Clever Ads Solutions IOS config object for Project Settings tab */
UCLASS(Config = Engine, DefaultConfig)
class CLEVERADSSOLUTIONS_API UCASSettingsIOS : public UObject
{
	
public:

	/** CAS Settings file path. Project relative.
	 * It is recommended to place it in Config folder, so your path would look like 'Config/cas_settings***.json'
	 */
	UPROPERTY(Config, EditAnywhere, Category="Integration")
	FString SettingsFile;

	/** CAS Settings file id */
	UPROPERTY(Config, VisibleAnywhere, Category="Integration")
	FString SettingsFileID;
	
	/** CAS App ID. */
	UPROPERTY(Config, EditAnywhere, Category="Integration")
	FString CASAppID;
		
	/** Enable Cross Promo ads */
	UPROPERTY(Config, EditAnywhere, Category="Integration")
	bool EnableCrossPromo;

	UPROPERTY(Config, EditAnywhere, Category="AdTypes")
	bool BannerAds;

	UPROPERTY(Config, EditAnywhere, Category="AdTypes")
	bool InterstitialAds;

	UPROPERTY(Config, EditAnywhere, Category="AdTypes")
	bool RewardedAds;
	
	UPROPERTY(Config, EditAnywhere, Category="AdTypes")
    bool ReturnAds;

	UPROPERTY(Config, EditAnywhere, Category="AdSettings", meta = (UIMin = 10, ClampMin = 10))
	float BannerDefaultRefreshInterval = 30.f;

	UPROPERTY(Config, EditAnywhere, Category="AdSettings", meta = (UIMin = 0, ClampMin = 0))
	float MinimumInterstitialInterval = 0.f;

	UPROPERTY(Config, EditAnywhere, Category="AdSettings")
	bool AllowInterstitialWhenRewardsCostAreLower;
	
	UPROPERTY(Config, EditAnywhere, Category="Privacy")
	bool ConsentFlow;

	UPROPERTY(Config, EditAnywhere, Category="Privacy")
	FString ConsentFlowURL;

	UPROPERTY(Config, EditAnywhere, Category="Privacy")
	FString UserTrackingUsageDescription;
	
	// Enable Networks -----

	UPROPERTY(Config, EditAnywhere, Category="Networks")
	ECASAdsSolution AdsSolution;

	// IOS ---
	
	UPROPERTY(Config, EditAnywhere, Category="Networks|AppLovin", meta=(EditCondition="AdsSolution == ECASAdsSolution::Custom"))
	bool EnableAppLovin;

	UPROPERTY(Config, EditAnywhere, Category="Networks|Facebook", meta=(EditCondition="AdsSolution == ECASAdsSolution::Custom"))
	bool EnableFacebook;
	
	UPROPERTY(Config, EditAnywhere, Category="Networks|Ironsource", meta=(EditCondition="AdsSolution == ECASAdsSolution::Custom"))
	bool EnableIronsource;
	
	UPROPERTY(Config, EditAnywhere, Category="Networks|GoogleAdMob", meta=(EditCondition="AdsSolution == ECASAdsSolution::Custom"))
	bool EnableGoogleAdMob;
	
	UPROPERTY(Config, EditAnywhere, Category="Networks|AdColony", meta=(EditCondition="AdsSolution == ECASAdsSolution::Custom"))
	bool EnableAdColony;
	
	UPROPERTY(Config, EditAnywhere, Category="Networks|Mintegral", meta=(EditCondition="AdsSolution == ECASAdsSolution::Custom"))
	bool EnableMintegral;
	
	UPROPERTY(Config, EditAnywhere, Category="Networks|MyTarget", meta=(EditCondition="AdsSolution == ECASAdsSolution::Custom"))
	bool EnableMyTarget;
	
	UPROPERTY(Config, EditAnywhere, Category="Networks|InMobi", meta=(EditCondition="AdsSolution == ECASAdsSolution::Custom"))
	bool EnableInMobi;

	UPROPERTY(Config, EditAnywhere, Category="Networks|Pangle", meta=(EditCondition="AdsSolution == ECASAdsSolution::Custom"))
	bool EnablePangle;
	
	UPROPERTY(Config, EditAnywhere, Category="Networks|Tapjoy", meta=(EditCondition="AdsSolution == ECASAdsSolution::Custom"))
	bool EnableTapjoy;
	
	UPROPERTY(Config, EditAnywhere, Category="Networks|UnityAds", meta=(EditCondition="AdsSolution == ECASAdsSolution::Custom"))
	bool EnableUnityAds;
		
	UPROPERTY(Config, EditAnywhere, Category="Networks|Vungle", meta=(EditCondition="AdsSolution == ECASAdsSolution::Custom"))
	bool EnableVungle;

	UPROPERTY(Config, EditAnywhere, Category="Networks|Yandex", meta=(EditCondition="AdsSolution == ECASAdsSolution::Custom"))
	bool EnableYandex;

	// Network settings  -------

	// Admob ---

	/** Admob App ID for Android */
	UPROPERTY(Config, EditAnywhere, Category="Networks|GoogleAdMob", meta=(EditCondition="EnableGoogleAdMob"))
	FString AdMobAppID;

	/** Facebook AdvertiserTrackingEnabled flag */
	UPROPERTY(Config, EditAnywhere, Category="Networks|Facebook", meta=(EditCondition="EnableFacebook"))
	bool AdvertiserTrackingEnabled;

	// Debug  -------
	
	/** Toggle test ads mode. When enabled, test ads will be served instead of real ones */
	UPROPERTY(Config, EditAnywhere, Category="Debug")
	bool TestAds;

	/** Toggle debug mode. When enabled, additional information will be printed to logs */
	UPROPERTY(Config, EditAnywhere, Category="Debug")
	bool DebugMode;

	/** List of test devices to show tests ads to, even with TestAds disabled */
	UPROPERTY(Config, EditAnywhere, Category="Debug")
	TArray<FString> TestDevices;

	/** Disables TestAds and DebugMode when building shipping builds. */
	UPROPERTY(Config, EditAnywhere, Category="Debug")
	bool DisableDevModesInShipping = true;

	// Last Page Ad -------
	
	//** The message that you want users to see */
	UPROPERTY(Config, EditAnywhere, Category="LastPageAd")
	FString Headline;

	//** A description for the app being promoted */
	UPROPERTY(Config, EditAnywhere, Category="LastPageAd")
	FString AdText;

	//** The URL that CAS will direct users to when they click the ad */
	UPROPERTY(Config, EditAnywhere, Category="LastPageAd")
	FString DestinationURL;

	//** The direct URL of the image to be used as the ad file */
	UPROPERTY(Config, EditAnywhere, Category="LastPageAd")
	FString ImageURL;

	//** The direct URL of the icon or logo (Small square picture) */
	UPROPERTY(Config, EditAnywhere, Category="LastPageAd")
	FString IconURL;

#if WITH_EDITOR
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override
	{
		Super::PostEditChangeProperty(PropertyChangedEvent);

		if (PropertyChangedEvent.Property->GetFName() == GET_MEMBER_NAME_CHECKED(UCASSettingsIOS, SettingsFile))
		{
			FString StrRef;
			SettingsFile.Split(".json", &SettingsFileID, &StrRef, ESearchCase::IgnoreCase);
			SettingsFileID.Split("cas_settings", &StrRef, &SettingsFileID, ESearchCase::IgnoreCase);
			UpdateSinglePropertyInConfigFile(GetClass()->FindPropertyByName(GET_MEMBER_NAME_CHECKED(UCASSettingsIOS, SettingsFileID)), GetDefaultConfigFilename());
		}
		
		// Android Ads Solution preset 
		if (PropertyChangedEvent.Property->GetFName() == GET_MEMBER_NAME_CHECKED(UCASSettingsIOS, AdsSolution))
		{
			switch (AdsSolution)
			{
			case ECASAdsSolution::Optimal:
				EnableAppLovin = true;
				EnableFacebook = true;
				EnableIronsource = true;
				EnableGoogleAdMob = true;
				EnableAdColony = true;
				EnableMintegral = true;
				EnableMyTarget = true;
				EnableInMobi = true;
				EnablePangle = true;
				EnableTapjoy = true;
				EnableUnityAds = true;
				EnableVungle = true;
				EnableYandex = true;
				break;
			case ECASAdsSolution::Families:
				EnableAppLovin = true;
				EnableFacebook = false;
				EnableIronsource = true;
				EnableGoogleAdMob = true;
				EnableAdColony = true;
				EnableMintegral = true;
				EnableMyTarget = true;
				EnableInMobi = true;
				EnablePangle = true;
				EnableTapjoy = true;
				EnableUnityAds = true;
				EnableVungle= true;
				EnableYandex = false;
				break;
			default: break;
			}

			if(AdsSolution != ECASAdsSolution::Custom)
			{
				UpdateSinglePropertyInConfigFile(GetClass()->FindPropertyByName(GET_MEMBER_NAME_CHECKED(UCASSettingsIOS, EnableAppLovin)), GetDefaultConfigFilename());
				UpdateSinglePropertyInConfigFile(GetClass()->FindPropertyByName(GET_MEMBER_NAME_CHECKED(UCASSettingsIOS, EnableFacebook)), GetDefaultConfigFilename());
				UpdateSinglePropertyInConfigFile(GetClass()->FindPropertyByName(GET_MEMBER_NAME_CHECKED(UCASSettingsIOS, EnableIronsource)), GetDefaultConfigFilename());
				UpdateSinglePropertyInConfigFile(GetClass()->FindPropertyByName(GET_MEMBER_NAME_CHECKED(UCASSettingsIOS, EnableGoogleAdMob)), GetDefaultConfigFilename());
				UpdateSinglePropertyInConfigFile(GetClass()->FindPropertyByName(GET_MEMBER_NAME_CHECKED(UCASSettingsIOS, EnableAdColony)), GetDefaultConfigFilename());
				UpdateSinglePropertyInConfigFile(GetClass()->FindPropertyByName(GET_MEMBER_NAME_CHECKED(UCASSettingsIOS, EnableMintegral)), GetDefaultConfigFilename());
				UpdateSinglePropertyInConfigFile(GetClass()->FindPropertyByName(GET_MEMBER_NAME_CHECKED(UCASSettingsIOS, EnableMyTarget)), GetDefaultConfigFilename());
				UpdateSinglePropertyInConfigFile(GetClass()->FindPropertyByName(GET_MEMBER_NAME_CHECKED(UCASSettingsIOS, EnableInMobi)), GetDefaultConfigFilename());
				UpdateSinglePropertyInConfigFile(GetClass()->FindPropertyByName(GET_MEMBER_NAME_CHECKED(UCASSettingsIOS, EnablePangle)), GetDefaultConfigFilename());
				UpdateSinglePropertyInConfigFile(GetClass()->FindPropertyByName(GET_MEMBER_NAME_CHECKED(UCASSettingsIOS, EnableTapjoy)), GetDefaultConfigFilename());
				UpdateSinglePropertyInConfigFile(GetClass()->FindPropertyByName(GET_MEMBER_NAME_CHECKED(UCASSettingsIOS, EnableUnityAds)), GetDefaultConfigFilename());
				UpdateSinglePropertyInConfigFile(GetClass()->FindPropertyByName(GET_MEMBER_NAME_CHECKED(UCASSettingsIOS, EnableVungle)), GetDefaultConfigFilename());
				UpdateSinglePropertyInConfigFile(GetClass()->FindPropertyByName(GET_MEMBER_NAME_CHECKED(UCASSettingsIOS, EnableYandex)), GetDefaultConfigFilename());
			}
		}
	}
#endif

	GENERATED_BODY()
};