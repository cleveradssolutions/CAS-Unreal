// Copyright CleverAdsSolutions LTD, CAS.AI. All Rights Reserved.

#include "CASMobileAds.h"
#include "CASInternal.h"

FOnCASInitializedDelegate UCASMobileAds::OnAdsInitialized;
FOnCASCallbackDelegate UCASMobileAds::OnConsentFlowDismissed;

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

FOnCASFailedDelegate UCASMobileAds::OnReturnToAppAdShowFailed;
FOnCASCallbackDelegate UCASMobileAds::OnReturnToAppAdDisplayed;
FOnCASCallbackDelegate UCASMobileAds::OnReturnToAppAdClicked;
FOnCASCallbackDelegate UCASMobileAds::OnReturnToAppAdDismissed;

void UCASMobileAds::BindEventToOnAdsImpression(const FCASImpressionDelegate &OnImpression) {
    CASInternal::OnAdsImpression.AddUnique(OnImpression);
}

void UCASMobileAds::UnbindEventFromOnAdsImpression(const FCASImpressionDelegate &OnImpression) {
    CASInternal::OnAdsImpression.Remove(OnImpression);
}

void UCASMobileAds::UnbindAllEventsFromOnAdsImpression() { CASInternal::OnAdsImpression.Clear(); }

FString UCASMobileAds::GetAdsErrorMessage(ECASError Error) {
    switch (Error) {
        case ECASError::NoConnection:
            return TEXT("No internet connection detected");
        case ECASError::NoFill:
            return TEXT("No Fill");
        case ECASError::Configuration:
            return TEXT("Invalid configuration");
        case ECASError::NotReady:
            return TEXT("Ad are not ready");
        case ECASError::ManagerIsDisabled:
            return TEXT("Manager is disabled");
        case ECASError::ReachedCap:
            return TEXT("Reached cap for user");
        case ECASError::IntervalNotYetPassed:
            return TEXT("The interval between Ad impressions has not yet passed");
        case ECASError::AlreadyDisplayed:
            return TEXT("Ad already displayed");
        case ECASError::AppNotForeground:
            return TEXT("Application is not foreground");
        default:
            return TEXT("Internal error");
    }
}