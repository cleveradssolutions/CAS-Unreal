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
	
public:
	
	/** CAS App ID for Android. In most cases, a casID is the same as your application package name. */
	UPROPERTY(Config, EditAnywhere)
	FString AndroidCASAppID;

	/** CAS App ID for IOS. In most cases, a casID is the same as your application store ID. */
	UPROPERTY(Config, EditAnywhere)
	FString IOSCASAppID;
	
	/** Admob App ID for Android */
	UPROPERTY(Config, EditAnywhere)
	FString AndroidAdMobAppID;

	/** Admob App ID for IOS */
	UPROPERTY(Config, EditAnywhere)
	FString IOSAdmobAppID;

	/** Toggle test mode. When enabled, test ads will be served instead of real ones */
	UPROPERTY(Config, EditAnywhere)
	bool TestMode;
};
