// Copyright CleverAdsSolutions LTD, CAS.AI. All Rights Reserved.

#include "CASInternal.h"

#include "CASMobileAds.h"


DEFINE_LOG_CATEGORY(LogCAS_AI);

// MARK: Internal Dynamic events

FCASImpressionEvent CASInternal::OnAdsImpression;

void CASInternal::KeepProxy(int32 Type, UBlueprintAsyncActionBase* Proxy) {
    static TArray<TWeakObjectPtr<UBlueprintAsyncActionBase>> Refs;
    if (Refs.Num() == 0) {
        Refs.SetNum(kCASUType_MANAGER);
    } else {
        UBlueprintAsyncActionBase* Instance = Refs[Type].Get();
        if (Instance != nullptr) {
            Instance->SetReadyToDestroy();
        }
    }
    if (Proxy == nullptr) {
        Refs[Type].Reset();
    } else {
        Refs[Type] = TWeakObjectPtr<UBlueprintAsyncActionBase>(Proxy);
    }
}

void CASInternal::HandleEventCallback(int AdType, int Callback, int Error) {
    ECASError CasError = static_cast<ECASError>(Error);
    switch (Callback) {
        case kCASUCallback_INITIALIZED:
            if (AdType == kCASUType_MANAGER && UCASMobileAds::OnAdsInitialized.IsBound()) {
                FCASInitialConfig Config;
                FillInitialConfig(Config);
                UCASMobileAds::OnAdsInitialized.Broadcast(Config);
            }
            break;
        case kCASUCallback_PAY:
            if (UCASMobileAds::OnAdsImpression.IsBound() || OnAdsImpression.IsBound()) {
                FCASImpressionInfo info;
                FillImpressionInfo(AdType, info);
                switch (AdType) {
                    case kCASUType_INTER:
                        info.PlacementType = CAS_FORMAT_INTERSTITIAL;
                        break;
                    case kCASUType_REWARD:
                        info.PlacementType = CAS_FORMAT_REWARDED;
                        break;
                    case kCASUType_APP_RETURN:
                        info.PlacementType = CAS_FORMAT_RETURN_TO_APP;
                        break;
                    case kCASUType_BANNER:
                        info.PlacementType = CAS_FORMAT_BANNER;
                        break;
                    case kCASUType_MREC:
                        info.PlacementType = CAS_FORMAT_MREC;
                        break;
                }

                UCASMobileAds::OnAdsImpression.Broadcast(info);
                OnAdsImpression.Broadcast(info);
            }
            break;
        case kCASUCallback_SHOW:
            switch (AdType) {
                case kCASUType_INTER:
                    UCASMobileAds::OnInterstitialAdDisplayed.Broadcast();
                    break;
                case kCASUType_REWARD:
                    UCASMobileAds::OnRewardedAdDisplayed.Broadcast();
                    break;
                case kCASUType_APP_RETURN:
                    UCASMobileAds::OnReturnToAppAdDisplayed.Broadcast();
                    break;
            }
            break;
        case kCASUCallback_SHOW_FAILED:
            switch (AdType) {
                case kCASUType_INTER:
                    UCASMobileAds::OnInterstitialAdShowFailed.Broadcast(CasError);
                    UCASMobileAds::OnInterstitialAdDismissed.Broadcast();
                    break;
                case kCASUType_REWARD:
                    UCASMobileAds::OnRewardedAdShowFailed.Broadcast(CasError);
                    UCASMobileAds::OnRewardedAdDismissed.Broadcast();
                    break;
                case kCASUType_APP_RETURN:
                    UCASMobileAds::OnReturnToAppAdShowFailed.Broadcast(CasError);
                    UCASMobileAds::OnReturnToAppAdDismissed.Broadcast();
                    break;
            }
            break;
        case kCASUCallback_COMPLETED:
            if (AdType == kCASUType_REWARD) {
                UCASMobileAds::OnRewardedAdEarnedReward.Broadcast();
            }
            break;
        case kCASUCallback_CLICKED:
            switch (AdType) {
                case kCASUType_INTER:
                    UCASMobileAds::OnInterstitialAdClicked.Broadcast();
                    break;
                case kCASUType_REWARD:
                    UCASMobileAds::OnRewardedAdClicked.Broadcast();
                    break;
                case kCASUType_APP_RETURN:
                    UCASMobileAds::OnReturnToAppAdClicked.Broadcast();
                    break;
                case kCASUType_BANNER:
                    UCASMobileAds::OnBannerAdClicked.Broadcast();
                    break;
                case kCASUType_MREC:
                    UCASMobileAds::OnMRecAdClicked.Broadcast();
                    break;
            }
            break;
        case kCASUCallback_CLOSED:
            switch (AdType) {
                case kCASUType_INTER:
                    UCASMobileAds::OnInterstitialAdDismissed.Broadcast();
                    break;
                case kCASUType_REWARD:
                    UCASMobileAds::OnRewardedAdDismissed.Broadcast();
                    break;
                case kCASUType_APP_RETURN:
                    UCASMobileAds::OnReturnToAppAdDismissed.Broadcast();
                    break;
            }
            break;
        case kCASUCallback_LOADED:
            switch (AdType) {
                case kCASUType_INTER:
                    UCASMobileAds::OnInterstitialAdLoaded.Broadcast();
                    break;
                case kCASUType_REWARD:
                    UCASMobileAds::OnRewardedAdLoaded.Broadcast();
                    break;
                case kCASUType_BANNER:
                    UCASMobileAds::OnBannerAdLoaded.Broadcast();
                    break;
                case kCASUType_MREC:
                    UCASMobileAds::OnMRecAdLoaded.Broadcast();
                    break;
            }
            break;
        case kCASUCallback_LOAD_FAILED:
            switch (AdType) {
                case kCASUType_INTER:
                    UCASMobileAds::OnInterstitialAdLoadFailed.Broadcast(CasError);
                    break;
                case kCASUType_REWARD:
                    UCASMobileAds::OnRewardedAdLoadFailed.Broadcast(CasError);
                    break;
                case kCASUType_BANNER:
                    UCASMobileAds::OnBannerAdFailed.Broadcast(CasError);
                    break;
                case kCASUType_MREC:
                    UCASMobileAds::OnMRecAdFailed.Broadcast(CasError);
                    break;
            }
            break;
        case kCASUCallback_CONSENT_FLOW_DISMISSED:
            UCASMobileAds::OnConsentFlowDismissed.Broadcast();
            UCASMobileAds::OnConsentFlowResult.Broadcast(static_cast<ECASConsentFlowStatus>(Error));
            break;
    }
}