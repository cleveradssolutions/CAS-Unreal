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

/** Clever Ads Solutions config object for Project Settings tab */
UCLASS(Config = Engine, DefaultConfig)
class CLEVERADSSOLUTIONS_API UCASSettings : public UObject
{
	GENERATED_BODY()
	
public:

	/** Admob App ID for Android */
	UPROPERTY(Config, EditAnywhere)
	FString AndroidSettingsFile;

	/** Admob App ID for Android */
	UPROPERTY(Config, VisibleAnywhere)
	FString AndroidSettingsFileID;

	/** Admob App ID for IOS */
	UPROPERTY(Config, EditAnywhere)
	FString IOSSettingsFile;
		
	/** Admob App ID for IOS */
	UPROPERTY(Config, VisibleAnywhere)
	FString IOSSettingsFileID;
	
	/** CAS App ID for Android. In most cases, a casID is the same as your application package name. */
	UPROPERTY(Config, EditAnywhere)
	FString AndroidCASAppID;

	/** CAS App ID for IOS. In most cases, a casID is the same as your application store ID. */
	UPROPERTY(Config, EditAnywhere)
	FString IOSCASAppID;

	// Enable Networks -----

	UPROPERTY(Config, EditAnywhere, Category="Networks")
	ECASAdsSolution AdsSolutionAndroid;

	UPROPERTY(Config, EditAnywhere, Category="Networks")
	ECASAdsSolution AdsSolutionIOS;

	// IOS ---
	
	UPROPERTY(Config, EditAnywhere, Category="Networks|AppLovin", meta=(EditCondition="AdsSolutionIOS == ECASAdsSolution::Custom"))
	bool EnableAppLovinIOS;

	UPROPERTY(Config, EditAnywhere, Category="Networks|Facebook", meta=(EditCondition="AdsSolutionIOS == ECASAdsSolution::Custom"))
	bool EnableFacebookIOS;
	
	UPROPERTY(Config, EditAnywhere, Category="Networks|Ironsource", meta=(EditCondition="AdsSolutionIOS == ECASAdsSolution::Custom"))
	bool EnableIronsourceIOS;
	
	UPROPERTY(Config, EditAnywhere, Category="Networks|GoogleAdMob", meta=(EditCondition="AdsSolutionIOS == ECASAdsSolution::Custom"))
	bool EnableGoogleAdMobIOS;
	
	UPROPERTY(Config, EditAnywhere, Category="Networks|AdColony", meta=(EditCondition="AdsSolutionIOS == ECASAdsSolution::Custom"))
	bool EnableAdColonyIOS;
	
	UPROPERTY(Config, EditAnywhere, Category="Networks|Mintegral", meta=(EditCondition="AdsSolutionIOS == ECASAdsSolution::Custom"))
	bool EnableMintegralIOS;
	
	UPROPERTY(Config, EditAnywhere, Category="Networks|MyTarget", meta=(EditCondition="AdsSolutionIOS == ECASAdsSolution::Custom"))
	bool EnableMyTargetIOS;
	
	UPROPERTY(Config, EditAnywhere, Category="Networks|InMobi", meta=(EditCondition="AdsSolutionIOS == ECASAdsSolution::Custom"))
	bool EnableInMobiIOS;

	UPROPERTY(Config, EditAnywhere, Category="Networks|Pangle", meta=(EditCondition="AdsSolutionIOS == ECASAdsSolution::Custom"))
	bool EnablePangleIOS;
	
	UPROPERTY(Config, EditAnywhere, Category="Networks|Tapjoy", meta=(EditCondition="AdsSolutionIOS == ECASAdsSolution::Custom"))
	bool EnableTapjoyIOS;
	
	UPROPERTY(Config, EditAnywhere, Category="Networks|UnityAds", meta=(EditCondition="AdsSolutionIOS == ECASAdsSolution::Custom"))
	bool EnableUnityAdsIOS;
		
	UPROPERTY(Config, EditAnywhere, Category="Networks|Vungle", meta=(EditCondition="AdsSolutionIOS == ECASAdsSolution::Custom"))
	bool EnableVungleIOS;

	UPROPERTY(Config, EditAnywhere, Category="Networks|Yandex", meta=(EditCondition="AdsSolutionIOS == ECASAdsSolution::Custom"))
	bool EnableYandexIOS;

	// Android ---

	UPROPERTY(Config, EditAnywhere, Category="Networks|AppLovin", meta=(EditCondition="AdsSolutionAndroid == ECASAdsSolution::Custom"))
	bool EnableAppLovinAndroid;

	UPROPERTY(Config, EditAnywhere, Category="Networks|Facebook", meta=(EditCondition="AdsSolutionAndroid == ECASAdsSolution::Custom"))
	bool EnableFacebookAndroid;
	
	UPROPERTY(Config, EditAnywhere, Category="Networks|Ironsource", meta=(EditCondition="AdsSolutionAndroid == ECASAdsSolution::Custom"))
	bool EnableIronsourceAndroid;
	
	UPROPERTY(Config, EditAnywhere, Category="Networks|GoogleAdMob", meta=(EditCondition="AdsSolutionAndroid == ECASAdsSolution::Custom"))
	bool EnableGoogleAdMobAndroid;
	
	UPROPERTY(Config, EditAnywhere, Category="Networks|AdColony", meta=(EditCondition="AdsSolutionAndroid == ECASAdsSolution::Custom"))
	bool EnableAdColonyAndroid;
	
	UPROPERTY(Config, EditAnywhere, Category="Networks|Mintegral", meta=(EditCondition="AdsSolutionAndroid == ECASAdsSolution::Custom"))
	bool EnableMintegralAndroid;
	
	UPROPERTY(Config, EditAnywhere, Category="Networks|MyTarget", meta=(EditCondition="AdsSolutionAndroid == ECASAdsSolution::Custom"))
	bool EnableMyTargetAndroid;
	
	UPROPERTY(Config, EditAnywhere, Category="Networks|InMobi", meta=(EditCondition="AdsSolutionAndroid == ECASAdsSolution::Custom"))
	bool EnableInMobiAndroid;

	UPROPERTY(Config, EditAnywhere, Category="Networks|Pangle", meta=(EditCondition="AdsSolutionAndroid == ECASAdsSolution::Custom"))
	bool EnablePangleAndroid;
	
	UPROPERTY(Config, EditAnywhere, Category="Networks|Tapjoy", meta=(EditCondition="AdsSolutionAndroid == ECASAdsSolution::Custom"))
	bool EnableTapjoyAndroid;
	
	UPROPERTY(Config, EditAnywhere, Category="Networks|UnityAds", meta=(EditCondition="AdsSolutionAndroid == ECASAdsSolution::Custom"))
	bool EnableUnityAdsAndroid;
		
	UPROPERTY(Config, EditAnywhere, Category="Networks|Vungle", meta=(EditCondition="AdsSolutionAndroid == ECASAdsSolution::Custom"))
	bool EnableVungleAndroid;

	UPROPERTY(Config, EditAnywhere, Category="Networks|Yandex", meta=(EditCondition="AdsSolutionAndroid == ECASAdsSolution::Custom"))
	bool EnableYandexAndroid;

	// Network settings  -------

	// Admob ---

	/** Admob App ID for Android */
	UPROPERTY(Config, EditAnywhere, Category="Networks|GoogleAdMob", meta=(EditCondition="EnableGoogleAdMobAndroid"))
	FString AndroidAdMobAppID;

	/** Admob App ID for IOS */
	UPROPERTY(Config, EditAnywhere, Category="Networks|GoogleAdMob", meta=(EditCondition="EnableGoogleAdMobIOS"))
	FString IOSAdmobAppID;

	// Facebook ---

	/** Facebook AdvertiserTrackingEnabled flag */
	UPROPERTY(Config, EditAnywhere, Category="Networks|Facebook", meta=(EditCondition="EnableFacebookIOS"))
	bool IOSAdvertiserTrackingEnabled;

	// Debug  -------
	
	/** Toggle test ads mode. When enabled, test ads will be served instead of real ones */
	UPROPERTY(Config, EditAnywhere, Category="Debug")
	bool TestAds;

	/** Toggle debug mode. When enabled, additional information will be printed to logs */
	UPROPERTY(Config, EditAnywhere, Category="Debug")
	bool DebugMode;

	/** Disables TestAds and DebugMode when building shipping builds. */
	UPROPERTY(Config, EditAnywhere, Category="Debug")
	bool DisableDevModesInShipping = true;

#if WITH_EDITOR
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override
	{

		if (PropertyChangedEvent.Property->GetFName() == GET_MEMBER_NAME_CHECKED(UCASSettings, IOSSettingsFile))
		{
			FString StrRef;
			IOSSettingsFile.Split(".json", &IOSSettingsFileID, &StrRef, ESearchCase::IgnoreCase);
			IOSSettingsFileID.Split("cas_settings", &StrRef, &IOSSettingsFileID, ESearchCase::IgnoreCase);
			UpdateSinglePropertyInConfigFile(GetClass()->FindPropertyByName(GET_MEMBER_NAME_CHECKED(UCASSettings, IOSSettingsFileID)), GetDefaultConfigFilename());
		}
		if (PropertyChangedEvent.Property->GetFName() == GET_MEMBER_NAME_CHECKED(UCASSettings, AndroidSettingsFile))
		{
			FString StrRef;
			AndroidSettingsFile.Split(".json", &AndroidSettingsFileID, &StrRef, ESearchCase::IgnoreCase);
			AndroidSettingsFileID.Split("cas_settings", &StrRef, &AndroidSettingsFileID, ESearchCase::IgnoreCase);
			UpdateSinglePropertyInConfigFile(GetClass()->FindPropertyByName(GET_MEMBER_NAME_CHECKED(UCASSettings, AndroidSettingsFileID)), GetDefaultConfigFilename());
		}
		
		// Android Ads Solution preset 
		if (PropertyChangedEvent.Property->GetFName() == GET_MEMBER_NAME_CHECKED(UCASSettings, AdsSolutionAndroid))
		{
			switch (AdsSolutionAndroid)
			{
				case ECASAdsSolution::Optimal:
				EnableAppLovinAndroid = true;
				EnableFacebookAndroid = true;
				EnableIronsourceAndroid = true;
				EnableGoogleAdMobAndroid = true;
				EnableAdColonyAndroid = true;
				EnableMintegralAndroid = true;
				EnableMyTargetAndroid = true;
				EnableInMobiAndroid = true;
				EnablePangleAndroid = true;
				EnableTapjoyAndroid = true;
				EnableUnityAdsAndroid = true;
				EnableVungleAndroid = true;
				EnableYandexAndroid = false;
				break;
				case ECASAdsSolution::Families:
				EnableAppLovinAndroid = true;
				EnableFacebookAndroid = false;
				EnableIronsourceAndroid = true;
				EnableGoogleAdMobAndroid = true;
				EnableAdColonyAndroid = true;
				EnableMintegralAndroid = false;
				EnableMyTargetAndroid = false;
				EnableInMobiAndroid = true;
				EnablePangleAndroid = false;
				EnableTapjoyAndroid = false;
				EnableUnityAdsAndroid = true;
				EnableVungleAndroid = true;
				EnableYandexAndroid = false;
				break;
				default: break;
			}

			if(AdsSolutionAndroid != ECASAdsSolution::Custom)
			{
				UpdateSinglePropertyInConfigFile(GetClass()->FindPropertyByName(GET_MEMBER_NAME_CHECKED(UCASSettings, EnableAppLovinAndroid)), GetDefaultConfigFilename());
				UpdateSinglePropertyInConfigFile(GetClass()->FindPropertyByName(GET_MEMBER_NAME_CHECKED(UCASSettings, EnableFacebookAndroid)), GetDefaultConfigFilename());
				UpdateSinglePropertyInConfigFile(GetClass()->FindPropertyByName(GET_MEMBER_NAME_CHECKED(UCASSettings, EnableIronsourceAndroid)), GetDefaultConfigFilename());
				UpdateSinglePropertyInConfigFile(GetClass()->FindPropertyByName(GET_MEMBER_NAME_CHECKED(UCASSettings, EnableGoogleAdMobAndroid)), GetDefaultConfigFilename());
				UpdateSinglePropertyInConfigFile(GetClass()->FindPropertyByName(GET_MEMBER_NAME_CHECKED(UCASSettings, EnableAdColonyAndroid)), GetDefaultConfigFilename());
				UpdateSinglePropertyInConfigFile(GetClass()->FindPropertyByName(GET_MEMBER_NAME_CHECKED(UCASSettings, EnableMintegralAndroid)), GetDefaultConfigFilename());
				UpdateSinglePropertyInConfigFile(GetClass()->FindPropertyByName(GET_MEMBER_NAME_CHECKED(UCASSettings, EnableMyTargetAndroid)), GetDefaultConfigFilename());
				UpdateSinglePropertyInConfigFile(GetClass()->FindPropertyByName(GET_MEMBER_NAME_CHECKED(UCASSettings, EnableInMobiAndroid)), GetDefaultConfigFilename());
				UpdateSinglePropertyInConfigFile(GetClass()->FindPropertyByName(GET_MEMBER_NAME_CHECKED(UCASSettings, EnablePangleAndroid)), GetDefaultConfigFilename());
				UpdateSinglePropertyInConfigFile(GetClass()->FindPropertyByName(GET_MEMBER_NAME_CHECKED(UCASSettings, EnableTapjoyAndroid)), GetDefaultConfigFilename());
				UpdateSinglePropertyInConfigFile(GetClass()->FindPropertyByName(GET_MEMBER_NAME_CHECKED(UCASSettings, EnableUnityAdsAndroid)), GetDefaultConfigFilename());
				UpdateSinglePropertyInConfigFile(GetClass()->FindPropertyByName(GET_MEMBER_NAME_CHECKED(UCASSettings, EnableVungleAndroid)), GetDefaultConfigFilename());
				UpdateSinglePropertyInConfigFile(GetClass()->FindPropertyByName(GET_MEMBER_NAME_CHECKED(UCASSettings, EnableYandexAndroid)), GetDefaultConfigFilename());
			}
		}
		// IOS Ads Solution preset 
		if (PropertyChangedEvent.Property->GetFName() == GET_MEMBER_NAME_CHECKED(UCASSettings, AdsSolutionIOS))
		{
			switch (AdsSolutionIOS)
			{
				case ECASAdsSolution::Optimal:
				EnableAppLovinIOS = true;
				EnableFacebookIOS = true;
				EnableIronsourceIOS = true;
				EnableGoogleAdMobIOS = true;
				EnableAdColonyIOS = true;
				EnableMintegralIOS = true;
				EnableMyTargetIOS = true;
				EnableInMobiIOS = true;
				EnablePangleIOS = true;
				EnableTapjoyIOS = true;
				EnableUnityAdsIOS = true;
				EnableVungleIOS = true;
				EnableYandexIOS = false;
				break;
				case ECASAdsSolution::Families:
				EnableAppLovinIOS = true;
				EnableFacebookIOS = false;
				EnableIronsourceIOS = true;
				EnableGoogleAdMobIOS = true;
				EnableAdColonyIOS = true;
				EnableMintegralIOS = true;
				EnableMyTargetIOS = true;
				EnableInMobiIOS = true;
				EnablePangleIOS = true;
				EnableTapjoyIOS = true;
				EnableUnityAdsIOS = true;
				EnableVungleIOS = true;
				EnableYandexIOS = false;
				break;
				default: break;
			}

			if(AdsSolutionIOS != ECASAdsSolution::Custom)
			{
				UpdateSinglePropertyInConfigFile(GetClass()->FindPropertyByName(GET_MEMBER_NAME_CHECKED(UCASSettings, EnableAppLovinIOS)), GetDefaultConfigFilename());
				UpdateSinglePropertyInConfigFile(GetClass()->FindPropertyByName(GET_MEMBER_NAME_CHECKED(UCASSettings, EnableFacebookIOS)), GetDefaultConfigFilename());
				UpdateSinglePropertyInConfigFile(GetClass()->FindPropertyByName(GET_MEMBER_NAME_CHECKED(UCASSettings, EnableIronsourceIOS)), GetDefaultConfigFilename());
				UpdateSinglePropertyInConfigFile(GetClass()->FindPropertyByName(GET_MEMBER_NAME_CHECKED(UCASSettings, EnableGoogleAdMobIOS)), GetDefaultConfigFilename());
				UpdateSinglePropertyInConfigFile(GetClass()->FindPropertyByName(GET_MEMBER_NAME_CHECKED(UCASSettings, EnableAdColonyIOS)), GetDefaultConfigFilename());
				UpdateSinglePropertyInConfigFile(GetClass()->FindPropertyByName(GET_MEMBER_NAME_CHECKED(UCASSettings, EnableMintegralIOS)), GetDefaultConfigFilename());
				UpdateSinglePropertyInConfigFile(GetClass()->FindPropertyByName(GET_MEMBER_NAME_CHECKED(UCASSettings, EnableMyTargetIOS)), GetDefaultConfigFilename());
				UpdateSinglePropertyInConfigFile(GetClass()->FindPropertyByName(GET_MEMBER_NAME_CHECKED(UCASSettings, EnableInMobiIOS)), GetDefaultConfigFilename());
				UpdateSinglePropertyInConfigFile(GetClass()->FindPropertyByName(GET_MEMBER_NAME_CHECKED(UCASSettings, EnablePangleIOS)), GetDefaultConfigFilename());
				UpdateSinglePropertyInConfigFile(GetClass()->FindPropertyByName(GET_MEMBER_NAME_CHECKED(UCASSettings, EnableTapjoyIOS)), GetDefaultConfigFilename());
				UpdateSinglePropertyInConfigFile(GetClass()->FindPropertyByName(GET_MEMBER_NAME_CHECKED(UCASSettings, EnableUnityAdsIOS)), GetDefaultConfigFilename());
				UpdateSinglePropertyInConfigFile(GetClass()->FindPropertyByName(GET_MEMBER_NAME_CHECKED(UCASSettings, EnableVungleIOS)), GetDefaultConfigFilename());
				UpdateSinglePropertyInConfigFile(GetClass()->FindPropertyByName(GET_MEMBER_NAME_CHECKED(UCASSettings, EnableYandexIOS)), GetDefaultConfigFilename());
			}
		}

		Super::PostEditChangeProperty(PropertyChangedEvent);
	}
#endif
};

