// Fill out your copyright notice in the Description page of Project Settings.

#if PLATFORM_IOS

#include "CASInterface_ReturnAds_IOS.h"
#include "CASInterface_General_IOS.h"
#include "CASSubsystem.h"

#include "CASSettings.h"

#include "Async/Async.h"
#include "IOS/IOSAppDelegate.h"

#import <CleverAdsSolutions/CleverAdsSolutions-Swift.h>

#define RETURNADS_CLASSNAME "com/unreal/cas/CASUnrealReturnAds"

UCASInterface_ReturnAds_IOS* CASReturnAdsIOS = nullptr;

@interface FCASReturnAdsViewController : UIViewController<CASAppReturnDelegate, CASCallback>

@property (nonatomic, strong) CASMediationManager *manager;

- (id)initWithManager:(CASMediationManager *) manager;

- (UIViewController * _Nonnull)viewControllerForPresentingAppReturnAd;
- (void)willShown:(id<CASStatusHandler>)impression;
- (void)didShowAdFailed:(NSString *)error;
- (void)didClickedAd;
- (void)didCompletedAd;
- (void)didClosedAd;
@end

@implementation FCASReturnAdsViewController

- (id)initWithManager:(CASMediationManager *) manager
{
	id Self = [super init];
	if (Self)
	{
		self.manager = manager;

		[manager enableAppReturnAdsWith:self];
	}
	return Self;
}

- (UIViewController * _Nonnull)viewControllerForPresentingAppReturnAd
{
	return [UIApplication sharedApplication].keyWindow.rootViewController;
}

// ---- Callbacks

- (void)willShown:(id<CASStatusHandler>)impression
{
	if(!CASReturnAdsIOS) return;

	FCASImpressionInfo ImpressionInfo = CASIOSHelpers::ParseImpressionInfo(impression);
	
	AsyncTask(ENamedThreads::GameThread, [ImpressionInfo]()
	{
		CASReturnAdsIOS->OnShown.Broadcast(ImpressionInfo);
	});
}

- (void)didShowAdFailed:(NSString *)error
{
	if(!CASReturnAdsIOS) return;
	
	FString ErrorMsg = FString(error);
	
	AsyncTask(ENamedThreads::GameThread, [ErrorMsg]()
	{
		CASReturnAdsIOS->OnShowError.Broadcast(ErrorMsg);
	});
}

- (void)didClickedAd
{
	if(!CASReturnAdsIOS) return;
	
	AsyncTask(ENamedThreads::GameThread, []()
	{
		CASReturnAdsIOS->OnClicked.Broadcast();
	});
}

- (void)didCompletedAd
{
	if(!CASReturnAdsIOS) return;
	
	AsyncTask(ENamedThreads::GameThread, []()
	{
		CASReturnAdsIOS->OnCompleted.Broadcast();
	});
}

- (void)didClosedAd
{
	if(!CASReturnAdsIOS) return;
	
	AsyncTask(ENamedThreads::GameThread, []()
	{
		CASReturnAdsIOS->OnClosed.Broadcast();
	});
}

@end

static FCASReturnAdsViewController* CASReturnAdsViewController;

void UCASInterface_ReturnAds_IOS::Init()
{
	CASReturnAdsIOS = this;

	const UCASSettingsIOS* CASSettings = GetDefault<UCASSettingsIOS>();

	if(CASSettings->ReturnAds)
	{
		CASMediationManager* Manager = UCASInterface_General_IOS::GetManager();
	
		CASReturnAdsViewController = [[FCASReturnAdsViewController alloc] initWithManager:Manager];
	}
}

void UCASInterface_ReturnAds_IOS::SkipNextAppReturnAds()
{
	[CASReturnAdsViewController.manager skipNextAppReturnAds];
}

void UCASInterface_ReturnAds_IOS::DisableAppReturnAds()
{
	[CASReturnAdsViewController.manager disableAppReturnAds];
}

#endif
