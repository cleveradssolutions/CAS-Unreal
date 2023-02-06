// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CASInterface.h"
#include "CASInterface_Banner.generated.h"

/** Abstract CAS interface class for Banner ads.
 */
UCLASS()
class CLEVERADSSOLUTIONS_API UCASInterface_Banner : public UCASInterface
{
	GENERATED_BODY()

public: // Delegates

	/** Called when Banner ad gets created and shown to a user */
	UPROPERTY(BlueprintAssignable)
	FCASImpressionEvent OnShow;

	/** Called when Banner ad gets loaded and became ready to show. Use IsReady() to make additional checks */
	UPROPERTY(BlueprintAssignable)
	FCASEvent OnLoaded;

	/** Called when Banner ad gets destroyed */
	UPROPERTY(BlueprintAssignable)
	FCASEvent OnDestroyed;

	/** Called when user clicks on Banner ad. */
	UPROPERTY(BlueprintAssignable)
	FCASEvent OnClicked;

	/** Called when Banner ad completes to show */
	UPROPERTY(BlueprintAssignable)
	FCASEvent OnCompleted;

	/** Called when system fails to show an Banner ad.
	 *@param ErrorMessage - String with an error message of what happened wrong
	 */
	UPROPERTY(BlueprintAssignable)
	FCASErrorEvent OnFail;

public: // Functions

	/** Shows ad.
	 * Will do nothing if ad is not loaded. Use IsReady() to check if it is ready.
	 */
	UFUNCTION(BlueprintCallable, Category="CleverAdsSolutions|Banner")
	virtual void CreateBanner(){}

	/** Loads next banner and discards current one.
	 */
	UFUNCTION(BlueprintCallable, Category="CleverAdsSolutions|Banner")
	virtual void LoadNextBanner(){}

	/** Destroys banner completely. If you want to hide it, use ToggleBannerVisibility. */
	UFUNCTION(BlueprintCallable, Category="CleverAdsSolutions|Banner")
	virtual void DestroyBanner(){}

	/** Toggles Banner visibility
	 * @param Visible - Reveal or hide the Banner
	 */
	UFUNCTION(BlueprintCallable, Category="CleverAdsSolutions|Banner")
	virtual void ToggleBannerVisibility(bool Visible){}

	/** Checks if Banner ad is visible or not. Use ToggleBannerVisibility to toggle visibility.
	 * @return True if Banner ad is visible
	 */
	UFUNCTION(BlueprintPure, Category="CleverAdsSolutions|Banner")
	virtual bool IsVisible() const {return false;}

	/** Checks if Banner ad is ready.
	 * @return True if Banner ad is ready
	 */
	UFUNCTION(BlueprintPure, Category="CleverAdsSolutions|Banner")
	virtual bool IsReady() const {return false;}

	/** Change the banner automatic refresh rate. */
	UFUNCTION(BlueprintCallable, Category="CleverAdsSolutions|Banner")
	virtual void SetRefreshInterval(int Interval){}

	/** Disables the banner automatic refresh rate. */
	UFUNCTION(BlueprintCallable, Category="CleverAdsSolutions|Banner")
	virtual void DisableRefreshInterval(){}
};
