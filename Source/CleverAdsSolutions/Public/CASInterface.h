// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "CASInterface.generated.h"

UCLASS(BlueprintType, Abstract)
class CLEVERADSSOLUTIONS_API UCASInterface : public UObject
{
	GENERATED_BODY()

public:

	/** 
	 * Returns platform specific CAS interface. Returns nullptr if CAS is not supported on this platform
	 */
	UFUNCTION(BlueprintPure, Category="CleverAdsSolutions", meta=(CompactNodeTitle="CAS"))
	static UCASInterface* GetCAS();

	/**
	 * Shows interstitial ad.
	 * Will do nothing if ad is not loaded.
	 */
	UFUNCTION(BlueprintCallable, Category="CleverAdsSolutions")
	virtual void ShowInterstitial(){};

	/**
	 * Shows rewarded ad.
	 * Will do nothing if ad is not loaded.
	 */
	UFUNCTION(BlueprintCallable, Category="CleverAdsSolutions")
	virtual void ShowRewarded(){};
	
};
