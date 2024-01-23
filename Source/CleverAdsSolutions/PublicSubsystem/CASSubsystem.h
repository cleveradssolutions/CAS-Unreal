// Copyright CleverAdsSolutions LTD, CAS.AI. All Rights Reserved.

#pragma once

#include "CASInterface.h"
#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "CASSubsystem.generated.h"

class UCASInterface_ReturnAds;
class UCASInterface_General;
class UCASInterface_Banner;
class UCASInterface_Rewarded;
class UCASInterface_Interstitial;

UCLASS()
class CLEVERADSSOLUTIONS_API UCAS : public UGameInstanceSubsystem {
    GENERATED_BODY()

   public:
    UPROPERTY(BlueprintAssignable)
    FCASEvent OnInitialized;

    virtual void Initialize(FSubsystemCollectionBase& Collection) override;

    /** Returns whenever CAS was initialized or not */
    UFUNCTION(BlueprintPure, Category = "CleverAdsSolutions")
    bool IsInitialized() const;

    /** Initialize Clever Ads Solutions and its interfaces */
    UFUNCTION(BlueprintCallable, Category = "CleverAdsSolutions")
    void Init();

    static UCAS* Get(UObject* WorldContext);

    /** Returns General interface.
     * Returns base class without implementation if CAS ads not supported on this platform
     * @return CAS General Interface object
     */
    UFUNCTION(
        BlueprintPure,
        Category = "CleverAdsSolutions",
        meta = (CompactNodeTitle = "General",
                DeprecatedFunction,
                DeprecationMessage =
                    "Please migrate to using the new UCASMobileAds API. Also try new Blueprint Async Action nodes."))
    UCASInterface_General* GetGeneralInterface() const;

    /** Returns Interstitial interface.
     * Returns base class without implementation if CAS or Interstitial ads not supported on this platform
     * @return CAS Interstitial Interface object
     */
    UFUNCTION(
        BlueprintPure,
        Category = "CleverAdsSolutions",
        meta = (CompactNodeTitle = "Interstitial",
                DeprecatedFunction,
                DeprecationMessage =
                    "Please migrate to using the new UCASMobileAds API. Also try new Blueprint Async Action nodes."))
    UCASInterface_Interstitial* GetInterstitialInterface();

    /** Returns Rewarded interface.
     * Returns base class without implementation if CAS or Rewarded ads not supported on this platform
     * @return CAS Interstitial Interface object
     */
    UFUNCTION(
        BlueprintPure,
        Category = "CleverAdsSolutions",
        meta = (CompactNodeTitle = "Rewarded",
                DeprecatedFunction,
                DeprecationMessage =
                    "Please migrate to using the new UCASMobileAds API. Also try new Blueprint Async Action nodes."))
    UCASInterface_Rewarded* GetRewardedInterface();

    /** Returns Banner interface.
     * Returns base class without implementation if CAS or Banner ads not supported on this platform
     * @return CAS Interstitial Interface object
     */
    UFUNCTION(
        BlueprintPure,
        Category = "CleverAdsSolutions",
        meta = (CompactNodeTitle = "Banner",
                DeprecatedFunction,
                DeprecationMessage =
                    "Please migrate to using the new UCASMobileAds API. Also try new Blueprint Async Action nodes."))
    UCASInterface_Banner* GetBannerInterface();

    /** Returns IncludeReturnAds interface.
     * Returns base class without implementation if CAS or IncludeReturnAds ads not supported on this platform
     * @return CAS IncludeReturnAds Interface object
     */
    UFUNCTION(
        BlueprintPure,
        Category = "CleverAdsSolutions",
        meta = (CompactNodeTitle = "Return To App Ad",
                DeprecatedFunction,
                DeprecationMessage =
                    "Please migrate to using the new UCASMobileAds API. Also try new Blueprint Async Action nodes."))
    UCASInterface_ReturnAds* GetReturnAdsInterface();

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

    UFUNCTION()
    void HandleInitialized(const FCASInitialConfig& Config);
};
