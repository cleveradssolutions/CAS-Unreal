// Fill out your copyright notice in the Description page of Project Settings.

#if PLATFORM_IOS

#include "CASInterface_Banner_IOS.h"
#include "CASInterface_General_IOS.h"
#include "CASSubsystem.h"

#include "Async/Async.h"
#include "IOS/IOSAppDelegate.h"

#import <CleverAdsSolutions/CleverAdsSolutions-Swift.h>

UCASInterface_Banner_IOS* CASBannerIOS = nullptr;

@interface FCASBannerViewController : UIViewController<CASBannerDelegate, CASCallback>

@property (nonatomic, strong) CASBannerView *bannerView;
@property (nonatomic, strong) CASMediationManager *manager;

- (id)initWithManager:(CASMediationManager *) manager;
- (void)dealloc;

- (void)viewDidLoad;
- (void)addBannerViewToView:(UIView *_Nonnull)bannerView;
- (void)positionBannerViewAtBottomOfSafeArea:(UIView *_Nonnull)bannerView;
- (void)bannerAdViewDidLoad:(CASBannerView *)view;
- (void)bannerAdView:(CASBannerView *)adView didFailWith:(NSString *)error;
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

	// In this case, we instantiate the banner with desired ad size.
	self.bannerView = [[CASBannerView alloc] initWithAdSize:CASSize.banner manager:self.manager];        
	[self.bannerView setTranslatesAutoresizingMaskIntoConstraints:NO];
	
	// This view controller is used to present an overlay when the ad is clicked. 
	self.bannerView.rootViewController = self; // Weak reference

	[self addBannerViewToView:self.bannerView];
}

-(void)addBannerViewToView:(UIView *_Nonnull)bannerView {
	self.bannerView.translatesAutoresizingMaskIntoConstraints = NO;
	[[IOSAppDelegate GetDelegate].IOSController.view addSubview:self.bannerView];
	[self positionBannerViewAtBottomOfSafeArea:bannerView];
}

- (void)positionBannerViewAtBottomOfSafeArea:(UIView *_Nonnull)bannerView {
	// Position the banner. Stick it to the bottom of the Safe Area.
	// Centered horizontally.
	UILayoutGuide *guide = [IOSAppDelegate GetDelegate].IOSController.view.safeAreaLayoutGuide;
	
	[NSLayoutConstraint activateConstraints:@[
	  [bannerView.centerXAnchor constraintEqualToAnchor:guide.centerXAnchor],
	  [bannerView.bottomAnchor constraintEqualToAnchor:guide.bottomAnchor]
	]];
}

// ---- Callbacks

- (void)bannerAdViewDidLoad:(CASBannerView *)view
{
	if(!CASBannerIOS) return;
	
	AsyncTask(ENamedThreads::GameThread, []()
	{
		CASBannerIOS->OnLoaded.Broadcast();
	});
}

- (void)bannerAdView:(CASBannerView *)adView didFailWith:(NSString *)error
{
	if(!CASBannerIOS) return;
	
	FString ErrorMsg = FString(error);
	
	AsyncTask(ENamedThreads::GameThread, [ErrorMsg]()
	{
		CASBannerIOS->OnShowError.Broadcast(ErrorMsg);
	});
}

- (void)bannerAdView:(CASBannerView *)adView willPresent:(id<CASStatusHandler>)impression
{
	if(!CASBannerIOS) return;
	
	AsyncTask(ENamedThreads::GameThread, []()
	{
		CASBannerIOS->OnCreated.Broadcast();
	});
}

- (void)bannerAdViewDidRecordClick:(CASBannerView *)adView
{
	if(!CASBannerIOS) return;
	
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
}

void UCASInterface_Banner_IOS::CreateBanner()
{
	CASMediationManager* Manager = UCASInterface_General_IOS::GetManager();
	
	CASBannerViewController = [[FCASBannerViewController alloc] initWithManager:Manager];
	
//	[CASBannerViewController addBannerViewToView:CASBannerViewController.bannerView];
}

bool UCASInterface_Banner_IOS::IsReady() const
{
	return CASBannerViewController.bannerView.isAdReady;
}

void UCASInterface_Banner_IOS::ToggleBannerVisibility(bool Visible)
{
	CASBannerViewController.bannerView.hidden = !Visible;
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

#endif