// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CASInterface.h"
#include "CASInterface_ScreenAd.generated.h"

/** Abstract CAS interface class for full screen ads, such as Interstitial and Rewarded. */
UCLASS()
class CLEVERADSSOLUTIONS_API UCASInterface_ScreenAd : public UCASInterface
{
	GENERATED_BODY()

public: // Delegates

	/** Called when ad gets shown to a user */
	UPROPERTY(BlueprintAssignable)
	FCASEvent OnShown;

	/** Called when ad gets loaded and became ready to show. Use IsReady() to make additional check */
	UPROPERTY(BlueprintAssignable)
	FCASEvent OnLoaded;

	/** Called when ad gets closed by a user */
	UPROPERTY(BlueprintAssignable)
	FCASEvent OnClosed;

	/** Called when user clicks on ad. */
	UPROPERTY(BlueprintAssignable)
	FCASEvent OnClicked;

	/** Called when ad completes to show */
	UPROPERTY(BlueprintAssignable)
	FCASEvent OnCompleted;

	/** Called when system fails to show an ad.
	 *@param ErrorMessage - String with an error message of what happened wrong
	 */
	UPROPERTY(BlueprintAssignable)
	FCASErrorEvent OnShowError;

	/** Called when system fails to load an ad.
	 *@param ErrorMessage - String with an error message of what happened wrong
	 */
	UPROPERTY(BlueprintAssignable)
	FCASErrorEvent OnLoadError;

public: // Functions

	/** Shows ad. Will do nothing if ad is not loaded. Use IsReady() to check if it is ready. */
	UFUNCTION(BlueprintCallable, Category="CleverAdsSolutions")
	virtual void Show(){}

	/** Checks if ad is loaded and ready to show
	 * @return True if ad is loaded and is ready
	 */
	UFUNCTION(BlueprintPure, Category="CleverAdsSolutions")
	virtual bool IsReady(){return false;}

	/** Manually loads a new ad. By default new ads load automatically. */
	UFUNCTION(BlueprintCallable, Category="CleverAdsSolutions")
	virtual void Load(){}
};
