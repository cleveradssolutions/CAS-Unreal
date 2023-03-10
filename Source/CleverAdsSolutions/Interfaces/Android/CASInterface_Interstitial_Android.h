// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CASInterface_Interstitial.h"
#include "CASInterface_Interstitial_Android.generated.h"

/** CAS Interstitial interface class for Android. */
UCLASS()
class CLEVERADSSOLUTIONS_API UCASInterface_Interstitial_Android : public UCASInterface_Interstitial
{
	GENERATED_BODY()

#if PLATFORM_ANDROID
	
	virtual void PreInit();
	
	virtual void Show() override;
	
	virtual bool IsReady() override;

	virtual void Load() override;

	virtual void SetMinimumInterval(int Interval) override;
	
	virtual void RestartInterval() override;

#endif
};
