// Copyright CleverAdsSolutions LTD, CAS.AI. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "CASImpressionInfo.generated.h"

USTRUCT(BlueprintType)
struct FCASImpressionInfo
{
	GENERATED_BODY()

public:
	/// NOTE: doubles are not supported by UE4 blueprints, so cropping to float
	UPROPERTY(BlueprintReadOnly)
	float RevenueCPM;

	UPROPERTY(BlueprintReadOnly)
	FString Network;

	UPROPERTY(BlueprintReadOnly)
	FString PlacementType;

	UPROPERTY(BlueprintReadOnly)
	FString CurrencyCode = TEXT("USD");

	UPROPERTY(BlueprintReadOnly)
	FString UnitID;

	UPROPERTY(BlueprintReadOnly)
	FString CreativeID;

	UPROPERTY(BlueprintReadOnly)
	int TotalImpressions;

	UPROPERTY(BlueprintReadOnly)
	float LifetimeRevenue;

	UPROPERTY(BlueprintReadOnly)
	FString NetworkSDKVersion = TEXT(""); // Deprecated
};