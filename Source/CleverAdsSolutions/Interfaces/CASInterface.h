// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "CASDefines.h"
#include "CASInterface.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FCASEvent);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FCASErrorEvent, FString, ErrorMessage);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FCASImpressionEvent, FCASImpressionInfo, ImpressionInfo);

UCLASS(BlueprintType, Abstract)
class CLEVERADSSOLUTIONS_API UCASInterface : public UObject
{
	GENERATED_BODY()
public:

	virtual void Init(){}
	
	virtual void PreInit(){}
	
};
