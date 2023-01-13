﻿// Fill out your copyright notice in the Description page of Project Settings.

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

	const UCASSettings* CASPluginSettings = GetDefault<UCASSettings>();

	if(!CASPluginSettings) return;

	CASManagerBuilder* builder = [CAS buildManager];
	
	// List Ad formats used in app
	[builder withAdFlags:CASTypeFlagsBanner | CASTypeFlagsInterstitial | CASTypeFlagsRewarded];
	
	[builder withTestAdMode:CASPluginSettings->TestMode];
	
	// TODO: Completion handler
	
	CASUnrealIOS::Manager = [builder createWithCasId:[NSString stringWithFString:CASPluginSettings->IOSAppID]];
}

#endif