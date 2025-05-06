// Copyright CleverAdsSolutions LTD, CAS.AI. All Rights Reserved.

#if PLATFORM_ANDROID
#include "CASMobileAds.h"

#include "Async/Async.h"
#include "CASDefaultConfig.h"
#include "CASInternal.h"
#include "JavaCASBridgeObject.h"
#include "Misc/EngineVersion.h"

TSharedPtr<FJavaCASBridgeObject> GetBridge() {
    static TSharedPtr<FJavaCASBridgeObject, ESPMode::ThreadSafe> Bridge;

    if (!Bridge.IsValid()) {
        Bridge = MakeShared<FJavaCASBridgeObject, ESPMode::ThreadSafe>();
        const UCASDefaultConfig *DefaultConfig = GetDefault<UCASDefaultConfig>();

        CAS_LOG_D("Apply Ads ID: %s", *DefaultConfig->CASAppID);

        if (DefaultConfig->Audience != ECASAudience::Undefined) {
            Bridge->CallMethod<void>(Bridge->SetUserAudienceForAds,
                                     static_cast<int>(DefaultConfig->Audience));
        }

#if !UE_BUILD_SHIPPING

        if (DefaultConfig->TestAdsMode) {
            Bridge->CallMethod<void>(Bridge->SetTestAdModeForInit);
        }

#endif

        TArray<FString> TestDeviceIds = DefaultConfig->TestDeviceIds;

        if (TestDeviceIds.Num()) {
            for (const FString &Element : TestDeviceIds) {
                Bridge->CallMethod<void>(Bridge->AddTestDeviceId,
                                         *Bridge->GetJString(Element));
            }
        }

        Bridge->CallMethod<void>(
            Bridge->SetConsentFlowEnabled, DefaultConfig->AutoConsentFlow,
            static_cast<int>(DefaultConfig->UserDebugGeography),
            *Bridge->GetJString(DefaultConfig->PrivacyPolicyURL));

        Bridge->CallMethod<void>(Bridge->SetAutoloadEnabled, kCASUType_BANNER,
                                 DefaultConfig->AutoloadBannerAds);
        Bridge->CallMethod<void>(Bridge->SetAutoloadEnabled, kCASUType_MREC,
                                 DefaultConfig->AutoloadMRecAds);
        Bridge->CallMethod<void>(Bridge->SetAutoloadEnabled, kCASUType_INTER,
                                 DefaultConfig->AutoloadInterstitialAds);
        Bridge->CallMethod<void>(Bridge->SetAutoloadEnabled, kCASUType_REWARD,
                                 DefaultConfig->AutoloadRewardedAds);
        Bridge->CallMethod<void>(Bridge->SetAutoloadEnabled, kCASUType_APP_OPEN,
                                 DefaultConfig->AutoloadAppOpenAds);

        // Framework Unreal info
        const FEngineVersion &EngineVersion = FEngineVersion::Current();
        const FString VersionString =
            FString::Printf(TEXT("%u.%u.%u"), EngineVersion.GetMajor(),
                            EngineVersion.GetMinor(), EngineVersion.GetPatch());

        Bridge->CallMethod<void>(Bridge->SetPluginConfig,
                                 *Bridge->GetJString(DefaultConfig->CASAppID),
                                 *Bridge->GetJString(VersionString));
    }

    return Bridge;
}

// MARK: Bridge Callbacks (Should be at top of script)

void CASInternal::FillImpressionInfo(int AdType, FCASImpressionInfo &Info) {
    TSharedPtr<FJavaCASBridgeObject> Bridge = GetBridge();

    Info.RevenueCPM = Bridge->CallFloatMethod(Bridge->GetImpressionCPM, AdType);
    Info.Network =
        Bridge->CallMethod<FString>(Bridge->GetImpressionNetwork, AdType);
    Info.UnitID =
        Bridge->CallMethod<FString>(Bridge->GetImpressionAdUnit, AdType);
    Info.CreativeID =
        Bridge->CallMethod<FString>(Bridge->GetImpressionCreative, AdType);
    Info.TotalImpressions =
        Bridge->CallMethod<int>(Bridge->GetImpressionDepth, AdType);
    Info.LifetimeRevenue =
        Bridge->CallFloatMethod(Bridge->GetImpressionLifetimeRevenue, AdType);
}

void CASInternal::FillInitialConfig(FCASInitialConfig &Config) {
    TSharedPtr<FJavaCASBridgeObject> Bridge = GetBridge();

    Config.Error = Bridge->CallMethod<FString>(Bridge->GetInitializationError);
    Config.CountryCode = Bridge->CallMethod<FString>(Bridge->GetUserCountryCode);
    Config.UserConsentStatus = static_cast<ECASUserConsentStatus>(
        Bridge->CallMethod<int>(Bridge->GetUserConsentForAds));
    Config.ConsentFlowStatus = static_cast<ECASConsentFlowStatus>(
        Bridge->CallMethod<int>(Bridge->GetUserConsentFlowStatus));
}

void CASInternal::ShowConsentFlow(bool ifRequired) {
    TSharedPtr<FJavaCASBridgeObject> Bridge = GetBridge();

    Bridge->CallMethod<void>(Bridge->ShowConsentFlow, ifRequired);
}

extern "C" JNIEXPORT void JNICALL
Java_com_cleveradssolutions_plugin_unreal_CASUEvent_callback(
    JNIEnv *env, jobject thiz, jint adType, jint callback, jint error) {
    const int AdType = (int)adType;
    const int Callback = (int)callback;
    const int Error = (int)error;

    AsyncTask(ENamedThreads::GameThread, [ = ]() {
        CASInternal::HandleEventCallback(AdType, Callback, Error);
    });
}

// MARK: Mobile Ads implementation

void UCASMobileAds::SetInitializationAdsExtras(FString key, FString value) {
    GetBridge()->CallMethod<void>(GetBridge()->SetInitializationExtras,
                                  *GetBridge()->GetJString(key),
                                  *GetBridge()->GetJString(value));
}

bool UCASMobileAds::IsInitializedAds() {
    return GetBridge()->CallMethod<bool>(GetBridge()->IsInitializedAds);
}

void UCASMobileAds::InitializeMobileAds() {
    if (IsInitializedAds()) {
        CASInternal::HandleEventCallback(kCASUType_MANAGER,
                                         kCASUCallback_INITIALIZED, 0);
        return;
    }

    CAS_LOG_D("Initialize Ads");
    GetBridge()->CallMethod<void>(GetBridge()->InitializeMobileAds);
}

FString UCASMobileAds::GetMobileAdsVersion() {
    return GetBridge()->CallMethod<FString>(GetBridge()->GetMobileAdsVersion);
}

void UCASMobileAds::ValidateAdsIntegration() {
    GetBridge()->CallMethod<void>(GetBridge()->ValidateAdsIntegration);
}

void UCASMobileAds::SetVerboseAdsLogs(bool Enabled) {
    GetBridge()->CallMethod<void>(GetBridge()->SetVerboseAdsLogs, Enabled);
}

void UCASMobileAds::SetAdsMuted(bool Mute) {
    GetBridge()->CallMethod<void>(GetBridge()->SetAdsMuted, Mute);
}

void UCASMobileAds::SetTrialAdFreeInterval(int Interval) {
    GetBridge()->CallMethod<void>(GetBridge()->SetTrialAdFreeInterval, Interval);
}

void UCASMobileAds::SetUserAudienceForAds(ECASAudience Audience) {
    GetBridge()->CallMethod<void>(GetBridge()->SetUserAudienceForAds,
                                  static_cast<int>(Audience));
}

bool UCASMobileAds::IsUserAdConsentRequired() {
    return GetBridge()->CallMethod<bool>(GetBridge()->IsUserConsentRequired);
}

void UCASMobileAds::SetUserConsentForAds(ECASUserConsentStatus ConsentStatus) {
    GetBridge()->CallMethod<void>(GetBridge()->SetUserConsentForAds,
                                  static_cast<int>(ConsentStatus));
}

void UCASMobileAds::SetUserOptOutSaleForAds(ECASUserCCPAStatus CCPAStatus) {
    GetBridge()->CallMethod<void>(GetBridge()->SetUserOptOutSaleForAds,
                                  static_cast<int>(CCPAStatus));
}

void UCASMobileAds::SetUserIDForAds(FString UserID) {
    GetBridge()->CallMethod<void>(GetBridge()->SetUserIDForAds,
                                  *GetBridge()->GetJString(UserID));
}

void UCASMobileAds::SetUserAgeForAds(int UserAge) {
    GetBridge()->CallMethod<void>(GetBridge()->SetUserAgeForAds, UserAge);
}

void UCASMobileAds::SetUserGenderForAds(ECASGender UserGender) {
    GetBridge()->CallMethod<void>(GetBridge()->SetUserGenderForAds,
                                  static_cast<int>(UserGender));
}

void UCASMobileAds::SetUserLocationCollectionForAds(bool Enabled) {
    GetBridge()->CallMethod<void>(GetBridge()->SetLocationCollectionEnabled,
                                  Enabled);
}

void UCASMobileAds::SetMetaDataProcessingOptions(
    ECASUserCCPAStatus MetaOptions) {
    GetBridge()->CallMethod<void>(GetBridge()->SetMetaDataProcessingOptions,
                                  static_cast<int>(MetaOptions));
}

// MARK: Banner Ads

bool UCASMobileAds::IsBannerAdReady() {
    return GetBridge()->CallMethod<bool>(GetBridge()->IsAdLoaded,
                                         kCASUType_BANNER);
}

void UCASMobileAds::LoadBannerAd(ECASBannerSize AdSize) {
    GetBridge()->CallMethod<void>(GetBridge()->LoadBannerAd,
                                  static_cast<int>(AdSize));
}

void UCASMobileAds::SetAutoloadBannerAd(bool Enabled) {
    GetBridge()->CallMethod<void>(GetBridge()->SetAutoloadEnabled,
                                  kCASUType_BANNER, Enabled);
}

void UCASMobileAds::ShowBannerAd() {
    GetBridge()->CallMethod<void>(GetBridge()->ShowAd, kCASUType_BANNER);
}

void UCASMobileAds::HideBannerAd() {
    GetBridge()->CallMethod<void>(GetBridge()->HideAd, kCASUType_BANNER);
}

void UCASMobileAds::SetBannerAdPosition(ECASPosition AdPosition) {
    GetBridge()->CallMethod<void>(GetBridge()->SetBannerAdPosition,
                                  static_cast<int>(AdPosition), 0, 0);
}

void UCASMobileAds::SetBannerAdRefreshInterval(int Interval) {
    GetBridge()->CallMethod<void>(GetBridge()->SetBannerAdRefreshInterval,
                                  Interval);
}

void UCASMobileAds::DestroyBannerAd() {
    GetBridge()->CallMethod<void>(GetBridge()->DestroyAd, kCASUType_BANNER);
}

// MARK: MRec Ads

bool UCASMobileAds::IsMRecAdReady() {
    return GetBridge()->CallMethod<bool>(GetBridge()->IsAdLoaded, kCASUType_MREC);
}

void UCASMobileAds::LoadMRecAd() {
    GetBridge()->CallMethod<void>(GetBridge()->LoadAd, kCASUType_MREC);
}

void UCASMobileAds::SetAutoloadMRecAd(bool Enabled) {
    GetBridge()->CallMethod<void>(GetBridge()->SetAutoloadEnabled, kCASUType_MREC,
                                  Enabled);
}

void UCASMobileAds::ShowMRecAd() {
    GetBridge()->CallMethod<void>(GetBridge()->ShowAd, kCASUType_MREC);
}

void UCASMobileAds::HideMRecAd() {
    GetBridge()->CallMethod<void>(GetBridge()->HideAd, kCASUType_MREC);
}

void UCASMobileAds::SetMRecAdPosition(ECASPosition AdPosition) {
    GetBridge()->CallMethod<void>(GetBridge()->SetMRecAdPosition,
                                  static_cast<int>(AdPosition), 0, 0);
}

void UCASMobileAds::SetMRecAdRefreshInterval(int Interval) {
    GetBridge()->CallMethod<void>(GetBridge()->SetMRecAdRefreshInterval,
                                  Interval);
}

void UCASMobileAds::DestroyMRecAd() {
    GetBridge()->CallMethod<void>(GetBridge()->DestroyAd, kCASUType_MREC);
}

// MARK: Interstitial Ads

void UCASMobileAds::LoadInterstitialAd() {
    GetBridge()->CallMethod<void>(GetBridge()->LoadAd, kCASUType_INTER);
}

bool UCASMobileAds::IsInterstitialAdReady() {
    return GetBridge()->CallMethod<bool>(GetBridge()->IsAdLoaded,
                                         kCASUType_INTER);
}

void UCASMobileAds::SetAutoloadInterstitialAd(bool Enabled) {
    GetBridge()->CallMethod<void>(GetBridge()->SetAutoloadEnabled,
                                  kCASUType_INTER, Enabled);
}

void UCASMobileAds::ShowInterstitialAd() {
    GetBridge()->CallMethod<void>(GetBridge()->ShowAd, kCASUType_INTER);
}

void UCASMobileAds::AutoshowInterstitialAd() {
    GetBridge()->CallMethod<void>(GetBridge()->SetAutoshowEnabled,
                                  kCASUType_INTER, true);
}

void UCASMobileAds::DisableAutoshowInterstitialAd() {
    GetBridge()->CallMethod<void>(GetBridge()->SetAutoshowEnabled,
                                  kCASUType_INTER, false);
}

void UCASMobileAds::SetInterstitialAdMinimumInterval(int Interval) {
    GetBridge()->CallMethod<void>(GetBridge()->SetInterstitialAdMinimumInterval,
                                  Interval);
}

void UCASMobileAds::RestartInterstitialAdInterval() {
    GetBridge()->CallMethod<void>(GetBridge()->RestartInterstitialAdInterval);
}

void UCASMobileAds::DestroyInterstitialAd() {
    GetBridge()->CallMethod<void>(GetBridge()->DestroyAd, kCASUType_INTER);
}

// MARK: Rewarded Ads

bool UCASMobileAds::IsRewardedAdReady() {
    return GetBridge()->CallMethod<bool>(GetBridge()->IsAdLoaded,
                                         kCASUType_REWARD);
}

void UCASMobileAds::LoadRewardedAd() {
    GetBridge()->CallMethod<void>(GetBridge()->LoadAd, kCASUType_REWARD);
}

void UCASMobileAds::SetAutoloadRewardedAd(bool Enabled) {
    GetBridge()->CallMethod<void>(GetBridge()->SetAutoloadEnabled,
                                  kCASUType_REWARD, Enabled);
}

void UCASMobileAds::ShowRewardedAd() {
    GetBridge()->CallMethod<void>(GetBridge()->ShowAd, kCASUType_REWARD);
}

void UCASMobileAds::DestroyRewardedAd() {
    GetBridge()->CallMethod<void>(GetBridge()->DestroyAd, kCASUType_REWARD);
}

// MARK: AppOpen Ads

bool UCASMobileAds::IsAppOpenAdReady() {
    return GetBridge()->CallMethod<bool>(GetBridge()->IsAdLoaded,
                                         kCASUType_APP_OPEN);
}

void UCASMobileAds::LoadAppOpenAd() {
    GetBridge()->CallMethod<void>(GetBridge()->LoadAd, kCASUType_APP_OPEN);
}

void UCASMobileAds::SetAutoloadAppOpenAd(bool Enabled) {
    GetBridge()->CallMethod<void>(GetBridge()->SetAutoloadEnabled,
                                  kCASUType_APP_OPEN, Enabled);
}

void UCASMobileAds::ShowAppOpenAd() {
    GetBridge()->CallMethod<void>(GetBridge()->ShowAd, kCASUType_APP_OPEN);
}

void UCASMobileAds::AutoshowAppOpenAd() {
    GetBridge()->CallMethod<void>(GetBridge()->SetAutoshowEnabled,
                                  kCASUType_APP_OPEN, true);
}

void UCASMobileAds::DisableAutoshowAppOpenAd() {
    GetBridge()->CallMethod<void>(GetBridge()->SetAutoshowEnabled,
                                  kCASUType_APP_OPEN, false);
}

void UCASMobileAds::DestroyAppOpenAd() {
    GetBridge()->CallMethod<void>(GetBridge()->DestroyAd, kCASUType_APP_OPEN);
}

#endif /* if PLATFORM_ANDROID */
