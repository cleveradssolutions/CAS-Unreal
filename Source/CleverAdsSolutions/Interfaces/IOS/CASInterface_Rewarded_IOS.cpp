// Fill out your copyright notice in the Description page of Project Settings.

#if PLATFORM_IOS

#include "CASInterface_Rewarded_IOS.h"
#include "CASInterface_General_IOS.h"
#include "CASSubsystem.h"

#include "Async/Async.h"
#include "IOS/IOSAppDelegate.h"

#import <CleverAdsSolutions/CleverAdsSolutions-Swift.h>

UCASInterface_Rewarded_IOS* CASRewardedIOS = nullptr;

@interface FCASRewardedViewController : UIViewController<CASLoadDelegate, CASCallback>

@property (nonatomic, strong) CASMediationManager *manager;

- (id)initWithManager:(CASMediationManager *) manager;

- (void)viewDidLoad;
- (void)onAdLoaded:(CASType)adType;
- (void)onAdFailedToLoad:(CASType) adType withError:(NSString *)error;
- (void)willShownWithAd:(id<CASStatusHandler>)adStatus;
- (void)didShowAdFailedWithError:(NSString *)error;
- (void)didClickedAd;
- (void)didCompletedAd;
- (void)didClosedAd;
@end

@implementation FCASRewardedViewController

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
	if (adType == CASTypeRewarded) {
		if(!CASRewardedIOS) return;
	
		AsyncTask(ENamedThreads::GameThread, []()
		{
			CASRewardedIOS->OnLoaded.Broadcast();
		});
	}
}
- (void)onAdFailedToLoad:(CASType) adType withError:(NSString *)error {
	if (adType == CASTypeRewarded) {
		if(!CASRewardedIOS) return;
	
		FString ErrorMsg = FString(error);
	
		AsyncTask(ENamedThreads::GameThread, [ErrorMsg]()
		{
			CASRewardedIOS->OnLoadError.Broadcast(ErrorMsg);
		});
	}
}

- (void)willShownWithAd:(id<CASStatusHandler>)adStatus
{
	if(!CASInterstitialIOS) return;
	
	AsyncTask(ENamedThreads::GameThread, []()
	{
		CASRewardedIOS->OnShown.Broadcast();
	});
}

- (void)didShowAdFailedWithError:(NSString *)error
{
	if(!CASRewardedIOS) return;
	
	FString ErrorMsg = FString(error);
	
	AsyncTask(ENamedThreads::GameThread, [ErrorMsg]()
	{
		CASRewardedIOS->OnShowError.Broadcast(ErrorMsg);
	});
}

- (void)didClickedAd
{
	if(!CASRewardedIOS) return;
	
	AsyncTask(ENamedThreads::GameThread, []()
	{
		CASRewardedIOS->OnClicked.Broadcast();
	});
}

- (void)didCompletedAd
{
	if(!CASRewardedIOS) return;
	
	AsyncTask(ENamedThreads::GameThread, []()
	{
		CASRewardedIOS->OnCompleted.Broadcast();
	});
}

- (void)didClosedAd
{
	if(!CASRewardedIOS) return;
	
	AsyncTask(ENamedThreads::GameThread, []()
	{
		CASRewardedIOS->OnClosed.Broadcast();
	});
}

@end

static FCASRewardedViewController* CASRewardedViewController;

void UCASInterface_Rewarded_IOS::Init()
{
	CASRewardedIOS = this;

	CASMediationManager* Manager = UCASInterface_General_IOS::GetManager();

	CASRewardedViewController = [[FCASRewardedViewController alloc] initWithManager:Manager];
}

void UCASInterface_Rewarded_IOS::Show()
{
	dispatch_async(dispatch_get_main_queue(), ^{
		[CASRewardedViewController.manager presentRewardedAdFromRootViewController:[IOSAppDelegate GetDelegate].IOSController callback:CASRewardedViewController];
	});
}

bool UCASInterface_Rewarded_IOS::IsReady()
{
	return CASRewardedViewController.manager.isRewardedAdReady;
}
#endif