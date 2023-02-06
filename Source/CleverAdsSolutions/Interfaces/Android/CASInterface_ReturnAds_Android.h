// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CASInterface_ReturnAds.h"
#include "CASInterface_ReturnAds_Android.generated.h"

/** CAS Interstitial interface class for Android. */
UCLASS()
class CLEVERADSSOLUTIONS_API UCASInterface_ReturnAds_Android : public UCASInterface_ReturnAds
{
	GENERATED_BODY()

#if PLATFORM_ANDROID
	
	virtual void Init() override;

	virtual void SkipNextAppReturnAds() override;
	
	virtual void DisableAppReturnAds() override;

#endif
};
