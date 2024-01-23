// Copyright CleverAdsSolutions LTD, CAS.AI. All Rights Reserved.

#pragma once

#include "CASDefines.h"
#include "CoreMinimal.h"
#include "UObject/ObjectMacros.h"
#include "Kismet/BlueprintAsyncActionBase.h"
#include "CASShowAdConsentFlowProxy.generated.h"

UCLASS()
class CLEVERADSSOLUTIONS_API UCASShowAdConsentFlowProxy : public UBlueprintAsyncActionBase {
    GENERATED_BODY()

   public:
    UPROPERTY(BlueprintAssignable, Category = "CAS Mobile Ads")
    FCASEvent OnCompleted;

    UFUNCTION(BlueprintCallable,
              Category = "CAS Mobile Ads",
              meta = (BlueprintInternalUseOnly = "true", WorldContext = "WorldContextObject"))
    static UCASShowAdConsentFlowProxy* ShowAdConsentFlow(UObject* WorldContextObject);

    // UBlueprintAsyncActionBase interface
    virtual void Activate() override;
    // End of UBlueprintAsyncActionBase interface

   protected:
    UFUNCTION()
    void HandleResult();

    UObject* WorldContextObject;
};