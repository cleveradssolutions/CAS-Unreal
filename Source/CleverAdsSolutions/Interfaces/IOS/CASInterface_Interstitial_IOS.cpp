// Fill out your copyright notice in the Description page of Project Settings.

#if PLATFORM_IOS

#include "CASInterface_Interstitial_IOS.h"
#include "CASInterface_General_IOS.h"
#include "CASSubsystem.h"

#include "Async/Async.h"
#include "IOS/IOSAppDelegate.h"

#include "CASSettings.h"

#import <CleverAdsSolutions/CleverAdsSolutions-Swift.h>

UCASInterface_Interstitial_IOS* CASInterstitialIOS = nullptr;

@interface FCASInterstitialViewController : UIViewController<CASLoadDelegate, CASCallback>

@property (nonatomic, strong) CASMediationManager *manager;

- (id)initWithManager:(CASMediationManager *) manager;

- (void)viewDidLoad;
- (void)onAdLoaded:(CASType)adType;
- (void)onAdFailedToLoad:(CASType) adType withError:(NSString *)error;
- (void)willShownWithAd:(id<CASStatusHandler>)impression;
- (void)didShowAdFailedWithError:(NSString *)error;
- (void)didClickedAd;
- (void)didCompletedAd;
- (void)didClosedAd;
@end

@implementation FCASInterstitialViewController

- (id)initWithManager:(CASMediationManager *) manager
{
	id Self = [super init];
	if (Self)
	{
		self.manager = manager;
	}
	return Self;
}

- (void)viewDidLoad {  
	[super viewDidLoad];
	
	self.manager.adLoadDelegate = self; // Weak reference
}

// ---- Callbacks

- (void)onAdLoaded:(CASType)adType {
	if (adType == CASTypeInterstitial) {
		if(!CASInterstitialIOS) return;
	
		AsyncTask(ENamedThreads::GameThread, []()
		{
			CASInterstitialIOS->OnLoaded.Broadcast();
		});
	}
}
- (void)onAdFailedToLoad:(CASType) adType withError:(NSString *)error {
	if (adType == CASTypeInterstitial) {
		if(!CASInterstitialIOS) return;
	
		FString ErrorMsg = FString(error);
	
		AsyncTask(ENamedThreads::GameThread, [ErrorMsg]()
		{
			CASInterstitialIOS->OnLoadError.Broadcast(ErrorMsg);
		});
	}
}

- (void)willShownWithAd:(id<CASStatusHandler>)impression
{
	if(!CASInterstitialIOS) return;

	FCASImpressionInfo ImpressionInfo = CASIOSHelpers::ParseImpressionInfo(impression);
	
	AsyncTask(ENamedThreads::GameThread, [ImpressionInfo]()
	{
		CASInterstitialIOS->OnShown.Broadcast(ImpressionInfo);
	});
}

- (void)didShowAdFailedWithError:(NSString *)error
{
	if(!CASInterstitialIOS) return;
	
	FString ErrorMsg = FString(error);
	
	AsyncTask(ENamedThreads::GameThread, [ErrorMsg]()
	{
		CASInterstitialIOS->OnShowError.Broadcast(ErrorMsg);
	});
}

- (void)didClickedAd
{
	if(!CASInterstitialIOS) return;
	
	AsyncTask(ENamedThreads::GameThread, []()
	{
		CASInterstitialIOS->OnClicked.Broadcast();
	});
}

- (void)didCompletedAd
{
	if(!CASInterstitialIOS) return;
	
	AsyncTask(ENamedThreads::GameThread, []()
	{
		CASInterstitialIOS->OnCompleted.Broadcast();
	});
}

- (void)didClosedAd
{
	if(!CASInterstitialIOS) return;
	
	AsyncTask(ENamedThreads::GameThread, []()
	{
		CASInterstitialIOS->OnClosed.Broadcast();
	});
}

@end

static FCASInterstitialViewController* CASInterstitialViewController;

void UCASInterface_Interstitial_IOS::PreInit()
{
	const UCASSettingsIOS* CASSettings = GetDefault<UCASSettingsIOS>();
	SetMinimumInterval(CASSettings->MinimumInterstitialInterval);
}

void UCASInterface_Interstitial_IOS::Init()
{
	CASInterstitialIOS = this;

	CASMediationManager* Manager = UCASInterface_General_IOS::GetManager();
	
	CASInterstitialViewController = [[FCASInterstitialViewController alloc] initWithManager:Manager];
}

void UCASInterface_Interstitial_IOS::Show()
{
	dispatch_async(dispatch_get_main_queue(), ^{
		[CASInterstitialViewController.manager presentInterstitialFromRootViewController:[IOSAppDelegate GetDelegate].IOSController callback:CASInterstitialViewController];
	});
}

bool UCASInterface_Interstitial_IOS::IsReady()
{
	return CASInterstitialViewController.manager.isInterstitialReady;
}

void UCASInterface_Interstitial_IOS::Load()
{
	[CASInterstitialViewController.manager loadInterstitial];
}

void UCASInterface_Interstitial_IOS::SetMinimumInterval(int Interval)
{
	[[CAS settings] setInterstitialWithInterval:Interval];
}

void UCASInterface_Interstitial_IOS::RestartInterval()
{
	[[CAS settings] restartInterstitialInterval];
}

#endif