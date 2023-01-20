// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CASInterface_General.h"
#include "CASInterface_General_IOS.generated.h"

#if PLATFORM_IOS
#import <CleverAdsSolutions/CleverAdsSolutions-Swift.h>
#endif

/** CAS general interface class for IOS. */
UCLASS()
class CLEVERADSSOLUTIONS_API UCASInterface_General_IOS : public UCASInterface_General
{
	GENERATED_BODY()

#if PLATFORM_IOS
public:
	
	static CASMediationManager* GetManager();
	
	virtual void Init() override;

	virtual FString GetCASVersion() const override;

	virtual void SetMuteAdSounds(bool Mute) override;

	virtual void SetLoadingMode(ECASLoadingMode Mode) override;

	virtual void ValidateIntegration() override;

	virtual void SetTaggedAudience(ECASChildrenAudience ChildrenAudienceStatus) override;

	virtual void SetUserConsent(ECASUserConsentStatus ConsentStatus) override;

	virtual void SetCCPAStatus(ECASUserCCPAStatus CCPAStatus) override;

#endif
};
