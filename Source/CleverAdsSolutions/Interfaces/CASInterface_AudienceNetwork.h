// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CASInterface.h"
#include "CASInterface_AudienceNetwork.generated.h"


UENUM()
enum ECASANDataProcessingOption
{
	DisableLDU, // Not enable Limited Data Use
	EnableLDU, // Enable LDU mode using geolocation
	EnableLDU_1_1000 // Enable LDU for users and specify user geography
};

/** Abstract CAS interface class for Audience Network adapter. */
UCLASS()
class CLEVERADSSOLUTIONS_API UCASInterface_AudienceNetwork : public UCASInterface
{
	GENERATED_BODY()

public:
	
	/** Facebook’s Limited Data Use flag in California */
	UFUNCTION(BlueprintCallable, Category="CleverAdsSolutions|AudienceNetwork")
	virtual void SetDataProcessingOptions(ECASANDataProcessingOption Option){}

};
