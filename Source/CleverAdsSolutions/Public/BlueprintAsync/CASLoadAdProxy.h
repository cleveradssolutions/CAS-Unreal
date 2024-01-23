// Copyright CleverAdsSolutions LTD, CAS.AI. All Rights Reserved.

#pragma once

#include "CASDefines.h"
#include "CoreMinimal.h"
#include "UObject/ObjectMacros.h"
#include "Kismet/BlueprintAsyncActionBase.h"
#include "CASLoadAdProxy.generated.h"

UCLASS()
class CLEVERADSSOLUTIONS_API UCASLoadAdProxy : public UBlueprintAsyncActionBase {
    GENERATED_BODY()

   public:
    UPROPERTY(BlueprintAssignable, Category = "CAS Mobile Ads")
    FCASResultEvent OnAdLoaded;
    UPROPERTY(BlueprintAssignable, Category = "CAS Mobile Ads")
    FCASResultEvent OnAdFailed;

    UFUNCTION(BlueprintCallable,
              Category = "CAS Mobile Ads",
              meta = (BlueprintInternalUseOnly = "true", WorldContext = "WorldContextObject"))
    static UCASLoadAdProxy* LoadInterstitialAd(UObject* WorldContextObject, bool EventOnce = true);

    UFUNCTION(BlueprintCallable,
              Category = "CAS Mobile Ads",
              meta = (BlueprintInternalUseOnly = "true", WorldContext = "WorldContextObject"))
    static UCASLoadAdProxy* LoadRewardedAd(UObject* WorldContextObject, bool EventOnce = true);

    // UBlueprintAsyncActionBase interface
    virtual void Activate() override;
    // End of UBlueprintAsyncActionBase interface

   protected:
    UPROPERTY()
    int32 PrivateFlag;
    UPROPERTY()
    bool IsOnceEvent;
    UFUNCTION()
    void HandleAdLoaded();
    UFUNCTION()
    void HandleAdFailed(ECASError error);

    UObject* WorldContextObject;
};