// Copyright CleverAdsSolutions LTD, CAS.AI. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "CASImpressionInfo.generated.h"

USTRUCT(BlueprintType)
struct FCASImpressionInfo {
    GENERATED_BODY()

   public:
    FCASImpressionInfo() : RevenueCPM(0.0f), TotalImpressions(0), LifetimeRevenue(0.0f) {}

    /// NOTE: doubles are not supported by UE4 blueprints, so cropping to float
    UPROPERTY(BlueprintReadOnly, Category = "CAS Mobile Ads")
    float RevenueCPM;

    UPROPERTY(BlueprintReadOnly, Category = "CAS Mobile Ads")
    FString Network;

    UPROPERTY(BlueprintReadOnly, Category = "CAS Mobile Ads")
    FString PlacementType;

    UPROPERTY(BlueprintReadOnly, Category = "CAS Mobile Ads")
    FString CurrencyCode = TEXT("USD");

    UPROPERTY(BlueprintReadOnly, Category = "CAS Mobile Ads")
    FString UnitID;

    UPROPERTY(BlueprintReadOnly, Category = "CAS Mobile Ads")
    FString CreativeID;

    UPROPERTY(BlueprintReadOnly, Category = "CAS Mobile Ads")
    int TotalImpressions;

    UPROPERTY(BlueprintReadOnly, Category = "CAS Mobile Ads")
    float LifetimeRevenue;

    UPROPERTY(BlueprintReadOnly, Category = "CAS Mobile Ads")
    FString NetworkSDKVersion = TEXT("");  // Deprecated
};