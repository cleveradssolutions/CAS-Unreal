// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CASInterface_ScreenAd.h"
#include "CASInterface_Interstitial.generated.h"

/** Abstract CAS interface class for Interstitials.
 * Platform implementation inside derived classes
 */
UCLASS()
class CLEVERADSSOLUTIONS_API UCASInterface_Interstitial : public UCASInterface_ScreenAd
{
	GENERATED_BODY()

	/** Limit the posting of an interstitial ad to a period of time in seconds after the ad is closed, during which display attempts will fail.
	 * Unlimited by default 0 seconds..
	 * @param Interval Interval in seconds
	 */
	UFUNCTION(BlueprintCallable, Category="CleverAdsSolutions|Interstitial")
	virtual void SetMinimumInterval(int Interval){}
	
	/** Restarts the interval to enable Interstitial ads again. */
	UFUNCTION(BlueprintCallable, Category="CleverAdsSolutions|Interstitial")
	virtual void RestartInterval(){}
};
