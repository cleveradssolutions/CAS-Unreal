// Fill out your copyright notice in the Description page of Project Settings.

#include "CASSubsystem.h"

#include "CASSettings.h"
#include "Engine/World.h"

#include "CASDefines.h"

#include "CASInterface_Interstitial.h"
#include "CASInterface_Rewarded.h"
#include "CASInterface_Banner.h"
#include "CASInterface_General.h"
#include "CASInterface_ReturnAds.h"
#include "CASInterface_AudienceNetwork.h"

#if PLATFORM_ANDROID
#include "Android/CASInterface_General_Android.h"
#include "Android/CASInterface_Interstitial_Android.h"
#include "Android/CASInterface_Rewarded_Android.h"
#include "Android/CASInterface_Banner_Android.h"
#include "Android/CASInterface_ReturnAds_Android.h"

#if WITH_CAS_FACEBOOK
#include "Android/CASInterface_AudienceNetwork_Android.h"
#endif

#endif

#if PLATFORM_IOS
#include "IOS/CASInterface_General_IOS.h"
#include "IOS/CASInterface_Interstitial_IOS.h"
#include "IOS/CASInterface_Rewarded_IOS.h"
#include "IOS/CASInterface_Banner_IOS.h"
#include "IOS/CASInterface_ReturnAds_IOS.h"

#if WITH_CAS_FACEBOOK
#include "IOS/CASInterface_AudienceNetwork_IOS.h"
#endif

#endif

void UCAS::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	CASSettingsIOS = GetMutableDefault<UCASSettingsIOS>();
	CASSettingsAndroid = GetMutableDefault<UCASSettingsAndroid>();

	DisableDevModesIfShipping();

	// General
#if PLATFORM_ANDROID
	GeneralInterface = NewObject<UCASInterface_General_Android>(this);
#elif PLATFORM_IOS
	GeneralInterface = NewObject<UCASInterface_General_IOS>(this);
#else
	GeneralInterface = NewObject<UCASInterface_General>(this);
#endif

	GeneralInterface->OnInitialized.BindLambda([&]
	{
		OnInitialized.Broadcast();
	});
}

void UCAS::Init()
{
	if(Initialized)
	{
		UE_LOG(LogCAS, Warning, TEXT("Clever Ads Solutions system already initialized!"));
		return;
	}
	
	// Interstitial
	#if PLATFORM_ANDROID
	InterstitialInterface = NewObject<UCASInterface_Interstitial_Android>(this);
#elif PLATFORM_IOS
	InterstitialInterface = NewObject<UCASInterface_Interstitial_IOS>(this);
#else
	InterstitialInterface = NewObject<UCASInterface_Interstitial>(this);
#endif

	// Rewarded
	#if PLATFORM_ANDROID
	RewardedInterface = NewObject<UCASInterface_Rewarded_Android>(this);
#elif PLATFORM_IOS
	RewardedInterface = NewObject<UCASInterface_Rewarded_IOS>(this);
#else
	RewardedInterface = NewObject<UCASInterface_Rewarded>(this);
#endif
	
	// Banner
	#if PLATFORM_ANDROID
	BannerInterface = NewObject<UCASInterface_Banner_Android>(this);
#elif PLATFORM_IOS
	BannerInterface = NewObject<UCASInterface_Banner_IOS>(this);
#else
	BannerInterface = NewObject<UCASInterface_Banner>(this);
#endif
	
	// ReturnAds
	#if PLATFORM_ANDROID
	ReturnAdsInterface = NewObject<UCASInterface_ReturnAds_Android>(this);
#elif PLATFORM_IOS
	ReturnAdsInterface = NewObject<UCASInterface_ReturnAds_IOS>(this);
#else
	ReturnAdsInterface = NewObject<UCASInterface_ReturnAds>(this);
#endif

	// AudienceNetwork
#if PLATFORM_ANDROID && WITH_CAS_FACEBOOK
	AudienceNetworkInterface = NewObject<UCASInterface_AudienceNetwork_Android>(this);
#elif PLATFORM_IOS && WITH_CAS_FACEBOOK
	AudienceNetworkInterface = NewObject<UCASInterface_AudienceNetwork_IOS>(this);
#else
	AudienceNetworkInterface = NewObject<UCASInterface_AudienceNetwork>(this);
#endif
	
	if(GeneralInterface) GeneralInterface->PreInit();
	if(InterstitialInterface) InterstitialInterface->PreInit();
	if(RewardedInterface) RewardedInterface->PreInit();
	if(BannerInterface) BannerInterface->PreInit();
	if(ReturnAdsInterface) ReturnAdsInterface->PreInit();
	
	if(GeneralInterface) GeneralInterface->Init();
	if(InterstitialInterface) InterstitialInterface->Init();
	if(RewardedInterface) RewardedInterface->Init();
	if(BannerInterface) BannerInterface->Init();
	if(ReturnAdsInterface) ReturnAdsInterface->Init();

	Initialized = true;
}

UCAS* UCAS::Get(UObject* WorldContext)
{
	if(!WorldContext) return nullptr;

	if(const UCASInterface* CASInterface = Cast<UCASInterface>(WorldContext))
	{
		return Cast<UCAS>(CASInterface->GetOuter());
	}

	if(!WorldContext->GetWorld() || !WorldContext->GetWorld()->GetGameInstance()) return nullptr;

	return WorldContext->GetWorld()->GetGameInstance()->GetSubsystem<UCAS>();
}

UCASInterface_Interstitial* UCAS::GetInterstitialInterface() const
{
	checkf(Initialized, TEXT("Trying to get CAS Interstitial interface before initializing. Call 'CAS > Init' before using ads!"));

	return InterstitialInterface;
}

UCASInterface_Rewarded* UCAS::GetRewardedInterface() const
{
	checkf(Initialized, TEXT("Trying to get CAS Rewarded interface before initializing. Call 'CAS > Init' before using ads!"));

	return RewardedInterface;
}

UCASInterface_Banner* UCAS::GetBannerInterface() const
{
	checkf(Initialized, TEXT("Trying to get CAS Banner interface before initializing. Call 'CAS > Init' before using ads!"));

	return BannerInterface;
}

UCASInterface_ReturnAds* UCAS::GetReturnAdsInterface() const
{
	checkf(Initialized, TEXT("Trying to get CAS ReturnAds interface before initializing. Call 'CAS > Init' before using ads!"));

	return ReturnAdsInterface;
}

UCASInterface_AudienceNetwork* UCAS::GetAudienceNetworkInterface() const
{
	checkf(Initialized, TEXT("Trying to get CAS AudienceNetwork interface before initializing. Call 'CAS > Init' before using ads!"));

	return AudienceNetworkInterface;
}

void UCAS::DisableDevModesIfShipping()
{
#if UE_BUILD_SHIPPING
	{
		if(CASSettingsIOS->DisableDevModesInShipping)
		{
			CASSettings->DebugMode = false;
			CASSettings->TestAds = false;
		}
	}
	{
		if(CASSettingsAndroid->DisableDevModesInShipping)
		{
			CASSettings->DebugMode = false;
			CASSettings->TestAds = false;
		}
	}
#endif
}
