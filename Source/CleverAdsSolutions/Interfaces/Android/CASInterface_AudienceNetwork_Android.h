// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CASInterface_AudienceNetwork.h"
#include "CASInterface_AudienceNetwork_Android.generated.h"

/** CAS AudienceNetwork interface class for IOS. */
UCLASS()
class CLEVERADSSOLUTIONS_API UCASInterface_AudienceNetwork_Android : public UCASInterface_AudienceNetwork
{
	GENERATED_BODY()

#if PLATFORM_ANDROID && WITH_CAS_FACEBOOK

	virtual void SetDataProcessingOptions(ECASANDataProcessingOption Option) override;

#endif
};
