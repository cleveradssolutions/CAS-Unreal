// Copyright CleverAdsSolutions LTD, CAS.AI. All Rights Reserved.

#pragma once

#include "CASDefines.h"
#include "CASLoadAdProxy.h"
#include "CoreMinimal.h"
#include "UObject/ObjectMacros.h"
#include "Kismet/BlueprintAsyncActionBase.h"
#include "CASLoadBannerAdProxy.generated.h"

UCLASS()
class CLEVERADSSOLUTIONS_API UCASLoadBannerAdProxy : public UCASLoadAdProxy {
    GENERATED_BODY()

   public:
    UPROPERTY(BlueprintAssignable, Category = "CAS Mobile Ads")
    FCASResultEvent OnAdClicked;

    UFUNCTION(BlueprintCallable,
              Category = "CAS Mobile Ads",
              meta = (BlueprintInternalUseOnly = "true", WorldContext = "WorldContextObject"))
    static UCASLoadBannerAdProxy* LoadBannerAd(UObject* WorldContextObject, ECASBannerSize AdSize, bool EventOnce = true);

    // Shows the login UI for the currently active online subsystem, if the subsystem supports a login UI.
    UFUNCTION(BlueprintCallable,
              Category = "CAS Mobile Ads",
              meta = (BlueprintInternalUseOnly = "true", WorldContext = "WorldContextObject"))
    static UCASLoadBannerAdProxy* LoadMrecAd(UObject* WorldContextObject, bool EventOnce = true);

    // UBlueprintAsyncActionBase interface
    virtual void Activate() override;
    // End of UBlueprintAsyncActionBase interface

   protected:
    UFUNCTION()
    void HandleAdClicked();
};