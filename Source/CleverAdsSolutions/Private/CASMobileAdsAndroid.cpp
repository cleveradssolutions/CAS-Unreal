// Copyright CleverAdsSolutions LTD, CAS.AI. All Rights Reserved.

#if PLATFORM_ANDROID
#include "CASMobileAds.h"

#include "CASInternal.h"
#include "JavaCASBridgeObject.h"
#include "CASDefaultConfig.h"
#include "Async/Async.h"
#include "Misc/EngineVersion.h"

TSharedPtr<FJavaCASBridgeObject> GetBridge() {
    static TSharedPtr<FJavaCASBridgeObject, ESPMode::ThreadSafe> Bridge;
    if (!Bridge.IsValid()) {
        Bridge = MakeShared<FJavaCASBridgeObject, ESPMode::ThreadSafe>();
        const UCASDefaultConfig *DefaultConfig = GetDefault<UCASDefaultConfig>();

        CAS_LOG_D("Apply Ads ID: %s", *DefaultConfig->CASAppID);

        if (DefaultConfig->Audience != ECASAudience::Undefined) {
            Bridge->CallMethod<void>(Bridge->SetUserAudienceForAds, static_cast<int>(DefaultConfig->Audience));
        }

#if !UE_BUILD_SHIPPING
        if (DefaultConfig->TestAdsMode) {
            Bridge->CallMethod<void>(Bridge->SetTestAdModeForInit);
        }
#endif

        TArray<FString> TestDeviceIds = DefaultConfig->TestDeviceIds;
        if (TestDeviceIds.Num()) {
            for (const FString &Element : TestDeviceIds) {
                Bridge->CallMethod<void>(Bridge->AddTestDeviceId, *Bridge->GetJString(Element));
            }
        }

        Bridge->CallMethod<void>(Bridge->SetConsentFlowEnabled, DefaultConfig->AutoConsentFlow,
                                 static_cast<int>(DefaultConfig->UserDebugGeography),
                                 *Bridge->GetJString(DefaultConfig->PrivacyPolicyURL));

        Bridge->CallMethod<void>(Bridge->SetAutoloadFormats, DefaultConfig->AutoloadBannerAds,
                                 DefaultConfig->AutoloadMRecAds, DefaultConfig->AutoloadInterstitialAds,
                                 DefaultConfig->AutoloadRewardedAds);

        // Framework Unreal info
        const FEngineVersion &EngineVersion = FEngineVersion::Current();
        const FString VersionString = FString::Printf(TEXT("%u.%u.%u"), EngineVersion.GetMajor(),
                                                      EngineVersion.GetMinor(), EngineVersion.GetPatch());

        Bridge->CallMethod<void>(Bridge->SetPluginConfig, *Bridge->GetJString(DefaultConfig->CASAppID),
                                 *Bridge->GetJString(VersionString));
    }
    return Bridge;
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

void CASInternal::ShowConsentFlow(bool ifRequired) {
    TSharedPtr<FJavaCASBridgeObject> Bridge = GetBridge();
    Bridge->CallMethod<void>(Bridge->ShowConsentFlow, ifRequired);
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

    CAS_LOG_D("Initialize Ads");
    GetBridge()->CallMethod<void>(GetBridge()->InitializeMobileAds);
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

void UCASMobileAds::SetUserAudienceForAds(ECASAudience Audience) {
    GetBridge()->CallMethod<void>(GetBridge()->SetUserAudienceForAds, static_cast<int>(Audience));
}

bool UCASMobileAds::IsUserAdConsentRequired() {
    return GetBridge()->CallMethod<bool>(GetBridge()->IsUserConsentRequired);
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

void UCASMobileAds::SetUserLocationCollectionForAds(bool Enabled) {
    GetBridge()->CallMethod<void>(GetBridge()->SetLocationCollectionEnabled, Enabled);
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
