// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CASInterface.h"
#include "CASInterface_ReturnAds.generated.h"

/** Abstract CAS interface class for app return ads. */
UCLASS()
class CLEVERADSSOLUTIONS_API UCASInterface_ReturnAds : public UCASInterface
{
	GENERATED_BODY()

public: // Delegates

	/** Called when ad gets shown to a user */
	UPROPERTY(BlueprintAssignable)
	FCASImpressionEvent OnShown;

	/** Called when ad gets closed by a user */
	UPROPERTY(BlueprintAssignable)
	FCASEvent OnClosed;

	/** Called when user clicks on ad. */
	UPROPERTY(BlueprintAssignable)
	FCASEvent OnClicked;

	/** Called when ad completes to show */
	UPROPERTY(BlueprintAssignable)
	FCASEvent OnCompleted;

	/** Called when ad failed to show */
	UPROPERTY(BlueprintAssignable)
	FCASErrorEvent OnShowError;

public: // Functions
	
	/** Call this method when you do not want users to see ads when they return to your application */
	UFUNCTION(BlueprintCallable, Category="CleverAdsSolutions|ReturnAds")
	virtual void SkipNextAppReturnAds(){}

	/** Disables return ads. Call when you no longer want to show ads to users after they return to the app */
	UFUNCTION(BlueprintCallable, Category="CleverAdsSolutions|ReturnAds")
	virtual void DisableAppReturnAds(){}


};
