// Fill out your copyright notice in the Description page of Project Settings.

#include "CASInterface_General_IOS.h"

#if PLATFORM_IOS

#include "CASSettings.h"

#import <CleverAdsSolutions/CleverAdsSolutions-Swift.h>

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

	const UCASSettingsIOS* CASSettings = GetDefault<UCASSettingsIOS>();

	if(!CASSettings) return;

	CASManagerBuilder* builder = [CAS buildManager];

	// Test devices
	NSMutableArray *testDevices = [[NSMutableArray alloc] init];
	for(const FString& DeviceId : CASSettings->TestDevices)
	{
		[testDevices addObject:[NSString stringWithFString:DeviceId]];
	}

	[[CAS settings] setTestDeviceWithIds:testDevices];

	[[CAS settings] setDebugMode:CASSettings->DebugMode];

	[[CAS settings] setInterstitialAdsWhenVideoCostAreLowerWithAllow:CASSettings->AllowInterstitialWhenRewardsCostAreLower];
	
	// List Ad formats used in app
	int enableAdTypes = CASTypeFlagsNone;
            
	if (CASSettings->BannerAds)
		enableAdTypes |= CASTypeFlagsBanner;

	if (CASSettings->InterstitialAds)
		enableAdTypes |= CASTypeFlagsInterstitial;

	if (CASSettings->RewardedAds)
		enableAdTypes |= CASTypeFlagsRewarded;
	
	[builder withAdFlags:CASTypeFlagsBanner | CASTypeFlagsInterstitial | CASTypeFlagsRewarded];
	
	[builder withTestAdMode:CASSettings->TestAds];

	// Framework Unreal info
	const FEngineVersion& EngineVersion = FEngineVersion::Current();
	FString VersionString = FString::Printf(TEXT("%u"), EngineVersion.GetMajor());
	VersionString += FString::Printf(TEXT(".%u"), EngineVersion.GetMinor());

	[builder withFramework:@"Unreal" version:[NSString stringWithFString:VersionString]];

	// Consent Flow
	CASConsentFlow *flow = [[CASConsentFlow alloc] initWithEnabled:CASSettings->ConsentFlow];
	flow.privacyPolicyUrl = [NSString stringWithFString:CASSettings->ConsentFlowURL];
	[builder withConsentFlow:flow];
	
	[builder withCompletionHandler:^(CASInitialConfig *config) {
		if (config.error != nil) {
			OnInitialized.Execute();
		}
	}];
	
	CASUnrealIOS::Manager = [builder createWithCasId:[NSString stringWithFString:CASSettings->CASAppID]];

	CASUnrealIOS::Manager.lastPageAdContent =
		[[CASLastPageAdContent alloc]
			initWithHeadline:[NSString stringWithFString:CASSettings->Headline]
			adText:[NSString stringWithFString:CASSettings->AdText]
			destinationURL:[NSString stringWithFString:CASSettings->DestinationURL]
			imageURL:[NSString stringWithFString:CASSettings->ImageURL]
			iconURL:[NSString stringWithFString:CASSettings->IconURL]
		];
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