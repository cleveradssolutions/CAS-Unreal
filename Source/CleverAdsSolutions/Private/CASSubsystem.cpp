// Fill out your copyright notice in the Description page of Project Settings.

#include "CASSubsystem.h"

#include "CASInterface_Interstitial.h"
#include "CASInterface_Rewarded.h"
#include "CASInterface_Banner.h"

#if PLATFORM_ANDROID
#include "CASInterface_Interstitial_Android.h"
#include "CASInterface_Rewarded_Android.h"
#include "CASInterface_Banner_Android.h"
#endif

void UCAS::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	InitCASInterfaces();
}

void UCAS::InitCASInterfaces()
{
	// Interstitial
	#if PLATFORM_ANDROID
	InterstitialInterface = NewObject<UCASInterface_Interstitial_Android>(this);
#else
	InterstitialInterface = NewObject<UCASInterface_Interstitial>(this);
#endif
	
	if(InterstitialInterface) InterstitialInterface->Init();

	// Rewarded
	#if PLATFORM_ANDROID
	RewardedInterface = NewObject<UCASInterface_Rewarded_Android>(this);
#else
	RewardedInterface = NewObject<UCASInterface_Rewarded>(this);
#endif
	
	if(RewardedInterface) RewardedInterface->Init();

	// Banner
	#if PLATFORM_ANDROID
	BannerInterface = NewObject<UCASInterface_Banner_Android>(this);
#else
	BannerInterface = NewObject<UCASInterface_Banner>(this);
#endif
	
	if(BannerInterface) BannerInterface->Init();
}
