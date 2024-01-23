// Copyright CleverAdsSolutions LTD, CAS.AI. All Rights Reserved.

#pragma once

#include "CASDefines.h"
#include "CoreMinimal.h"
#include "UObject/ObjectMacros.h"
#include "Kismet/BlueprintAsyncActionBase.h"
#include "CASInitializeAdProxy.generated.h"

UCLASS()
class CLEVERADSSOLUTIONS_API UCASInitializeAdProxy : public UBlueprintAsyncActionBase {
    GENERATED_BODY()

   public:
    UPROPERTY(BlueprintAssignable, Category = "CAS Mobile Ads")
    FCASInitResultEvent OnCompleted;

    UPROPERTY(BlueprintAssignable, Category = "CAS Mobile Ads")
    FCASInitResultEvent OnFailed;

    UFUNCTION(BlueprintCallable,
              Category = "CAS Mobile Ads",
              meta = (BlueprintInternalUseOnly = "true", WorldContext = "WorldContextObject"))
    static UCASInitializeAdProxy* InitializeMobileAds(UObject* WorldContextObject);

    // UBlueprintAsyncActionBase interface
    virtual void Activate() override;
    // End of UBlueprintAsyncActionBase interface

   protected:
    UFUNCTION()
    void HandleResult(const FCASInitialConfig& Config);

    UObject* WorldContextObject;
};