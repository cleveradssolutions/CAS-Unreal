// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CASInterface_General.h"
#include "CASInterface_General_Android.generated.h"

/** CAS general interface class for Android. */
UCLASS()
class CLEVERADSSOLUTIONS_API UCASInterface_General_Android : public UCASInterface_General
{
	GENERATED_BODY()

#if PLATFORM_ANDROID
	
	virtual void Init() override;

	virtual FString GetCASVersion() const override;

	virtual void SetMuteAdSounds(bool Mute) override;

	virtual void SetLoadingMode(ECASLoadingMode Mode) override;

	virtual void ValidateIntegration() override;

#endif
};
