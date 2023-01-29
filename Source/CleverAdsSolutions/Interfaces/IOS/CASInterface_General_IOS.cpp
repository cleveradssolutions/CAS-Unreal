// Fill out your copyright notice in the Description page of Project Settings.

#include "CASInterface_General_IOS.h"

#if PLATFORM_IOS

#include "CASSettings.h"

#import <CleverAdsSolutions/CleverAdsSolutions-Swift.h>
#import <FBAudienceNetwork/FBAudienceNetwork.h>

namespace CASUnrealIOS
{
	static CASMediationManager * Manager;
}

CASMediationManager* UCASInterface_General_IOS::GetManager()
{
	return CASUnrealIOS::Manager;
}

void UCASInterface_General_IOS::Init()
{
	Super::Init();

	const UCASSettings* CASPluginSettings = GetDefault<UCASSettings>();

	if(!CASPluginSettings) return;

	CASManagerBuilder* builder = [CAS buildManager];

	const FEngineVersion& EngineVersion = FEngineVersion::Current();
	FString VersionString = FString::Printf(TEXT("%u"), EngineVersion.GetMajor());
	VersionString += FString::Printf(TEXT(".%u"), EngineVersion.GetMinor());

	[[CAS settings] setPluginPlatformWithName:@"Unreal" version:[NSString stringWithFString:VersionString]];

	[[CAS settings] setDebugMode:CASPluginSettings->DebugMode];
	
	// List Ad formats used in app
	[builder withAdFlags:CASTypeFlagsBanner | CASTypeFlagsInterstitial | CASTypeFlagsRewarded];
	
	[builder withTestAdMode:CASPluginSettings->TestAds];

	// Facebook AdvertiserTrackingEnabled flag
	[FBAdSettings setAdvertiserTrackingEnabled:CASPluginSettings->IOSAdvertiserTrackingEnabled];
	
	// TODO: Completion handler
	
	CASUnrealIOS::Manager = [builder createWithCasId:[NSString stringWithFString:CASPluginSettings->IOSCASAppID]];
}

FString UCASInterface_General_IOS::GetCASVersion() const
{
	return FString([CAS getSDKVersion]);
}

void UCASInterface_General_IOS::ValidateIntegration()
{
	[CAS validateIntegration];
}

void UCASInterface_General_IOS::SetMuteAdSounds(bool Mute)
{
	[[CAS settings] setMuteAdSoundsTo:Mute];
}

void UCASInterface_General_IOS::SetLoadingMode(ECASLoadingMode Mode)
{
	[[CAS settings] setLoadingWithMode:(CASLoadingManagerMode)Mode];
}

void UCASInterface_General_IOS::SetTaggedAudience(ECASChildrenAudience ChildrenAudienceStatus)
{
	switch (ChildrenAudienceStatus)
	{
	case ECASChildrenAudience::Children: [CAS.settings setTaggedWithAudience: CASAudienceChildren]; break;
	case ECASChildrenAudience::NotChildren: [CAS.settings setTaggedWithAudience: CASAudienceNotChildren]; break;
	default: [CAS.settings setTaggedWithAudience: CASAudienceUndefined];
	}
}

void UCASInterface_General_IOS::SetUserConsent(ECASUserConsentStatus ConsentStatus)
{
	switch (ConsentStatus)
	{
	case ECASUserConsentStatus::Accepted: [CAS.settings updateUserWithConsent: CASConsentStatusAccepted]; break;
	case ECASUserConsentStatus::Denied: [CAS.settings updateUserWithConsent: CASConsentStatusDenied]; break;
	default: break;
	}
}

void UCASInterface_General_IOS::SetCCPAStatus(ECASUserCCPAStatus CCPAStatus)
{
	switch (CCPAStatus)
	{
	case ECASUserCCPAStatus::OptOutSale: [CAS.settings updateCCPAWithStatus: CASCCPAStatusOptOutSale]; break;
	case ECASUserCCPAStatus::OptInSale: [CAS.settings updateCCPAWithStatus: CASCCPAStatusOptInSale]; break;
	default: break;
	}
}

#endif