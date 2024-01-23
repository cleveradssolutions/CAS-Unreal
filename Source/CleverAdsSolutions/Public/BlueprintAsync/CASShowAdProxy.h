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

    // Shows the login UI for the currently active online subsystem, if the subsystem supports a login UI.
    UFUNCTION(BlueprintCallable,
              Category = "CAS Mobile Ads",
              meta = (BlueprintInternalUseOnly = "true", WorldContext = "WorldContextObject"))
    static UCASShowAdProxy* ShowInterstitialAd(UObject* WorldContextObject);

    // Shows the login UI for the currently active online subsystem, if the subsystem supports a login UI.
    UFUNCTION(BlueprintCallable,
              Category = "CAS Mobile Ads",
              meta = (BlueprintInternalUseOnly = "true", WorldContext = "WorldContextObject"))
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