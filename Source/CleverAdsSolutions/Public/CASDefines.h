#pragma once

#include "CoreMinimal.h"
#include "CASDefines.generated.h"

DEFINE_LOG_CATEGORY_STATIC(LogCAS, Log, All);

USTRUCT(BlueprintType)
struct FCASImpressionInfo
{
	GENERATED_BODY()

public:

	UPROPERTY(BlueprintReadOnly, Category="CleverAdsSolutions|Impression")
	float RevenueCPM;

	UPROPERTY(BlueprintReadOnly, Category="CleverAdsSolutions|Impression")
	FString CurrencyCode = "USD";

	UPROPERTY(BlueprintReadOnly, Category="CleverAdsSolutions|Impression")
	FString Network;

	UPROPERTY(BlueprintReadOnly, Category="CleverAdsSolutions|Impression")
	FString NetworkSDKVersion;

	UPROPERTY(BlueprintReadOnly, Category="CleverAdsSolutions|Impression")
	FString UnitID;

	UPROPERTY(BlueprintReadOnly, Category="CleverAdsSolutions|Impression")
	FString PlacementType;
	
	UPROPERTY(BlueprintReadOnly, Category="CleverAdsSolutions|Impression")
	FString CreativeID;

	UPROPERTY(BlueprintReadOnly, Category="CleverAdsSolutions|Impression")
	int TotalImpressions;
	
	UPROPERTY(BlueprintReadOnly, Category="CleverAdsSolutions|Impression")
	float LifetimeRevenue;
};
