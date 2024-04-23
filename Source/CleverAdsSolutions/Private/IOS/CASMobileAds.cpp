// Copyright CleverAdsSolutions LTD, CAS.AI. All Rights Reserved.

#if PLATFORM_IOS
#include "CASMobileAds.h"

#include "CASInternal.h"
#include "CASDefaultConfig.h"
#include "Async/Async.h"
#include "IOS/IOSAppDelegate.h"
#include "Misc/EngineVersion.h"

THIRD_PARTY_INCLUDES_START
#include <CASUnrealBridge/CASUnrealBridge.h>
THIRD_PARTY_INCLUDES_END

CASUBridge *GetBridge() {
    static CASUBridge *Bridge;
    static dispatch_once_t OnceToken;

    dispatch_once(&OnceToken, ^{
      const UCASDefaultConfig *DefaultConfig = GetDefault<UCASDefaultConfig>();

      CAS_LOG_W("Apply Ads ID: %s", *DefaultConfig->CASAppID);

      // Framework Unreal info
      const FEngineVersion &EngineVersion = FEngineVersion::Current();
      const FString VersionString = FString::Printf(TEXT("%u.%u.%u"), EngineVersion.GetMajor(),
                                                    EngineVersion.GetMinor(), EngineVersion.GetPatch());

      Bridge = [[CASUBridge alloc] initWithId:DefaultConfig->CASAppID.GetNSString()
                                engineVersion:VersionString.GetNSString()];

      if (DefaultConfig->Audience != ECASAudience::Undefined) {
          [GetBridge() setUserAudienceForAds:static_cast<int>(DefaultConfig->Audience)];
      }

#if !UE_BUILD_SHIPPING
      if (DefaultConfig->TestAdsMode) {
          [Bridge setTestAdModeForInit];
      }
#endif

      TArray<FString> TestDeviceIds = DefaultConfig->TestDeviceIds;
      if (TestDeviceIds.Num()) {
          NSMutableArray<NSString *> *NewArray = [NSMutableArray arrayWithCapacity:TestDeviceIds.Num()];
          for (const FString &Element : TestDeviceIds) {
              [NewArray addObject:Element.GetNSString()];
          }
          [Bridge setTestDeviceIds:NewArray];
      }

      UIViewController *Controller = [IOSAppDelegate GetDelegate].IOSController;

      [Bridge setConsentFlowEnabled:DefaultConfig->AutoConsentFlow
                          geography:static_cast<int>(DefaultConfig->UserDebugGeography)
                         privacyURL:DefaultConfig->PrivacyPolicyURL.GetNSString()
                         controller:Controller];

      [Bridge setMetaAdvertiserTrackingEnabled:DefaultConfig->UseAdvertisingId];

      [Bridge setAutoloadFormatsWith:DefaultConfig->AutoloadBannerAds
                            withMRec:DefaultConfig->AutoloadMRecAds
                           withInter:DefaultConfig->AutoloadInterstitialAds
                          withReward:DefaultConfig->AutoloadRewardedAds];
    });
    return Bridge;
}

// MARK: Bridge Callbacks (Should be at top of script)

void CASInternal::FillImpressionInfo(int AdType, FCASImpressionInfo &Info) {
    CASUBridge *Bridge = GetBridge();
    Info.RevenueCPM = [Bridge getImpressionCPMFor:AdType];
    Info.Network = FString([Bridge getImpressionNetworkFor:AdType]);
    Info.UnitID = FString([Bridge getImpressionAdUnitFor:AdType]);
    Info.CreativeID = FString([Bridge getImpressionCreativeFor:AdType]);
    Info.TotalImpressions = [Bridge getImpressionDepthFor:AdType];
    Info.LifetimeRevenue = [Bridge getImpressionLifetimeRevenueFor:AdType];
}

void CASInternal::FillInitialConfig(FCASInitialConfig &Config) {
    CASUBridge *Bridge = GetBridge();
    Config.Error = FString([Bridge getInitializationError]);
    Config.CountryCode = FString([Bridge getUserCountryCode]);
    Config.UserConsentStatus = static_cast<ECASUserConsentStatus>([Bridge getUserConsentForAds]);
}

void CASInternal::ShowConsentFlow(bool ifRequired) {
    UIViewController *Controller = [IOSAppDelegate GetDelegate].IOSController;
    [GetBridge() showConsentFlowIfRequired:ifRequired controller:Controller];
}

extern "C" void CASUEventCallback(int adType, int callback, int error) {
    AsyncTask(ENamedThreads::GameThread, [=]() { CASInternal::HandleEventCallback(adType, callback, error); });
}

// MARK: Mobile Ads implementation

bool UCASMobileAds::IsInitializedAds() { return [GetBridge() isInitializedAds]; }

void UCASMobileAds::InitializeMobileAds() {
    if (IsInitializedAds()) {
        CASUEventCallback(kCASUType_MANAGER, kCASUCallback_INITIALIZED, 0);
        return;
    }

    CAS_LOG_W("Initialize Ads");

    [GetBridge() initializeAds];
}

FString UCASMobileAds::GetMobileAdsVersion() { return FString([GetBridge() getMobileAdsVersion]); }

void UCASMobileAds::ValidateAdsIntegration() { [GetBridge() validateAdsIntegration]; }

void UCASMobileAds::SetVerboseAdsLogs(bool Enabled) { [GetBridge() setVerboseAdsLogs:Enabled]; }

void UCASMobileAds::SetAdsMuted(bool Mute) { [GetBridge() setAdsMuted:Mute]; }

void UCASMobileAds::SetTrialAdFreeInterval(int Interval) { [GetBridge() setTrialAdFreeInterval:Interval]; }

void UCASMobileAds::SetUserAudienceForAds(ECASAudience Audience) {
    [GetBridge() setUserAudienceForAds:static_cast<int>(Audience)];
}

void UCASMobileAds::SetUserConsentForAds(ECASUserConsentStatus ConsentStatus) {
    [GetBridge() setUserConsentForAds:static_cast<int>(ConsentStatus)];
}

void UCASMobileAds::SetUserOptOutSaleForAds(ECASUserCCPAStatus CCPAStatus) {
    [GetBridge() setUserOptOutSaleForAds:static_cast<int>(CCPAStatus)];
}

void UCASMobileAds::SetUserAgeForAds(int UserAge) { [GetBridge() setUserAgeForAds:UserAge]; }

void UCASMobileAds::SetUserGenderForAds(ECASGender UserGender) {
    [GetBridge() setUserGenderForAds:static_cast<int>(UserGender)];
}

void UCASMobileAds::SetUserLocationCollectionForAds(bool Enabled) {
    [GetBridge() setUserLocationCollectionForAds:Enabled];
}

void UCASMobileAds::SetMetaDataProcessingOptions(ECASUserCCPAStatus MetaOptions) {
    [GetBridge() setMetaDataProcessingOptions:static_cast<int>(MetaOptions)];
}

// MARK: Banner Ads

bool UCASMobileAds::IsBannerAdReady() { return [GetBridge() isBannerAdReady]; }

void UCASMobileAds::LoadBannerAd(ECASBannerSize AdSize) {
    UIView *MainView = (UIView *)[IOSAppDelegate GetDelegate].IOSView;
    [GetBridge() loadBannerAd:static_cast<int>(AdSize) mainView:MainView];
}

void UCASMobileAds::ShowBannerAd() { [GetBridge() showBannerAd]; }

void UCASMobileAds::HideBannerAd() { [GetBridge() hideBannerAd]; }

void UCASMobileAds::SetBannerAdPosition(ECASPosition AdPosition) {
    [GetBridge() setBannerAdPosition:static_cast<int>(AdPosition) withX:0 withY:0];
}

void UCASMobileAds::SetBannerAdRefreshInterval(int Interval) {
    [GetBridge() setBannerAdRefreshInterval:Interval];
}

void UCASMobileAds::DestroyBannerAd() { [GetBridge() destroyBannerAd]; }

// MARK: MRec Ads

bool UCASMobileAds::IsMRecAdReady() { return [GetBridge() isMRecAdReady]; }

void UCASMobileAds::LoadMRecAd() {
    UIView *MainView = (UIView *)[IOSAppDelegate GetDelegate].IOSView;
    [GetBridge() loadMRecAd:MainView];
}

void UCASMobileAds::ShowMRecAd() { [GetBridge() showMRecAd]; }

void UCASMobileAds::HideMRecAd() { [GetBridge() hideMRecAd]; }

void UCASMobileAds::SetMRecAdPosition(ECASPosition AdPosition) {
    [GetBridge() setMRecAdPosition:static_cast<int>(AdPosition) withX:0 withY:0];
}

void UCASMobileAds::SetMRecAdRefreshInterval(int Interval) { [GetBridge() setMRecAdRefreshInterval:Interval]; }

void UCASMobileAds::DestroyMRecAd() { [GetBridge() destroyMRecAd]; }

// MARK: Interstitial Ads

void UCASMobileAds::LoadInterstitialAd() { [GetBridge() loadInterstitialAd]; }

bool UCASMobileAds::IsInterstitialAdReady() { return [GetBridge() isInterstitialAdReady]; }

void UCASMobileAds::ShowInterstitialAd() {
    UIViewController *Controller = [IOSAppDelegate GetDelegate].IOSController;
    [GetBridge() showInterstitialAd:Controller];
}

void UCASMobileAds::SetInterstitialAdMinimumInterval(int Interval) {
    [GetBridge() setInterstitialAdMinimumInterval:(int)Interval];
}

void UCASMobileAds::RestartInterstitialAdInterval() { [GetBridge() restartInterstitialAdInterval]; }

// MARK: Rewarded Ads

void UCASMobileAds::LoadRewardedAd() { [GetBridge() loadRewardedAd]; }

bool UCASMobileAds::IsRewardedAdReady() { return [GetBridge() isRewardedAdReady]; }

void UCASMobileAds::ShowRewardedAd() {
    UIViewController *Controller = [IOSAppDelegate GetDelegate].IOSController;
    [GetBridge() showRewardedAd:Controller];
}

// MARK: Return To App Ads

void UCASMobileAds::ShowAdOnReturnToApp() { [GetBridge() showAdOnReturnToApp]; }

void UCASMobileAds::SkipAdOnNextReturnToApp() { [GetBridge() skipAdOnNextReturnToApp]; }

void UCASMobileAds::DestroyReturnToAppAd() {
    CASInternal::KeepProxy(kCASUType_APP_RETURN, nullptr);
    [GetBridge() destroyReturnToAppAd];
}

#endif