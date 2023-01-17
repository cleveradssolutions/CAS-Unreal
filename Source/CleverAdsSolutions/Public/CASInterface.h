// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "CASInterface.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FCASEvent);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FCASErrorEvent, FString, ErrorMessage);

UCLASS(BlueprintType, Abstract)
class CLEVERADSSOLUTIONS_API UCASInterface : public UObject
{
	GENERATED_BODY()
public:

	virtual void Init(){}
	
};
