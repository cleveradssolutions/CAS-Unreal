// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "CASSettings.generated.h"

/** Clever Ads Solutions config object for Project Settings tab */
UCLASS(Config = Engine, DefaultConfig)
class CLEVERADSSOLUTIONS_API UCASSettings : public UObject
{
	GENERATED_BODY()

	/** CAS App ID for Android */
	UPROPERTY(Config, EditAnywhere)
	FString AndroidAppID;

	/** CAS App ID for IOS */
	UPROPERTY(Config, EditAnywhere)
	FString IOSAppID;

	/** Toggle test mode. When enabled, test ads will be served instead of real ones */
	UPROPERTY(Config, EditAnywhere)
	bool TestMode;
};
