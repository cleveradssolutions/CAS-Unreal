// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CASInterface.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "CASSubsystem.generated.h"

class UCASSettingsAndroid;
class UCASSettingsIOS;
class UCASInterface_AudienceNetwork;
class UCASInterface_ReturnAds;
class UCASInterface_General;
class UCASInterface_Banner;
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

	UPROPERTY()
	UCASInterface_Banner* BannerInterface;

	UPROPERTY()
	UCASInterface_General* GeneralInterface;
	
	UPROPERTY()
	UCASInterface_ReturnAds* ReturnAdsInterface;

	UPROPERTY()
	UCASInterface_AudienceNetwork* AudienceNetworkInterface;

	UPROPERTY()
	UCASSettingsIOS* CASSettingsIOS;
	
	UPROPERTY()
	UCASSettingsAndroid* CASSettingsAndroid;

	bool Initialized;

public:

	UPROPERTY(BlueprintAssignable)
	FCASEvent OnInitialized;
	
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

	/** Initialize Clever Ads Solutions and its interfaces */
	UFUNCTION(BlueprintCallable, Category="CleverAdsSolutions")
	void Init();

	static UCAS* Get(UObject* WorldContext);

	/** Returns General interface.
	 * Returns base class without implementation if CAS ads not supported on this platform
	 * @return CAS General Interface object
	 */
	UFUNCTION(BlueprintPure, Category="CleverAdsSolutions", meta=(CompactNodeTitle="General"))
	UCASInterface_General* GetGeneralInterface() const {return GeneralInterface;}

	/** Returns Interstitial interface.
	 * Returns base class without implementation if CAS or Interstitial ads not supported on this platform
	 * @return CAS Interstitial Interface object
	 */
	UFUNCTION(BlueprintPure, Category="CleverAdsSolutions", meta=(CompactNodeTitle="Interstitial"))
	UCASInterface_Interstitial* GetInterstitialInterface() const;

	/** Returns Rewarded interface.
	 * Returns base class without implementation if CAS or Rewarded ads not supported on this platform
	 * @return CAS Interstitial Interface object
	 */
	UFUNCTION(BlueprintPure, Category="CleverAdsSolutions", meta=(CompactNodeTitle="Rewarded"))
	UCASInterface_Rewarded* GetRewardedInterface() const;

	/** Returns Banner interface.
	 * Returns base class without implementation if CAS or Banner ads not supported on this platform
	 * @return CAS Interstitial Interface object
	 */
	UFUNCTION(BlueprintPure, Category="CleverAdsSolutions", meta=(CompactNodeTitle="Banner"))
	UCASInterface_Banner* GetBannerInterface() const;

	/** Returns ReturnAds interface.
	 * Returns base class without implementation if CAS or ReturnAds ads not supported on this platform
	 * @return CAS ReturnAds Interface object
	 */
	UFUNCTION(BlueprintPure, Category="CleverAdsSolutions", meta=(CompactNodeTitle="ReturnAds"))
	UCASInterface_ReturnAds* GetReturnAdsInterface() const;

	/** Returns AudienceNetwork interface.
	 * Returns base class without implementation if CAS or AudienceNetwork not supported or disabled on this platform
	 * @return CAS AudienceNetwork Interface object
	 */
	UFUNCTION(BlueprintPure, Category="CleverAdsSolutions", meta=(CompactNodeTitle="AudienceNetwork"))
	UCASInterface_AudienceNetwork* GetAudienceNetworkInterface() const;

	/** Returns whenever CAS was initialized or not */
	UFUNCTION(BlueprintPure, Category="CleverAdsSolutions")
	bool IsInitialized() const {return Initialized;}

private:

	void DisableDevModesIfShipping();
	
};
