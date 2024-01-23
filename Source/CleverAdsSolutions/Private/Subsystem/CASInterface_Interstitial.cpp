// Copyright CleverAdsSolutions LTD, CAS.AI. All Rights Reserved.

#include "CASInterface_Interstitial.h"

#include "CASMobileAds.h"
#include "CASInternal.h"

UCASInterface_Interstitial::UCASInterface_Interstitial() : UCASInterface_ScreenAd() {
    UCASMobileAds::OnInterstitialAdLoaded.AddUObject(this, &UCASInterface_Interstitial::AdLoadedCallback);
    UCASMobileAds::OnInterstitialAdClicked.AddUObject(this, &UCASInterface_Interstitial::AdClickedCallback);
    UCASMobileAds::OnInterstitialAdDismissed.AddUObject(this, &UCASInterface_Interstitial::AdClosedCallback);
    UCASMobileAds::OnInterstitialAdLoadFailed.AddUObject(this, &UCASInterface_Interstitial::AdFailedCallback);
    UCASMobileAds::OnInterstitialAdShowFailed.AddUObject(this, &UCASInterface_Interstitial::AdShowFailedCallback);
    CASInternal::OnAdsImpression.AddDynamic(this, &UCASInterface_Interstitial::AdShownCallback);
}

void UCASInterface_Interstitial::Show() { UCASMobileAds::ShowInterstitialAd(); }

void UCASInterface_Interstitial::Load() { UCASMobileAds::LoadInterstitialAd(); }

bool UCASInterface_Interstitial::IsReady() { return UCASMobileAds::IsInterstitialAdReady(); }

void UCASInterface_Interstitial::RestartInterval() const { UCASMobileAds::RestartInterstitialAdInterval(); }

void UCASInterface_Interstitial::AdShownCallback(const FCASImpressionInfo& info) {
    if (info.PlacementType == CAS_FORMAT_INTERSTITIAL) {
        Super::AdShownCallback(info);
    }
}