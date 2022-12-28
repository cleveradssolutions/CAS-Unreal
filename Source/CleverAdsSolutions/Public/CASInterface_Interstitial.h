// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CASInterface.h"
#include "CASInterface_Interstitial.generated.h"

/** Abstract CAS interface class for Interstitials.
 * Platform implementation inside derived classes
 */
UCLASS()
class CLEVERADSSOLUTIONS_API UCASInterface_Interstitial : public UCASInterface
{
	GENERATED_BODY()

public: // Delegates

	/** Called when Interstitial ad gets shown to a user */
	UPROPERTY(BlueprintAssignable)
	FCASEvent OnInterstitialShown;

	/** Called when Interstitial ad gets loaded and became ready to show.
	 * Use IsInterstitialReady() to make additional checks
	 */
	UPROPERTY(BlueprintAssignable)
	FCASEvent OnInterstitialLoaded;

	/** Called when Interstitial ad gets closed by a user */
	UPROPERTY(BlueprintAssignable)
	FCASEvent OnInterstitialClosed;

	/** Called when user clicks on Interstitial ad. */
	UPROPERTY(BlueprintAssignable)
	FCASEvent OnInterstitialClicked;

	/** Called when Interstitial ad completes to show */
	UPROPERTY(BlueprintAssignable)
	FCASEvent OnInterstitialCompleted;

	/** Called when system fails to show an Interstitial ad.
	 *@param ErrorMessage - String with an error message of what happened wrong
	 */
	UPROPERTY(BlueprintAssignable)
	FCASErrorEvent OnInterstitialShowError;

	/** Called when system fails to load an Interstitial ad.
	 *@param ErrorMessage - String with an error message of what happened wrong
	 */
	UPROPERTY(BlueprintAssignable)
	FCASErrorEvent OnInterstitialLoadError;

public: // Functions

	/** Shows interstitial ad.
	 * Will do nothing if ad is not loaded. Use IsInterstitialReady() to check if it is ready.
	 */
	UFUNCTION(BlueprintCallable, Category="CleverAdsSolutions")
	virtual void Show(){}

	/** Shows interstitial ad.
	 * @return True if interstitial is ready
	 */
	UFUNCTION(BlueprintPure, Category="CleverAdsSolutions")
	virtual bool IsReady(){return false;}
};
