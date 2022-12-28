// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Subsystems/GameInstanceSubsystem.h"
#include "CASSubsystem.generated.h"

class UCASInterface_Rewarded;
class UCASInterface_Interstitial;

/** Clever Ads Solutions subsystem. Used to get access to ads interface objects and general functionality
 */
UCLASS()
class CLEVERADSSOLUTIONS_API UCAS : public UGameInstanceSubsystem
{
	GENERATED_BODY()

private:

	UPROPERTY()
	UCASInterface_Interstitial* InterstitialInterface;
	
	UPROPERTY()
	UCASInterface_Rewarded* RewardedInterface;

public:
	
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

	/** Returns Interstitial interface.
	 * Returns base class without implementation if CAS or Interstitial ads not supported on this platform
	 * @return CAS Interstitial Interface object
	 */
	UFUNCTION(BlueprintPure, Category="CleverAdsSolutions", meta=(CompactNodeTitle="Interstitial"))
	UCASInterface_Interstitial* GetInterstitialInterface() const {return InterstitialInterface;}

	/** Returns Rewarded interface.
	 * Returns base class without implementation if CAS or Rewarded ads not supported on this platform
	 * @return CAS Interstitial Interface object
	 */
	UFUNCTION(BlueprintPure, Category="CleverAdsSolutions", meta=(CompactNodeTitle="Rewarded"))
	UCASInterface_Rewarded* GetRewardedInterface() const {return RewardedInterface;}
};
