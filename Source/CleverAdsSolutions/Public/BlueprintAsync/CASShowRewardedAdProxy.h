// Copyright CleverAdsSolutions LTD, CAS.AI. All Rights Reserved.

#pragma once

#include "CASDefines.h"
#include "CASShowAdProxy.h"
#include "CoreMinimal.h"
#include "UObject/ObjectMacros.h"
#include "Kismet/BlueprintAsyncActionBase.h"
#include "CASShowRewardedAdProxy.generated.h"

UCLASS()
class CLEVERADSSOLUTIONS_API UCASShowRewardedAdProxy : public UCASShowAdProxy {
    GENERATED_BODY()

   public:
    UPROPERTY(BlueprintAssignable, Category = "CAS Mobile Ads")
    FCASResultEvent OnAdEarnedReward;

    // Shows the login UI for the currently active online subsystem, if the subsystem supports a login UI.
    UFUNCTION(BlueprintCallable, Category = "CAS Mobile Ads", meta = (BlueprintInternalUseOnly = "true", WorldContext = "WorldContextObject"))
    static UCASShowRewardedAdProxy* ShowRewardedAd(UObject* WorldContextObject);

    // UBlueprintAsyncActionBase interface
    virtual void Activate() override;
    // End of UBlueprintAsyncActionBase interface

   protected:
    UFUNCTION()
    void HandleAdEarnReward();

    UObject* WorldContextObject;
};