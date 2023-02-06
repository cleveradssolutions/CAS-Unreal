// Fill out your copyright notice in the Description page of Project Settings.

#if PLATFORM_IOS

#include "CASInterface_Banner_IOS.h"
#include "CASInterface_General_IOS.h"
#include "CASSubsystem.h"

#include "CASSettings.h"

#include "Async/Async.h"
#include "IOS/IOSAppDelegate.h"

#include "IOSHelpers.h"

#import <CleverAdsSolutions/CleverAdsSolutions-Swift.h>

UCASInterface_Banner_IOS* CASBannerIOS = nullptr;

@interface FCASBannerViewController : UIViewController<CASBannerDelegate, CASCallback>

@property (nonatomic, strong) CASBannerView *bannerView;
@property (nonatomic, strong) CASMediationManager *manager;

- (id)initWithManager:(CASMediationManager *) manager;
- (void)dealloc;

- (void)viewDidLoad;
- (void)addBannerViewToView:(UIView *_Nonnull)bannerView;
- (void)bannerAdViewDidLoad:(CASBannerView *)view;
- (void)bannerAdView:(CASBannerView *)adView didFailWith:(enum CASError)error;
- (void)bannerAdView:(CASBannerView *)adView willPresent:(id<CASStatusHandler>)impression;
- (void)bannerAdViewDidRecordClick:(CASBannerView *)adView;
@end

@implementation FCASBannerViewController

- (id)initWithManager:(CASMediationManager *) manager
{
	id Self = [super init];
	if (Self)
	{
		self.manager = manager;
	}
	return Self;
}

-(void)dealloc {
	[self.bannerView destroy];
	[super dealloc];
}

- (void)viewDidLoad {  
	[super viewDidLoad];

	UE_LOG(LogTemp, Log, TEXT("CAS Banner viewDidLoad"));
}

-(void)addBannerViewToView:(UIView *_Nonnull)bannerView {

	// In this case, we instantiate the banner with desired ad size.
	self.bannerView = [[CASBannerView alloc] initWithAdSize:CASSize.banner manager:self.manager];        
	[self.bannerView setTranslatesAutoresizingMaskIntoConstraints:NO];

	self.bannerView.adDelegate = self;
	
	// This view controller is used to present an overlay when the ad is clicked. 
	self.bannerView.rootViewController = [UIApplication sharedApplication].keyWindow.rootViewController; // Weak reference
	
	[self.bannerView.rootViewController.view addSubview:self.bannerView];

	[NSLayoutConstraint activateConstraints:@[
		[self.bannerView.centerXAnchor constraintEqualToAnchor:self.bannerView.rootViewController.view.centerXAnchor],
		[self.bannerView.bottomAnchor constraintEqualToAnchor:self.bannerView.rootViewController.view.bottomAnchor]
	]];
}

// ---- Callbacks

- (void)bannerAdViewDidLoad:(CASBannerView *)view
{
	if(!CASBannerIOS) return;
	
	UE_LOG(LogTemp, Log, TEXT("CAS Banner Ad Did Load"));
	
	AsyncTask(ENamedThreads::GameThread, []()
	{
		CASBannerIOS->OnLoaded.Broadcast();
	});
}

- (void)bannerAdView:(CASBannerView *)adView didFailWith:(enum CASError)error
{
	if(!CASBannerIOS) return;
	
	FString ErrorMsg("Unknown");
	
	switch((int)error)
	{
	case CASErrorNoConnection: ErrorMsg = FString("No internet connection detected"); break;
	case CASErrorNoFill: ErrorMsg = FString("No Fill"); break;
	case CASErrorNotReady: ErrorMsg = FString("Ad are not ready. You need to call Load ads or use one of the automatic cache mode."); break;
	case CASErrorManagerIsDisabled: ErrorMsg = FString("Manager is disabled"); break;
	case CASErrorReachedCap: ErrorMsg = FString("Reached cap for user"); break;
	case CASErrorNotEnoughSpace: ErrorMsg = FString("Not enough space to display ads"); break;
	case CASErrorIntervalNotYetPassed: ErrorMsg = FString("The interval between impressions Ad has not yet passed."); break;
	case CASErrorAlreadyDisplayed: ErrorMsg = FString("Ad already displayed"); break;
	case CASErrorAppIsPaused: ErrorMsg = FString("Application is paused"); break;
	default: break;
	}
	
	AsyncTask(ENamedThreads::GameThread, [ErrorMsg]()
	{
		CASBannerIOS->OnFail.Broadcast(FString(ErrorMsg));
	});
}

- (void)bannerAdView:(CASBannerView *)adView willPresent:(id<CASStatusHandler>)impression
{
	if(!CASBannerIOS) return;
	
	UE_LOG(LogTemp, Log, TEXT("CAS Banner Ad Presented"));

	FCASImpressionInfo ImpressionInfo = CASIOSHelpers::ParseImpressionInfo(impression);
	
	AsyncTask(ENamedThreads::GameThread, [ImpressionInfo]()
	{
		CASBannerIOS->OnShow.Broadcast(ImpressionInfo);
	});
}

- (void)bannerAdViewDidRecordClick:(CASBannerView *)adView
{
	if(!CASBannerIOS) return;
	
	UE_LOG(LogTemp, Log, TEXT("CAS Banner Clicked"));
	
	AsyncTask(ENamedThreads::GameThread, []()
	{
		CASBannerIOS->OnClicked.Broadcast();
	});
}

@end

static FCASBannerViewController* CASBannerViewController;

void UCASInterface_Banner_IOS::Init()
{
	CASBannerIOS = this;

	CASMediationManager* Manager = UCASInterface_General_IOS::GetManager();

	CASBannerViewController = [[FCASBannerViewController alloc] initWithManager:Manager];
}

void UCASInterface_Banner_IOS::CreateBanner()
{
	const UCASSettingsIOS* CASSettings = GetDefault<UCASSettingsIOS>();
	
	dispatch_async(dispatch_get_main_queue(), ^{
		[CASBannerViewController addBannerViewToView:CASBannerViewController.bannerView];

		if(!CASSettings) SetRefreshInterval(CASSettings->BannerDefaultRefreshInterval);
	});
}

bool UCASInterface_Banner_IOS::IsReady() const
{
	return CASBannerViewController.bannerView.isAdReady;
}

void UCASInterface_Banner_IOS::ToggleBannerVisibility(bool Visible)
{
	dispatch_async(dispatch_get_main_queue(), ^{
		CASBannerViewController.bannerView.hidden = !Visible;
	});
}

void UCASInterface_Banner_IOS::DestroyBanner()
{
	CASBannerViewController = nil;
}

void UCASInterface_Banner_IOS::LoadNextBanner()
{
	[CASBannerViewController.bannerView loadNextAd];
}

bool UCASInterface_Banner_IOS::IsVisible() const
{
	return !CASBannerViewController.bannerView.isHidden;
}

void UCASInterface_Banner_IOS::SetRefreshInterval(int Interval)
{
	CASBannerViewController.bannerView.refreshInterval = FMath::Max(Interval, 10);
}

void UCASInterface_Banner_IOS::DisableRefreshInterval()
{
	[CASBannerViewController.bannerView disableAdRefresh];
}

#endif