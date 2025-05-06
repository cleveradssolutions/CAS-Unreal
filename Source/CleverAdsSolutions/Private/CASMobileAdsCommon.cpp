// Copyright CleverAdsSolutions LTD, CAS.AI. All Rights Reserved.

#include "CASMobileAds.h"

#include "Async/Async.h"
#include "CASDefines.h"
#include "CASImpressionInfo.h"
#include "CASInternal.h"

#if !PLATFORM_ANDROID && !PLATFORM_IOS
#include "CASConfigContainer.h"
#include "Interfaces/IPluginManager.h"
#endif

// MARK: Multiplatform
FOnCASInitializedDelegate UCASMobileAds::OnAdsInitialized;
FOnCASCallbackDelegate UCASMobileAds::OnConsentFlowDismissed;
FOnCASConsentFlowCallbackDelegate UCASMobileAds::OnConsentFlowResult;

FOnCASImpressionDelegate UCASMobileAds::OnAdsImpression;

FOnCASCallbackDelegate UCASMobileAds::OnBannerAdLoaded;
FOnCASFailedDelegate UCASMobileAds::OnBannerAdFailed;
FOnCASCallbackDelegate UCASMobileAds::OnBannerAdClicked;

FOnCASCallbackDelegate UCASMobileAds::OnMRecAdLoaded;
FOnCASFailedDelegate UCASMobileAds::OnMRecAdFailed;
FOnCASCallbackDelegate UCASMobileAds::OnMRecAdClicked;

FOnCASCallbackDelegate UCASMobileAds::OnInterstitialAdLoaded;
FOnCASFailedDelegate UCASMobileAds::OnInterstitialAdLoadFailed;
FOnCASFailedDelegate UCASMobileAds::OnInterstitialAdShowFailed;
FOnCASCallbackDelegate UCASMobileAds::OnInterstitialAdDisplayed;
FOnCASCallbackDelegate UCASMobileAds::OnInterstitialAdClicked;
FOnCASCallbackDelegate UCASMobileAds::OnInterstitialAdDismissed;

FOnCASCallbackDelegate UCASMobileAds::OnRewardedAdLoaded;
FOnCASFailedDelegate UCASMobileAds::OnRewardedAdLoadFailed;
FOnCASCallbackDelegate UCASMobileAds::OnRewardedAdEarnedReward;
FOnCASFailedDelegate UCASMobileAds::OnRewardedAdShowFailed;
FOnCASCallbackDelegate UCASMobileAds::OnRewardedAdDisplayed;
FOnCASCallbackDelegate UCASMobileAds::OnRewardedAdClicked;
FOnCASCallbackDelegate UCASMobileAds::OnRewardedAdDismissed;

FOnCASCallbackDelegate UCASMobileAds::OnAppOpenAdLoaded;
FOnCASFailedDelegate UCASMobileAds::OnAppOpenAdLoadFailed;
FOnCASFailedDelegate UCASMobileAds::OnAppOpenAdShowFailed;
FOnCASCallbackDelegate UCASMobileAds::OnAppOpenAdDisplayed;
FOnCASCallbackDelegate UCASMobileAds::OnAppOpenAdClicked;
FOnCASCallbackDelegate UCASMobileAds::OnAppOpenAdDismissed;

FOnCASFailedDelegate UCASMobileAds::OnReturnToAppAdShowFailed;
FOnCASCallbackDelegate UCASMobileAds::OnReturnToAppAdDisplayed;
FOnCASCallbackDelegate UCASMobileAds::OnReturnToAppAdClicked;
FOnCASCallbackDelegate UCASMobileAds::OnReturnToAppAdDismissed;

void UCASMobileAds::ShowAdConsentFlow() {
    CASInternal::ShowConsentFlow(false);
}

void UCASMobileAds::ShowAdConsentFlowIfRequired() {
    CASInternal::ShowConsentFlow(true);
}

void UCASMobileAds::BindEventToOnAdsImpression(
    const FCASImpressionDelegate &OnImpression) {
    CASInternal::OnAdsImpression.AddUnique(OnImpression);
}

void UCASMobileAds::UnbindEventFromOnAdsImpression(
    const FCASImpressionDelegate &OnImpression) {
    CASInternal::OnAdsImpression.Remove(OnImpression);
}

void UCASMobileAds::UnbindAllEventsFromOnAdsImpression() {
    CASInternal::OnAdsImpression.Clear();
}

FString UCASMobileAds::GetAdsErrorMessage(ECASError Error) {
    switch (Error) {
        case ECASError::NoConnection:
            return TEXT("No internet connection");

        case ECASError::Rejected:
            return TEXT("Device rejected");

        case ECASError::Timeout:
            return TEXT("Timeout");

        case ECASError::NoFill:
            return TEXT("No Fill");

        case ECASError::Configuration:
            return TEXT("Invalid configuration");

        case ECASError::NotReady:
            return TEXT("Ad are not ready");

        case ECASError::NotInitialized:
            return TEXT("Not initialized");

        case ECASError::ReachedCap:
            return TEXT("Reached cap for user");

        case ECASError::IntervalNotYetPassed:
            return TEXT("Interval has not yet passed");

        case ECASError::AlreadyDisplayed:
            return TEXT("Ad already displayed");

        case ECASError::AppNotForeground:
            return TEXT("App not foreground");

        default:
            return TEXT("Internal error");
    }
}

// MARK: Return To App Ads

void UCASMobileAds::ShowAdOnReturnToApp() {
    UCASMobileAds::AutoshowInterstitialAd();
}

void UCASMobileAds::SkipAdOnNextReturnToApp() {
}

void UCASMobileAds::DestroyReturnToAppAd() {
    CASInternal::KeepProxy(kCASUType_APP_RETURN, nullptr);
    UCASMobileAds::DisableAutoshowInterstitialAd();
}

// MARK: Editor Impl
#if !PLATFORM_ANDROID && !PLATFORM_IOS

bool Initialized = false;
bool ReadyBanner = false;
bool ReadyMREC = false;
bool ReadyInter = false;
bool ReadyReward = false;
bool ReadyAppOpen = false;

bool AutoloadBannerAds = false;
bool AutoloadMRecAds = false;
bool AutoloadInterstitialAds = false;
bool AutoloadRewardedAds = false;
bool AutoloadAppOpenAds = false;

void CASInternal::FillImpressionInfo(int AdType, FCASImpressionInfo &Info) {
    static int TotalImpressions = 0;
    static float LifetimeRevenue = 0.f;

    TotalImpressions++;
    LifetimeRevenue += 1.0f;

    Info.RevenueCPM = 1.0f;
    Info.Network = TEXT("CrossPromotion");
    Info.CreativeID = TEXT("TextCreativeIdHash");
    Info.UnitID = TEXT("0");
    Info.TotalImpressions = TotalImpressions;
    Info.LifetimeRevenue = LifetimeRevenue;
}

void CASInternal::FillInitialConfig(FCASInitialConfig &Config) {
    Config.Error = TEXT("");
    Config.CountryCode = TEXT("");
    Config.UserConsentStatus = ECASUserConsentStatus::Undefined;
}

void CASUEventCallback(int adType, int callback, int error) {
    AsyncTask(ENamedThreads::GameThread, [ = ]() {
        CASInternal::HandleEventCallback(adType, callback, error);
    });
}

void CASInternal::ShowConsentFlow(bool ifRequired) {
    CAS_LOG_D("Manual Show Consent Flow with Privacy Policy: %s",
              *UCASConfigContainer::GetConfig()->PrivacyPolicyURL);
    CASUEventCallback(kCASUType_MANAGER, kCASUCallback_CONSENT_FLOW_DISMISSED,
                      static_cast<int>(ECASConsentFlowStatus::Unavailable));
}

// MARK: General
void UCASMobileAds::SetInitializationAdsExtras(FString key, FString value) {
}

bool UCASMobileAds::IsInitializedAds() {
    return Initialized;
}

void UCASMobileAds::InitializeMobileAds() {
    bool notInitialized = !Initialized;

    Initialized = true;

    if (notInitialized) {
        const UCASDefaultConfig *DefaultConfig = UCASConfigContainer::GetConfig();
        CAS_LOG_D("Initialize Ads for %s", *DefaultConfig->CASAppID);

        AutoloadBannerAds = DefaultConfig->AutoloadBannerAds;
        AutoloadMRecAds = DefaultConfig->AutoloadMRecAds;
        AutoloadInterstitialAds = DefaultConfig->AutoloadInterstitialAds;
        AutoloadRewardedAds = DefaultConfig->AutoloadRewardedAds;
        AutoloadAppOpenAds = DefaultConfig->AutoloadAppOpenAds;
    }

    CASUEventCallback(kCASUType_MANAGER, kCASUCallback_INITIALIZED, 0);

    if (notInitialized) {
        if (AutoloadInterstitialAds) {
            ReadyInter = true;
            CASUEventCallback(kCASUType_INTER, kCASUCallback_LOADED, 0);
        }

        if (AutoloadRewardedAds) {
            ReadyReward = true;
            CASUEventCallback(kCASUType_REWARD, kCASUCallback_LOADED, 0);
        }

        if (AutoloadAppOpenAds) {
            ReadyAppOpen = true;
            CASUEventCallback(kCASUType_APP_OPEN, kCASUCallback_LOADED, 0);
        }
    }
}

FString UCASMobileAds::GetMobileAdsVersion() {
    TSharedPtr<IPlugin> Plugin =
        IPluginManager::Get().FindPlugin("CleverAdsSolutions");

    return Plugin->GetDescriptor().VersionName;
}

void UCASMobileAds::ValidateAdsIntegration() {
}

void UCASMobileAds::SetVerboseAdsLogs(bool Enabled) {
    CAS_LOG_D("Setting Verbose Ads Logs to %s", CAS_BOOL_TO_STR(Enabled));
}

void UCASMobileAds::SetAdsMuted(bool Mute) {
    CAS_LOG_D("Setting Mute Ad sounds to %s", CAS_BOOL_TO_STR(Mute))
}

void UCASMobileAds::SetTrialAdFreeInterval(int interval) {
}

bool UCASMobileAds::IsUserAdConsentRequired() {
    const UCASDefaultConfig *DefaultConfig = UCASConfigContainer::GetConfig();

    return DefaultConfig->UserDebugGeography == ECASUserDebugGeography::EEA;
}

void UCASMobileAds::SetUserAudienceForAds(ECASAudience Audience) {
    FString AudienceStr = UEnum::GetValueAsString(Audience);

    CAS_LOG_D("Setting User Audience to %s", *AudienceStr);
}

void UCASMobileAds::SetUserConsentForAds(ECASUserConsentStatus ConsentStatus) {
    FString GDPRStr = UEnum::GetValueAsString(ConsentStatus);

    CAS_LOG_D("Setting User GDPR status to %s", *GDPRStr);
}

void UCASMobileAds::SetUserOptOutSaleForAds(ECASUserCCPAStatus CCPAStatus) {
    FString CCPAStr = UEnum::GetValueAsString(CCPAStatus);

    CAS_LOG_D("Setting User CCPA status to %s", *CCPAStr);
}

void UCASMobileAds::SetUserIDForAds(FString UserID) {
    CAS_LOG_D("Setting User ID to %s", *UserID);
}

void UCASMobileAds::SetUserAgeForAds(int UserAge) {
    CAS_LOG_D("Setting User Age to %d", UserAge);
}

void UCASMobileAds::SetUserGenderForAds(ECASGender UserGender) {
    FString GenderStr = UEnum::GetValueAsString(UserGender);

    CAS_LOG_D("Setting User Gender to %s", *GenderStr);
}

void UCASMobileAds::SetUserLocationCollectionForAds(bool Enabled) {
    CAS_LOG_D("Setting User Location collection to %s", CAS_BOOL_TO_STR(Enabled));
}

void UCASMobileAds::SetMetaDataProcessingOptions(
    ECASUserCCPAStatus MetaOptions) {
    FString OptionsStr = UEnum::GetValueAsString(MetaOptions);

    CAS_LOG_D("Setting Meta Audience Network DataProcessingOptions to %s",
              *OptionsStr);
}

// MARK: Banner Ads

bool UCASMobileAds::IsBannerAdReady() {
    return ReadyBanner;
}

void UCASMobileAds::LoadBannerAd(ECASBannerSize AdSize) {
    FString SizeString = UEnum::GetValueAsString(AdSize);

    CAS_LOG_D("Loading Banner ad with size (%s)", *SizeString);
    ReadyBanner = Initialized;

    if (ReadyBanner) {
        CASUEventCallback(kCASUType_BANNER, kCASUCallback_LOADED, 0);
    } else {
        CASUEventCallback(kCASUType_BANNER, kCASUCallback_LOAD_FAILED,
                          static_cast<int>(ECASError::NotInitialized));
    }
}

void UCASMobileAds::SetAutoloadBannerAd(bool Enabled) {
    AutoloadBannerAds = Enabled;
}

void UCASMobileAds::ShowBannerAd() {
    CAS_LOG_D("Showing Banner ad");
    int AdType = kCASUType_BANNER;

    if (Initialized) {
        if (ReadyBanner) {
            CASUEventCallback(AdType, kCASUCallback_PAY, 0);
            CASUEventCallback(AdType, kCASUCallback_CLICKED, 0);
        } else {
            CASUEventCallback(AdType, kCASUCallback_LOAD_FAILED,
                              static_cast<int>(ECASError::NotReady));
        }
    } else {
        CASUEventCallback(AdType, kCASUCallback_LOAD_FAILED,
                          static_cast<int>(ECASError::NotInitialized));
    }
}

void UCASMobileAds::HideBannerAd() {
    CAS_LOG_D("Hiding Banner ad");
}

void UCASMobileAds::SetBannerAdPosition(ECASPosition AdPosition) {
    FString PositionString = UEnum::GetValueAsString(AdPosition);

    CAS_LOG_D("Setting Banner ad position to %s", *PositionString);
}

void UCASMobileAds::SetBannerAdRefreshInterval(int Interval) {
    CAS_LOG_D("Setting Banner ad refresh interval to %d", Interval);
}

void UCASMobileAds::DestroyBannerAd() {
    CAS_LOG_D("Destroying Banner");
    ReadyBanner = false;
}

// MARK: MRec Ads

bool UCASMobileAds::IsMRecAdReady() {
    return ReadyMREC;
}

void UCASMobileAds::LoadMRecAd() {
    CAS_LOG_D("Loading MRec ad");
    ReadyMREC = Initialized;

    if (ReadyMREC) {
        CASUEventCallback(kCASUType_MREC, kCASUCallback_LOADED, 0);
    } else {
        CASUEventCallback(kCASUType_MREC, kCASUCallback_LOAD_FAILED,
                          static_cast<int>(ECASError::NotInitialized));
    }
}

void UCASMobileAds::SetAutoloadMRecAd(bool Enabled) {
    AutoloadMRecAds = Enabled;
}

void UCASMobileAds::ShowMRecAd() {
    CAS_LOG_D("Showing MRec ad");
    int AdType = kCASUType_MREC;

    if (Initialized) {
        if (ReadyMREC) {
            CASUEventCallback(AdType, kCASUCallback_PAY, 0);
            CASUEventCallback(AdType, kCASUCallback_CLICKED, 0);
        } else {
            CASUEventCallback(AdType, kCASUCallback_LOAD_FAILED,
                              static_cast<int>(ECASError::NotReady));
        }
    } else {
        CASUEventCallback(AdType, kCASUCallback_LOAD_FAILED,
                          static_cast<int>(ECASError::NotInitialized));
    }
}

void UCASMobileAds::HideMRecAd() {
    CAS_LOG_D("Hiding Banner ad");
}

void UCASMobileAds::SetMRecAdPosition(ECASPosition AdPosition) {
    FString PositionString = UEnum::GetValueAsString(AdPosition);

    CAS_LOG_D("Setting MRec ad position to %s", *PositionString);
}

void UCASMobileAds::SetMRecAdRefreshInterval(int Interval) {
    CAS_LOG_D("Setting MRec ad refresh interval to %d", Interval);
}

void UCASMobileAds::DestroyMRecAd() {
    CAS_LOG_D("Destroying MRec");
    ReadyMREC = false;
}

// MARK: Interstitial Ads

void UCASMobileAds::LoadInterstitialAd() {
    CAS_LOG_D("Loading Interstitial ad");
    ReadyInter = Initialized;

    if (ReadyInter) {
        CASUEventCallback(kCASUType_INTER, kCASUCallback_LOADED, 0);
    } else {
        CASUEventCallback(kCASUType_INTER, kCASUCallback_LOAD_FAILED,
                          static_cast<int>(ECASError::NotInitialized));
    }
}

void UCASMobileAds::SetAutoloadInterstitialAd(bool Enabled) {
    AutoloadInterstitialAds = Enabled;
}

bool UCASMobileAds::IsInterstitialAdReady() {
    return ReadyInter;
}

void UCASMobileAds::ShowInterstitialAd() {
    CAS_LOG_D("Showing Interstitial ad");
    int AdType = kCASUType_INTER;

    if (Initialized) {
        if (ReadyInter) {
            CASUEventCallback(AdType, kCASUCallback_SHOW, 0);
            CASUEventCallback(AdType, kCASUCallback_PAY, 0);
            CASUEventCallback(AdType, kCASUCallback_CLICKED, 0);
            CASUEventCallback(AdType, kCASUCallback_CLOSED, 0);

            if (AutoloadInterstitialAds) {
                CASUEventCallback(AdType, kCASUCallback_LOADED, 0);
            } else {
                ReadyInter = false;
            }
        } else {
            CASUEventCallback(AdType, kCASUCallback_SHOW_FAILED,
                              static_cast<int>(ECASError::NotReady));
            CASUEventCallback(AdType, kCASUCallback_CLOSED, 0);
        }
    } else {
        CASUEventCallback(AdType, kCASUCallback_SHOW_FAILED,
                          static_cast<int>(ECASError::NotInitialized));
        CASUEventCallback(AdType, kCASUCallback_CLOSED, 0);
    }
}

void UCASMobileAds::AutoshowInterstitialAd() {
    CAS_LOG_D("Enable Interstitial ad impression on Return To App Ad. Not "
              "supported in Editor.");
}

void UCASMobileAds::DisableAutoshowInterstitialAd() {
    CAS_LOG_D("Disable Interstitial ad impression on Return To App Ad. Not "
              "supported in Editor.");
}

void UCASMobileAds::SetInterstitialAdMinimumInterval(int Interval) {
    CAS_LOG_D("Setting Interstitial ad interval to %d", Interval);
}

void UCASMobileAds::RestartInterstitialAdInterval() {
    CAS_LOG_D("Interstitial ad interval is restarted");
}

void UCASMobileAds::DestroyInterstitialAd() {
    CAS_LOG_D("Destroying Interstitial ad");
    ReadyInter = false;
}

// MARK: Rewarded Ads

void UCASMobileAds::LoadRewardedAd() {
    CAS_LOG_D("Loading Rewarded ad");
    ReadyReward = Initialized;

    if (ReadyReward) {
        CASUEventCallback(kCASUType_REWARD, kCASUCallback_LOADED, 0);
    } else {
        CASUEventCallback(kCASUType_REWARD, kCASUCallback_LOAD_FAILED,
                          static_cast<int>(ECASError::NotInitialized));
    }
}

void UCASMobileAds::SetAutoloadRewardedAd(bool Enabled) {
    AutoloadRewardedAds = Enabled;
}

bool UCASMobileAds::IsRewardedAdReady() {
    return ReadyReward;
}

void UCASMobileAds::ShowRewardedAd() {
    CAS_LOG_D("Showing Rewarded ad");
    int AdType = kCASUType_REWARD;

    if (Initialized) {
        if (ReadyReward) {
            CASUEventCallback(AdType, kCASUCallback_SHOW, 0);
            CASUEventCallback(AdType, kCASUCallback_PAY, 0);
            CASUEventCallback(AdType, kCASUCallback_CLICKED, 0);
            CASUEventCallback(AdType, kCASUCallback_COMPLETED, 0);
            CASUEventCallback(AdType, kCASUCallback_CLOSED, 0);

            if (AutoloadRewardedAds) {
                CASUEventCallback(AdType, kCASUCallback_LOADED, 0);
            } else {
                ReadyReward = false;
            }
        } else {
            CASUEventCallback(AdType, kCASUCallback_SHOW_FAILED,
                              static_cast<int>(ECASError::NotReady));
            CASUEventCallback(AdType, kCASUCallback_CLOSED, 0);
        }
    } else {
        CASUEventCallback(AdType, kCASUCallback_SHOW_FAILED,
                          static_cast<int>(ECASError::NotInitialized));
        CASUEventCallback(AdType, kCASUCallback_CLOSED, 0);
    }
}

void UCASMobileAds::DestroyRewardedAd() {
    CAS_LOG_D("Destroying Rewarded ad");
    ReadyReward = false;
}

// MARK: AppOpen Ads

void UCASMobileAds::LoadAppOpenAd() {
    CAS_LOG_D("Loading AppOpen ad");
    ReadyAppOpen = Initialized;

    if (ReadyAppOpen) {
        CASUEventCallback(kCASUType_APP_OPEN, kCASUCallback_LOADED, 0);
    } else {
        CASUEventCallback(kCASUType_APP_OPEN, kCASUCallback_LOAD_FAILED,
                          static_cast<int>(ECASError::NotInitialized));
    }
}

void UCASMobileAds::SetAutoloadAppOpenAd(bool Enabled) {
    AutoloadAppOpenAds = Enabled;
}

bool UCASMobileAds::IsAppOpenAdReady() {
    return ReadyAppOpen;
}

void UCASMobileAds::ShowAppOpenAd() {
    CAS_LOG_D("Showing AppOpen ad");
    int AdType = kCASUType_APP_OPEN;

    if (Initialized) {
        if (ReadyAppOpen) {
            CASUEventCallback(AdType, kCASUCallback_SHOW, 0);
            CASUEventCallback(AdType, kCASUCallback_PAY, 0);
            CASUEventCallback(AdType, kCASUCallback_CLICKED, 0);
            CASUEventCallback(AdType, kCASUCallback_CLOSED, 0);

            if (AutoloadAppOpenAds) {
                CASUEventCallback(AdType, kCASUCallback_LOADED, 0);
            } else {
                ReadyAppOpen = false;
            }
        } else {
            CASUEventCallback(AdType, kCASUCallback_SHOW_FAILED,
                              static_cast<int>(ECASError::NotReady));
            CASUEventCallback(AdType, kCASUCallback_CLOSED, 0);
        }
    } else {
        CASUEventCallback(AdType, kCASUCallback_SHOW_FAILED,
                          static_cast<int>(ECASError::NotInitialized));
        CASUEventCallback(AdType, kCASUCallback_CLOSED, 0);
    }
}

void UCASMobileAds::AutoshowAppOpenAd() {
    CAS_LOG_D("Enable AppOpen ad impression on Return To App Ad. Not supported "
              "in Editor.");
}

void UCASMobileAds::DisableAutoshowAppOpenAd() {
    CAS_LOG_D("Disable AppOpen ad impression on Return To App Ad. Not supported "
              "in Editor.");
}

void UCASMobileAds::DestroyAppOpenAd() {
    CAS_LOG_D("Destroying AppOpen ad");
    ReadyAppOpen = false;
}

#endif /* if !PLATFORM_ANDROID && !PLATFORM_IOS */
