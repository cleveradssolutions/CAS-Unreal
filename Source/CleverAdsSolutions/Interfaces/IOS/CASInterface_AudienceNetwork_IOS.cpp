// Fill out your copyright notice in the Description page of Project Settings.

#if PLATFORM_IOS && WITH_CAS_FACEBOOK

#include "CASInterface_AudienceNetwork_IOS.h"

#include "CASSettings.h"

#import <FBAudienceNetwork/FBAudienceNetwork.h>

void UCASInterface_AudienceNetwork_IOS::PreInit()
{
	const UCASSettingsIOS* CASSettings = GetDefault<UCASSettingsIOS>();
	
	// Facebook AdvertiserTrackingEnabled flag
	[FBAdSettings setAdvertiserTrackingEnabled:CASSettings->AdvertiserTrackingEnabled];
}

void UCASInterface_AudienceNetwork_IOS::SetDataProcessingOptions(ECASANDataProcessingOption Option)
{
	switch(Option)
	{
	case ECASANDataProcessingOption::DisableLDU:
		[FBAdSettings setDataProcessingOptions:@[]];
		break;
	case ECASANDataProcessingOption::EnableLDU:
		[FBAdSettings setDataProcessingOptions:@[@"LDU"] country:0 state:0];
		break;
	case ECASANDataProcessingOption::EnableLDU_1_1000:
		[FBAdSettings setDataProcessingOptions:@[@"LDU"] country:1 state:1000];
		break;
	default: break;
	}
}

#endif
