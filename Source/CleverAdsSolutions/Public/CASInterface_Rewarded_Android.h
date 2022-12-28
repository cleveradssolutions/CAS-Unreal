// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CASInterface_Rewarded.h"
#include "CASInterface_Rewarded_Android.generated.h"

/** CAS Rewarded interface class for Android. */
UCLASS()
class CLEVERADSSOLUTIONS_API UCASInterface_Rewarded_Android : public UCASInterface_Rewarded
{
	GENERATED_BODY()

#if PLATFORM_ANDROID
	
	virtual void Init();
	
	virtual void Show() override;
	
	virtual bool IsReady() override;

#endif
};
