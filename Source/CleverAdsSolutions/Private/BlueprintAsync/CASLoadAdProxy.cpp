// Copyright CleverAdsSolutions LTD, CAS.AI. All Rights Reserved.

#include "BlueprintAsync/CASLoadAdProxy.h"
#include "CASMobileAds.h"
#include "CASInternal.h"

UCASLoadAdProxy* UCASLoadAdProxy::LoadInterstitialAd(UObject* WorldContextObject, bool EventOnce) {
    UCASLoadAdProxy* Proxy = NewObject<UCASLoadAdProxy>();
    Proxy->PrivateFlag = 10 + kCASUType_INTER;
    Proxy->IsOnceEvent = EventOnce;
    Proxy->WorldContextObject = WorldContextObject;

    // Invoke in any case to reset previously instance
    CASInternal::KeepProxy(kCASUType_INTER, Proxy);
    return Proxy;
}

UCASLoadAdProxy* UCASLoadAdProxy::LoadRewardedAd(UObject* WorldContextObject, bool EventOnce) {
    UCASLoadAdProxy* Proxy = NewObject<UCASLoadAdProxy>();
    Proxy->PrivateFlag = 10 + kCASUType_REWARD;
    Proxy->IsOnceEvent = EventOnce;
    Proxy->WorldContextObject = WorldContextObject;

    // Invoke in any case to reset previously instance
    CASInternal::KeepProxy(kCASUType_REWARD, Proxy);
    return Proxy;
}

void UCASLoadAdProxy::Activate() {
    bool NeedDestroy = true;
    if (PrivateFlag == 10 + kCASUType_INTER) {
        if (OnAdLoaded.IsBound()) {
            NeedDestroy = false;
            UCASMobileAds::OnInterstitialAdLoaded.AddUObject(this, &UCASLoadAdProxy::HandleAdLoaded);
        }
        if (OnAdFailed.IsBound()) {
            NeedDestroy = false;
            UCASMobileAds::OnInterstitialAdLoadFailed.AddUObject(this, &UCASLoadAdProxy::HandleAdFailed);
        }

        UCASMobileAds::LoadInterstitialAd();
    } else if (PrivateFlag == 10 + kCASUType_REWARD) {
        if (OnAdLoaded.IsBound()) {
            NeedDestroy = false;
            UCASMobileAds::OnRewardedAdLoaded.AddUObject(this, &UCASLoadAdProxy::HandleAdLoaded);
        }
        if (OnAdFailed.IsBound()) {
            NeedDestroy = false;
            UCASMobileAds::OnRewardedAdLoadFailed.AddUObject(this, &UCASLoadAdProxy::HandleAdFailed);
        }

        UCASMobileAds::LoadRewardedAd();
    }
    if (NeedDestroy) {
        SetReadyToDestroy();
    } else {
        RegisterWithGameInstance(WorldContextObject);
    }
}

void UCASLoadAdProxy::HandleAdLoaded() {
    if (OnAdLoaded.IsBound()) {
        OnAdLoaded.Broadcast(ECASError::OK);
        if (IsOnceEvent) {
            OnAdLoaded.Clear();
            OnAdFailed.Clear();
        }
    }
}

void UCASLoadAdProxy::HandleAdFailed(ECASError error) {
    if (OnAdFailed.IsBound()) {
        OnAdFailed.Broadcast(error);
        if (IsOnceEvent) {
            OnAdFailed.Clear();
        }
    }
}
