// Copyright CleverAdsSolutions LTD, CAS.AI. All Rights Reserved.

#if PLATFORM_ANDROID
#include "CASMobileAds.h"

#include "CASInternal.h"
#include "JavaCASBridgeObject.h"
#include "CASDefaultConfig.h"
#include "Async/Async.h"
#include "Misc/EngineVersion.h"

TSharedPtr<FJavaCASBridgeObject> GetBridge() {
    static TSharedPtr<FJavaCASBridgeObject, ESPMode::ThreadSafe> Instance;
    if (!Instance.IsValid()) {
        Instance = MakeShared<FJavaCASBridgeObject, ESPMode::ThreadSafe>();
    }
    return Instance;
}

// MARK: Bridge Callbacks (Should be at top of script)

void CASInternal::FillImpressionInfo(int AdType, FCASImpressionInfo &Info) {
    TSharedPtr<FJavaCASBridgeObject> Bridge = GetBridge();
    Info.RevenueCPM = Bridge->CallFloatMethod(Bridge->GetImpressionCPM, AdType);
    Info.Network = Bridge->CallMethod<FString>(Bridge->GetImpressionNetwork, AdType);
    Info.UnitID = Bridge->CallMethod<FString>(Bridge->GetImpressionAdUnit, AdType);
    Info.CreativeID = Bridge->CallMethod<FString>(Bridge->GetImpressionCreative, AdType);
    Info.TotalImpressions = Bridge->CallMethod<int>(Bridge->GetImpressionDepth, AdType);
    Info.LifetimeRevenue = Bridge->CallFloatMethod(Bridge->GetImpressionLifetimeRevenue, AdType);
}

void CASInternal::FillInitialConfig(FCASInitialConfig &Config) {
    TSharedPtr<FJavaCASBridgeObject> Bridge = GetBridge();
    Config.Error = Bridge->CallMethod<FString>(Bridge->GetInitializationError);
    Config.CountryCode = Bridge->CallMethod<FString>(Bridge->GetUserCountryCode);
    Config.UserConsentStatus =
        static_cast<ECASUserConsentStatus>(Bridge->CallMethod<int>(Bridge->GetUserConsentForAds));
}

extern "C" JNIEXPORT void JNICALL Java_com_cleveradssolutions_plugin_unreal_CASUEvent_callback(
    JNIEnv *env, jobject thiz, jint adType, jint callback, jint error) {
    const int AdType = (int)adType;
    const int Callback = (int)callback;
    const int Error = (int)error;
    AsyncTask(ENamedThreads::GameThread, [=]() { CASInternal::HandleEventCallback(AdType, Callback, Error); });
}

// MARK: Mobile Ads implementation

bool UCASMobileAds::IsInitializedAds() { return GetBridge()->CallMethod<bool>(GetBridge()->IsInitializedAds); }

void UCASMobileAds::InitializeMobileAds() {
    if (IsInitializedAds()) {
        CASInternal::HandleEventCallback(kCASUType_MANAGER, kCASUCallback_INITIALIZED, 0);
        return;
    }
    TSharedPtr<FJavaCASBridgeObject> Bridge = GetBridge();
    if (Bridge->CallMethod<bool>(Bridge->IsInitializedPlugin)) {
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
        for (const FString &Element : TestDeviceIds) {
            Bridge->CallMethod<void>(Bridge->AddTestDeviceId, *Bridge->GetJString(Element));
        }
    }

    Bridge->CallMethod<void>(Bridge->SetConsentFlowEnabled, defaultConfig->AutoConsentFlow,
                             *Bridge->GetJString(defaultConfig->PrivacyPolicyURL));

    // Framework Unreal info
    const FEngineVersion &EngineVersion = FEngineVersion::Current();
    const FString VersionString =
        FString::Printf(TEXT("%u.%u.%u"), EngineVersion.GetMajor(), EngineVersion.GetMinor(), EngineVersion.GetPatch());
    Bridge->CallMethod<void>(Bridge->SetUnrealVersion, *Bridge->GetJString(VersionString));

#if !UE_BUILD_SHIPPING
    if (defaultConfig->TestAdsMode) {
        Bridge->CallMethod<void>(Bridge->SetTestAdModeForInit);
    }
#endif

    Bridge->CallMethod<void>(Bridge->InitializeMobileAds, *Bridge->GetJString(defaultConfig->CASAppID),
                             defaultConfig->AutoloadBannerAds, defaultConfig->AutoloadMRecAds,
                             defaultConfig->AutoloadInterstitialAds, defaultConfig->AutoloadRewardedAds);
}

FString UCASMobileAds::GetMobileAdsVersion() {
    return GetBridge()->CallMethod<FString>(GetBridge()->GetMobileAdsVersion);
}

void UCASMobileAds::ValidateAdsIntegration() { GetBridge()->CallMethod<void>(GetBridge()->ValidateAdsIntegration); }

void UCASMobileAds::SetVerboseAdsLogs(bool Enabled) {
    GetBridge()->CallMethod<void>(GetBridge()->SetVerboseAdsLogs, Enabled);
}

void UCASMobileAds::SetAdsMuted(bool Mute) { GetBridge()->CallMethod<void>(GetBridge()->SetAdsMuted, Mute); }

void UCASMobileAds::SetTrialAdFreeInterval(int Interval) {
    GetBridge()->CallMethod<void>(GetBridge()->SetTrialAdFreeInterval, Interval);
}

void UCASMobileAds::ShowAdConsentFlow() {
    TSharedPtr<FJavaCASBridgeObject> Bridge = GetBridge();
    const UCASDefaultConfig *defaultConfig = GetDefault<UCASDefaultConfig>();
    Bridge->CallMethod<void>(Bridge->ShowConsentFlow, *Bridge->GetJString(defaultConfig->PrivacyPolicyURL));
}

void UCASMobileAds::SetUserAudienceForAds(ECASAudience Audience) {
    GetBridge()->CallMethod<void>(GetBridge()->SetUserAudienceForAds, static_cast<int>(Audience));
}

void UCASMobileAds::SetUserConsentForAds(ECASUserConsentStatus ConsentStatus) {
    GetBridge()->CallMethod<void>(GetBridge()->SetUserConsentForAds, static_cast<int>(ConsentStatus));
}

void UCASMobileAds::SetUserOptOutSaleForAds(ECASUserCCPAStatus CCPAStatus) {
    GetBridge()->CallMethod<void>(GetBridge()->SetUserOptOutSaleForAds, static_cast<int>(CCPAStatus));
}

void UCASMobileAds::SetUserAgeForAds(int UserAge) {
    GetBridge()->CallMethod<void>(GetBridge()->SetUserAgeForAds, UserAge);
}

void UCASMobileAds::SetUserGenderForAds(ECASGender UserGender) {
    GetBridge()->CallMethod<void>(GetBridge()->SetUserGenderForAds, static_cast<int>(UserGender));
}

void UCASMobileAds::SetMetaDataProcessingOptions(ECASUserCCPAStatus MetaOptions) {
    GetBridge()->CallMethod<void>(GetBridge()->SetMetaDataProcessingOptions, static_cast<int>(MetaOptions));
}

// MARK: Banner Ads

bool UCASMobileAds::IsBannerAdReady() { return GetBridge()->CallMethod<bool>(GetBridge()->IsBannerAdReady); }

void UCASMobileAds::LoadBannerAd(ECASBannerSize AdSize) {
    GetBridge()->CallMethod<void>(GetBridge()->LoadBannerAd, static_cast<int>(AdSize));
}

void UCASMobileAds::ShowBannerAd() { GetBridge()->CallMethod<void>(GetBridge()->ShowBannerAd); }

void UCASMobileAds::HideBannerAd() { GetBridge()->CallMethod<void>(GetBridge()->HideBannerAd); }

void UCASMobileAds::SetBannerAdPosition(ECASPosition AdPosition) {
    GetBridge()->CallMethod<void>(GetBridge()->SetBannerAdPosition, static_cast<int>(AdPosition), 0, 0);
}

void UCASMobileAds::SetBannerAdRefreshInterval(int Interval) {
    GetBridge()->CallMethod<void>(GetBridge()->SetBannerAdRefreshInterval, Interval);
}

void UCASMobileAds::DestroyBannerAd() { GetBridge()->CallMethod<void>(GetBridge()->DestroyBannerAd); }

// MARK: MRec Ads

bool UCASMobileAds::IsMRecAdReady() { return GetBridge()->CallMethod<bool>(GetBridge()->IsMRecAdReady); }

void UCASMobileAds::LoadMRecAd() { GetBridge()->CallMethod<void>(GetBridge()->LoadMRecAd); }

void UCASMobileAds::ShowMRecAd() { GetBridge()->CallMethod<void>(GetBridge()->ShowMRecAd); }

void UCASMobileAds::HideMRecAd() { GetBridge()->CallMethod<void>(GetBridge()->HideMRecAd); }

void UCASMobileAds::SetMRecAdPosition(ECASPosition AdPosition) {
    GetBridge()->CallMethod<void>(GetBridge()->SetMRecAdPosition, static_cast<int>(AdPosition), 0, 0);
}

void UCASMobileAds::SetMRecAdRefreshInterval(int Interval) {
    GetBridge()->CallMethod<void>(GetBridge()->SetMRecAdRefreshInterval, Interval);
}

void UCASMobileAds::DestroyMRecAd() { GetBridge()->CallMethod<void>(GetBridge()->DestroyMRecAd); }

// MARK: Interstitial Ads

void UCASMobileAds::LoadInterstitialAd() { GetBridge()->CallMethod<void>(GetBridge()->LoadInterstitialAd); }

bool UCASMobileAds::IsInterstitialAdReady() {
    return GetBridge()->CallMethod<bool>(GetBridge()->IsInterstitialAdReady);
}

void UCASMobileAds::ShowInterstitialAd() { GetBridge()->CallMethod<void>(GetBridge()->ShowInterstitialAd); }

void UCASMobileAds::SetInterstitialAdMinimumInterval(int Interval) {
    GetBridge()->CallMethod<void>(GetBridge()->SetInterstitialAdMinimumInterval, Interval);
}

void UCASMobileAds::RestartInterstitialAdInterval() {
    GetBridge()->CallMethod<void>(GetBridge()->RestartInterstitialAdInterval);
}

// MARK: Rewarded Ads

void UCASMobileAds::LoadRewardedAd() { GetBridge()->CallMethod<void>(GetBridge()->LoadRewardedAd); }

bool UCASMobileAds::IsRewardedAdReady() { return GetBridge()->CallMethod<bool>(GetBridge()->IsRewardedAdReady); }

void UCASMobileAds::ShowRewardedAd() { GetBridge()->CallMethod<void>(GetBridge()->ShowRewardedAd); }

// MARK: Return To App Ads

void UCASMobileAds::ShowAdOnReturnToApp() { GetBridge()->CallMethod<void>(GetBridge()->ShowAdOnReturnToApp); }

void UCASMobileAds::SkipAdOnNextReturnToApp() { GetBridge()->CallMethod<void>(GetBridge()->SkipAdOnNextReturnToApp); }

void UCASMobileAds::DestroyReturnToAppAd() {
    CASInternal::KeepProxy(kCASUType_APP_RETURN, nullptr);
    GetBridge()->CallMethod<void>(GetBridge()->DestroyReturnToAppAd);
}

#endif