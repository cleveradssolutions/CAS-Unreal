// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CASInterface_Rewarded.h"
#include "CASInterface_Rewarded_IOS.generated.h"

/** CAS Rewarded interface class for IOS. */
UCLASS()
class CLEVERADSSOLUTIONS_API UCASInterface_Rewarded_IOS : public UCASInterface_Rewarded
{
	GENERATED_BODY()

#if PLATFORM_IOS
	
	virtual void Init();
	
	virtual void Show() override;
	
	virtual bool IsReady() override;

	virtual void Load() override;

#endif
};
