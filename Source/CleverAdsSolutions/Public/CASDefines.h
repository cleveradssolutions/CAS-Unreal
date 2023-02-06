#pragma once

#include "CoreMinimal.h"
#include "CASDefines.generated.h"

DEFINE_LOG_CATEGORY_STATIC(LogCAS, Log, All);

USTRUCT(BlueprintType)
struct FCASImpressionInfo
{
	GENERATED_BODY()

public:

	UPROPERTY(BlueprintReadOnly)
	float RevenueCPM;

	UPROPERTY(BlueprintReadOnly)
	FString CurrencyCode = "USD";

	UPROPERTY(BlueprintReadOnly)
	FString Network;

	UPROPERTY(BlueprintReadOnly)
	FString NetworkSDKVersion;

	UPROPERTY(BlueprintReadOnly)
	FString UnitID;

	UPROPERTY(BlueprintReadOnly)
	FString PlacementType;
	
	UPROPERTY(BlueprintReadOnly)
	FString CreativeID;

	UPROPERTY(BlueprintReadOnly)
	int TotalImpressions;
	
	UPROPERTY(BlueprintReadOnly)
	float LifetimeRevenue;
};
