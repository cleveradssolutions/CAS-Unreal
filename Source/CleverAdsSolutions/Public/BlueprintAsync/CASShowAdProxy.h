// Copyright CleverAdsSolutions LTD, CAS.AI. All Rights Reserved.

#pragma once

#include "CASDefines.h"
#include "CoreMinimal.h"
#include "UObject/ObjectMacros.h"
#include "Kismet/BlueprintAsyncActionBase.h"
#include "CASShowAdProxy.generated.h"

UCLASS()
class CLEVERADSSOLUTIONS_API UCASShowAdProxy : public UBlueprintAsyncActionBase {
    GENERATED_BODY()

   public:
    UPROPERTY(BlueprintAssignable, Category = "CAS Mobile Ads")
    FCASResultEvent OnAdDisplayed;
    UPROPERTY(BlueprintAssignable, Category = "CAS Mobile Ads")
    FCASResultEvent OnAdFailed;
    UPROPERTY(BlueprintAssignable, Category = "CAS Mobile Ads")
    FCASResultEvent OnAdClicked;
    UPROPERTY(BlueprintAssignable, Category = "CAS Mobile Ads")
    FCASResultEvent OnAdDismissed;

    UFUNCTION(BlueprintCallable,
              Category = "CAS Mobile Ads",
              meta = (BlueprintInternalUseOnly = "true", WorldContext = "WorldContextObject"))
    static UCASShowAdProxy* ShowInterstitialAd(UObject* WorldContextObject);

    UFUNCTION(BlueprintCallable,
              Category = "CAS Mobile Ads",
              meta = (BlueprintInternalUseOnly = "true", WorldContext = "WorldContextObject"))
    static UCASShowAdProxy* ShowAppOpenAd(UObject* WorldContextObject);

    UFUNCTION(BlueprintCallable,
              Category = "CAS Mobile Ads",
              meta = (BlueprintInternalUseOnly = "true", WorldContext = "WorldContextObject"))
    static UCASShowAdProxy* AutoshowInterstitialAd(UObject* WorldContextObject);

    UFUNCTION(BlueprintCallable,
              Category = "CAS Mobile Ads",
              meta = (BlueprintInternalUseOnly = "true", WorldContext = "WorldContextObject"))
    static UCASShowAdProxy* AutoshowAppOpenAd(UObject* WorldContextObject);

    UFUNCTION(BlueprintCallable,
              Category = "CAS Mobile Ads",
              meta = (BlueprintInternalUseOnly = "true",
                      WorldContext = "WorldContextObject",
                      DeprecatedFunction,
                      DeprecationMessage = "Use new AutoshowInterstitialAd() or AutoshowAppOpenAd()"))
    static UCASShowAdProxy* ShowAdOnReturnToApp(UObject* WorldContextObject);

    // UBlueprintAsyncActionBase interface
    virtual void Activate() override;
    // End of UBlueprintAsyncActionBase interface

   protected:
    UPROPERTY()
    int32 PrivateFlag;
    UFUNCTION()
    void HandleAdDisplayed();
    UFUNCTION()
    void HandleAdFailed(ECASError error);
    UFUNCTION()
    void HandleAdClicked();
    UFUNCTION()
    void HandleAdDismissed();

    UObject* WorldContextObject;
};