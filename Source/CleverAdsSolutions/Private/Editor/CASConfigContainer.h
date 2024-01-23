// Copyright CleverAdsSolutions LTD, CAS.AI. All Rights Reserved.

#pragma once

#include "CASDefaultConfig.h"
#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "CASConfigContainer.generated.h"

UCLASS()
class CLEVERADSSOLUTIONS_API UCASConfigContainer : public UObject {
    GENERATED_BODY()
#if WITH_EDITORONLY_DATA
   private:
    UPROPERTY()
    UCASDefaultConfig* ConfigAndroid;
    UPROPERTY()
    UCASDefaultConfig* ConfigIOS;
#endif
#if WITH_EDITOR
   public:
    static UCASConfigContainer* Get();

    UCASDefaultConfig* GetAndroid();

    UCASDefaultConfig* GetIOS();
#endif
};