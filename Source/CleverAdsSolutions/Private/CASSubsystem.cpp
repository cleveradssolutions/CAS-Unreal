// Fill out your copyright notice in the Description page of Project Settings.

#include "CASSubsystem.h"

#include "CASInterface_Interstitial_Android.h"

void UCAS::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

#if PLATFORM_ANDROID
	InterstitialInterface = NewObject<UCASInterface_Interstitial_Android>(this);
#else
	InterstitialInterface = NewObject<UCASInterface_Interstitial>(this);
#endif
	
	if(InterstitialInterface) InterstitialInterface->Init();
}
