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

// MARK: Bridge Callbacks (Should be at top of script)

void CASInternal::FillImpressionInfo(int AdType, FCASImpressionInfo &Info) {
    CASUBridge *bridge = [CASUBridge shared];
    Info.RevenueCPM = [bridge getImpressionCPMFor:AdType];
    Info.Network = FString([bridge getImpressionNetworkFor:AdType]);
    Info.UnitID = FString([bridge getImpressionAdUnitFor:AdType]);
    Info.CreativeID = FString([bridge getImpressionCreativeFor:AdType]);
    Info.TotalImpressions = [bridge getImpressionDepthFor:AdType];
    Info.LifetimeRevenue = [bridge getImpressionLifetimeRevenueFor:AdType];
}

void CASInternal::FillInitialConfig(FCASInitialConfig &Config) {
    CASUBridge *bridge = [CASUBridge shared];
    Config.Error = FString([bridge getInitializationError]);
    Config.CountryCode = FString([bridge getUserCountryCode]);
    Config.UserConsentStatus = static_cast<ECASUserConsentStatus>([bridge getUserConsentForAds]);
}

extern "C" void CASUEventCallback(int adType, int callback, int error) {
    AsyncTask(ENamedThreads::GameThread, [=]() { CASInternal::HandleEventCallback(adType, callback, error); });
}

// MARK: Mobile Ads implementation

bool UCASMobileAds::IsInitializedAds() { return [[CASUBridge shared] isInitializedAds]; }

void UCASMobileAds::InitializeMobileAds() {
    if (IsInitializedAds()) {
        CASUEventCallback(kCASUType_MANAGER, kCASUCallback_INITIALIZED, 0);
        return;
    }
    CASUBridge *bridge = [CASUBridge shared];
    if ([bridge isInitializedPlugin]) {
        // In initiailization progress
        return;
    }

    const UCASDefaultConfig *defaultConfig = GetDefault<UCASDefaultConfig>();

    CAS_LOG_W("Initialize with id: %s", *defaultConfig->CASAppID);

    if (defaultConfig->Audience != ECASAudience::Undefined) {
        SetUserAudienceForAds(defaultConfig->Audience);
    }

    TArray<FString> TestDeviceIds = defaultConfig->TestDeviceIds;
    if (TestDeviceIds.Num()) {
        NSMutableArray<NSString *> *NewArray = [NSMutableArray arrayWithCapacity:TestDeviceIds.Num()];
        for (const FString &Element : TestDeviceIds) {
            [NewArray addObject:Element.GetNSString()];
        }
        [bridge setTestDeviceIds:NewArray];
    }

    UIViewController *controller = [IOSAppDelegate GetDelegate].IOSController;

    [bridge setConsentFlowEnabled:defaultConfig->AutoConsentFlow
                       privacyURL:defaultConfig->PrivacyPolicyURL.GetNSString()
                       controller:controller];

    // Framework Unreal info
    const FEngineVersion &EngineVersion = FEngineVersion::Current();
    const FString VersionString =
        FString::Printf(TEXT("%u.%u.%u"), EngineVersion.GetMajor(), EngineVersion.GetMinor(), EngineVersion.GetPatch());
    [bridge setUnrealVersion:VersionString.GetNSString()];

    [bridge setMetaAdvertiserTrackingEnabled:defaultConfig->UseAdvertisingId];

#if !UE_BUILD_SHIPPING
    if (defaultConfig->TestAdsMode) {
        [bridge setTestAdModeForInit];
    }
#endif

    [bridge initializeWithCASId:defaultConfig->CASAppID.GetNSString()
                      useBanner:defaultConfig->AutoloadBannerAds
                        useMRec:defaultConfig->AutoloadMRecAds
                       useInter:defaultConfig->AutoloadInterstitialAds
                      useReward:defaultConfig->AutoloadRewardedAds];
}

FString UCASMobileAds::GetMobileAdsVersion() { return FString([[CASUBridge shared] getMobileAdsVersion]); }

void UCASMobileAds::ValidateAdsIntegration() { [[CASUBridge shared] validateAdsIntegration]; }

void UCASMobileAds::SetVerboseAdsLogs(bool Enabled) { [[CASUBridge shared] setVerboseAdsLogs:Enabled]; }

void UCASMobileAds::SetAdsMuted(bool Mute) { [[CASUBridge shared] setAdsMuted:Mute]; }

void UCASMobileAds::SetTrialAdFreeInterval(int Interval) { [[CASUBridge shared] setTrialAdFreeInterval:Interval]; }

void UCASMobileAds::ShowAdConsentFlow() {
    const UCASDefaultConfig *defaultConfig = GetDefault<UCASDefaultConfig>();
    UIViewController *controller = [IOSAppDelegate GetDelegate].IOSController;
    [[CASUBridge shared] showConsentFlowWithPrivacyURL:defaultConfig->PrivacyPolicyURL.GetNSString()
                                            controller:controller];
}

void UCASMobileAds::SetUserAudienceForAds(ECASAudience Audience) {
    [[CASUBridge shared] setUserAudienceForAds:static_cast<int>(Audience)];
}

void UCASMobileAds::SetUserConsentForAds(ECASUserConsentStatus ConsentStatus) {
    [[CASUBridge shared] setUserConsentForAds:static_cast<int>(ConsentStatus)];
}

void UCASMobileAds::SetUserOptOutSaleForAds(ECASUserCCPAStatus CCPAStatus) {
    [[CASUBridge shared] setUserOptOutSaleForAds:static_cast<int>(CCPAStatus)];
}

void UCASMobileAds::SetUserAgeForAds(int UserAge) { [[CASUBridge shared] setUserAgeForAds:UserAge]; }

void UCASMobileAds::SetUserGenderForAds(ECASGender UserGender) {
    [[CASUBridge shared] setUserGenderForAds:static_cast<int>(UserGender)];
}

void UCASMobileAds::SetMetaDataProcessingOptions(ECASUserCCPAStatus MetaOptions) {
    [[CASUBridge shared] setMetaDataProcessingOptions:static_cast<int>(MetaOptions)];
}

// MARK: Banner Ads

bool UCASMobileAds::IsBannerAdReady() { return [[CASUBridge shared] isBannerAdReady]; }

void UCASMobileAds::LoadBannerAd(ECASBannerSize AdSize) {
    UIView *MainView = (UIView *)[IOSAppDelegate GetDelegate].IOSView;
    [[CASUBridge shared] loadBannerAd:static_cast<int>(AdSize) mainView:MainView];
}

void UCASMobileAds::ShowBannerAd() { [[CASUBridge shared] showBannerAd]; }

void UCASMobileAds::HideBannerAd() { [[CASUBridge shared] hideBannerAd]; }

void UCASMobileAds::SetBannerAdPosition(ECASPosition AdPosition) {
    [[CASUBridge shared] setBannerAdPosition:static_cast<int>(AdPosition) withX:0 withY:0];
}

void UCASMobileAds::SetBannerAdRefreshInterval(int Interval) {
    [[CASUBridge shared] setBannerAdRefreshInterval:Interval];
}

void UCASMobileAds::DestroyBannerAd() { [[CASUBridge shared] destroyBannerAd]; }

// MARK: MRec Ads

bool UCASMobileAds::IsMRecAdReady() { return [[CASUBridge shared] isMRecAdReady]; }

void UCASMobileAds::LoadMRecAd() {
    UIView *MainView = (UIView *)[IOSAppDelegate GetDelegate].IOSView;
    [[CASUBridge shared] loadMRecAd:MainView];
}

void UCASMobileAds::ShowMRecAd() { [[CASUBridge shared] showMRecAd]; }

void UCASMobileAds::HideMRecAd() { [[CASUBridge shared] hideMRecAd]; }

void UCASMobileAds::SetMRecAdPosition(ECASPosition AdPosition) {
    [[CASUBridge shared] setMRecAdPosition:static_cast<int>(AdPosition) withX:0 withY:0];
}

void UCASMobileAds::SetMRecAdRefreshInterval(int Interval) { [[CASUBridge shared] setMRecAdRefreshInterval:Interval]; }

void UCASMobileAds::DestroyMRecAd() { [[CASUBridge shared] destroyMRecAd]; }

// MARK: Interstitial Ads

void UCASMobileAds::LoadInterstitialAd() { [[CASUBridge shared] loadInterstitialAd]; }

bool UCASMobileAds::IsInterstitialAdReady() { return [[CASUBridge shared] isInterstitialAdReady]; }

void UCASMobileAds::ShowInterstitialAd() {
    UIViewController *controller = [IOSAppDelegate GetDelegate].IOSController;
    [[CASUBridge shared] showInterstitialAd:controller];
}

void UCASMobileAds::SetInterstitialAdMinimumInterval(int Interval) {
    [[CASUBridge shared] setInterstitialAdMinimumInterval:(int)Interval];
}

void UCASMobileAds::RestartInterstitialAdInterval() { [[CASUBridge shared] restartInterstitialAdInterval]; }

// MARK: Rewarded Ads

void UCASMobileAds::LoadRewardedAd() { [[CASUBridge shared] loadRewardedAd]; }

bool UCASMobileAds::IsRewardedAdReady() { return [[CASUBridge shared] isRewardedAdReady]; }

void UCASMobileAds::ShowRewardedAd() {
    UIViewController *controller = [IOSAppDelegate GetDelegate].IOSController;
    [[CASUBridge shared] showRewardedAd:controller];
}

// MARK: Return To App Ads

void UCASMobileAds::ShowAdOnReturnToApp() { [[CASUBridge shared] showAdOnReturnToApp]; }

void UCASMobileAds::SkipAdOnNextReturnToApp() { [[CASUBridge shared] skipAdOnNextReturnToApp]; }

void UCASMobileAds::DestroyReturnToAppAd() {
    CASInternal::KeepProxy(kCASUType_APP_RETURN, nullptr);
    [[CASUBridge shared] destroyReturnToAppAd];
}

#endif