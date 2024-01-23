// Copyright CleverAdsSolutions LTD, CAS.AI. All Rights Reserved.

#include "CASInterface_Rewarded.h"

#include "CASMobileAds.h"
#include "CASInternal.h"

UCASInterface_Rewarded::UCASInterface_Rewarded() : UCASInterface_ScreenAd() {
    UCASMobileAds::OnRewardedAdEarnedReward.AddUObject(this, &UCASInterface_Rewarded::AdCompletedCallback);
    UCASMobileAds::OnRewardedAdLoaded.AddUObject(this, &UCASInterface_Rewarded::AdLoadedCallback);
    UCASMobileAds::OnRewardedAdClicked.AddUObject(this, &UCASInterface_Rewarded::AdClickedCallback);
    UCASMobileAds::OnRewardedAdDismissed.AddUObject(this, &UCASInterface_Rewarded::AdClosedCallback);
    UCASMobileAds::OnRewardedAdLoadFailed.AddUObject(this, &UCASInterface_Rewarded::AdFailedCallback);
    UCASMobileAds::OnRewardedAdShowFailed.AddUObject(this, &UCASInterface_Rewarded::AdShowFailedCallback);
    CASInternal::OnAdsImpression.AddDynamic(this, &UCASInterface_Rewarded::AdShownCallback);
}

void UCASInterface_Rewarded::Show() { UCASMobileAds::ShowRewardedAd(); }

void UCASInterface_Rewarded::Load() { UCASMobileAds::LoadRewardedAd(); }

bool UCASInterface_Rewarded::IsReady() { return UCASMobileAds::IsRewardedAdReady(); }

void UCASInterface_Rewarded::AdShownCallback(const FCASImpressionInfo& info) {
    if (info.PlacementType == CAS_FORMAT_REWARDED) {
        Super::AdShownCallback(info);
    }
}