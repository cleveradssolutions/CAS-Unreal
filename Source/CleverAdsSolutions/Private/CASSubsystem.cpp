// Fill out your copyright notice in the Description page of Project Settings.

#include "CASSubsystem.h"

#include "CASSettings.h"
#include "Engine/World.h"

#include "CASDefines.h"

#include "CASInterface_Interstitial.h"
#include "CASInterface_Rewarded.h"
#include "CASInterface_Banner.h"
#include "CASInterface_General.h"

#if PLATFORM_ANDROID
#include "Android/CASInterface_General_Android.h"
#include "Android/CASInterface_Interstitial_Android.h"
#include "Android/CASInterface_Rewarded_Android.h"
#include "Android/CASInterface_Banner_Android.h"
#endif

#if PLATFORM_IOS
#include "IOS/CASInterface_General_IOS.h"
#include "IOS/CASInterface_Interstitial_IOS.h"
#include "IOS/CASInterface_Rewarded_IOS.h"
#include "IOS/CASInterface_Banner_IOS.h"
#endif

void UCAS::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	DisableDevModesIfShipping();

	// General
#if PLATFORM_ANDROID
	GeneralInterface = NewObject<UCASInterface_General_Android>(this);
#elif PLATFORM_IOS
	GeneralInterface = NewObject<UCASInterface_General_IOS>(this);
#else
	GeneralInterface = NewObject<UCASInterface_General>(this);
#endif
}

void UCAS::Init()
{
	if(Initialized)
	{
		UE_LOG(LogCAS, Warning, TEXT("Clever Ads Solutions system already initialized!"));
		return;
	}
	
	if(GeneralInterface) GeneralInterface->Init();
	
	// Interstitial
	#if PLATFORM_ANDROID
	InterstitialInterface = NewObject<UCASInterface_Interstitial_Android>(this);
#elif PLATFORM_IOS
	InterstitialInterface = NewObject<UCASInterface_Interstitial_IOS>(this);
#else
	InterstitialInterface = NewObject<UCASInterface_Interstitial>(this);
#endif
	
	if(InterstitialInterface) InterstitialInterface->Init();

	// Rewarded
	#if PLATFORM_ANDROID
	RewardedInterface = NewObject<UCASInterface_Rewarded_Android>(this);
#elif PLATFORM_IOS
	RewardedInterface = NewObject<UCASInterface_Rewarded_IOS>(this);
#else
	RewardedInterface = NewObject<UCASInterface_Rewarded>(this);
#endif
	
	if(RewardedInterface) RewardedInterface->Init();

	// Banner
	#if PLATFORM_ANDROID
	BannerInterface = NewObject<UCASInterface_Banner_Android>(this);
#elif PLATFORM_IOS
	BannerInterface = NewObject<UCASInterface_Banner_IOS>(this);
#else
	BannerInterface = NewObject<UCASInterface_Banner>(this);
#endif
	
	if(BannerInterface) BannerInterface->Init();

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

void UCAS::DisableDevModesIfShipping()
{
#if UE_BUILD_SHIPPING
	UCASSettings* CASSettings = GetMutableDefault<UCASSettings>();
	
	if(CASSettings->DisableDevModesInShipping)
	{
		CASSettings->DebugMode = false;
		CASSettings->TestAds = false;
	}
#endif
}
