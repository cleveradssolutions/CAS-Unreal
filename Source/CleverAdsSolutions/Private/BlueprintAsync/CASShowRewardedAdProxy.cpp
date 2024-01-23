// Copyright CleverAdsSolutions LTD, CAS.AI. All Rights Reserved.

#include "BlueprintAsync/CASShowRewardedAdProxy.h"
#include "CASMobileAds.h"
#include "CASInternal.h"

UCASShowRewardedAdProxy* UCASShowRewardedAdProxy::ShowRewardedAd(UObject* WorldContextObject) {
    UCASShowRewardedAdProxy* Proxy = NewObject<UCASShowRewardedAdProxy>();
    Proxy->RegisterWithGameInstance(WorldContextObject);
    Proxy->PrivateFlag = 10 + kCASUType_REWARD;
    Proxy->WorldContextObject = WorldContextObject;
    return Proxy;
}

void UCASShowRewardedAdProxy::Activate() {
    bool NeedDestroy = true;
    if (OnAdEarnedReward.IsBound()) {
        NeedDestroy = false;
        UCASMobileAds::OnRewardedAdEarnedReward.AddUObject(this, &UCASShowRewardedAdProxy::HandleAdEarnReward);
    }
    if (OnAdDisplayed.IsBound()) {
        NeedDestroy = false;
        UCASMobileAds::OnRewardedAdDisplayed.AddUObject(this, &UCASShowRewardedAdProxy::HandleAdDisplayed);
    }
    if (OnAdFailed.IsBound()) {
        NeedDestroy = false;
        UCASMobileAds::OnRewardedAdShowFailed.AddUObject(this, &UCASShowRewardedAdProxy::HandleAdFailed);
    }
    if (OnAdClicked.IsBound()) {
        NeedDestroy = false;
        UCASMobileAds::OnRewardedAdClicked.AddUObject(this, &UCASShowRewardedAdProxy::HandleAdClicked);
    }
    // Must be subscribe if bound another event to destroy on dismissed.
    if (!NeedDestroy || OnAdDismissed.IsBound()) {
        NeedDestroy = false;
        UCASMobileAds::OnRewardedAdDismissed.AddUObject(this, &UCASShowRewardedAdProxy::HandleAdDismissed);
    }
    UCASMobileAds::ShowRewardedAd();
    if (NeedDestroy) {
        SetReadyToDestroy();
    } else {
        RegisterWithGameInstance(WorldContextObject);
    }
}

void UCASShowRewardedAdProxy::HandleAdEarnReward() { OnAdEarnedReward.Broadcast(ECASError::OK); }
