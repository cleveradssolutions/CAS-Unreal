// Copyright CleverAdsSolutions LTD, CAS.AI. All Rights Reserved.

#include "BlueprintAsync/CASShowAdProxy.h"
#include "CASMobileAds.h"
#include "CASInternal.h"

UCASShowAdProxy* UCASShowAdProxy::ShowInterstitialAd(UObject* WorldContextObject) {
    UCASShowAdProxy* Proxy = NewObject<UCASShowAdProxy>();
    Proxy->PrivateFlag = 10 + kCASUType_INTER;
    Proxy->WorldContextObject = WorldContextObject;
    return Proxy;
}

UCASShowAdProxy* UCASShowAdProxy::ShowAppOpenAd(UObject* WorldContextObject) {
    UCASShowAdProxy* Proxy = NewObject<UCASShowAdProxy>();
    Proxy->PrivateFlag = 10 + kCASUType_APP_OPEN;
    Proxy->WorldContextObject = WorldContextObject;
    return Proxy;
}

UCASShowAdProxy* UCASShowAdProxy::AutoshowInterstitialAd(UObject* WorldContextObject) {
    UCASShowAdProxy* Proxy = NewObject<UCASShowAdProxy>();
    Proxy->PrivateFlag = 20 + kCASUType_INTER;
    Proxy->WorldContextObject = WorldContextObject;
    return Proxy;
}

UCASShowAdProxy* UCASShowAdProxy::AutoshowAppOpenAd(UObject* WorldContextObject) {
    UCASShowAdProxy* Proxy = NewObject<UCASShowAdProxy>();
    Proxy->PrivateFlag = 20 + kCASUType_APP_OPEN;
    Proxy->WorldContextObject = WorldContextObject;
    return Proxy;
}

UCASShowAdProxy* UCASShowAdProxy::ShowAdOnReturnToApp(UObject* WorldContextObject) {
    UCASShowAdProxy* Proxy = NewObject<UCASShowAdProxy>();
    Proxy->PrivateFlag = 20 + kCASUType_INTER;
    Proxy->WorldContextObject = WorldContextObject;

    // Invoke in any case to reset previously instance
    CASInternal::KeepProxy(kCASUType_APP_RETURN, Proxy);
    return Proxy;
}

void UCASShowAdProxy::Activate() {
    bool NeedDestroy = true;
    if (PrivateFlag % 10 == kCASUType_INTER) {
        if (OnAdDisplayed.IsBound()) {
            NeedDestroy = false;
            UCASMobileAds::OnInterstitialAdDisplayed.AddUObject(this, &UCASShowAdProxy::HandleAdDisplayed);
        }
        if (OnAdFailed.IsBound()) {
            NeedDestroy = false;
            UCASMobileAds::OnInterstitialAdShowFailed.AddUObject(this, &UCASShowAdProxy::HandleAdFailed);
        }
        if (OnAdClicked.IsBound()) {
            NeedDestroy = false;
            UCASMobileAds::OnInterstitialAdClicked.AddUObject(this, &UCASShowAdProxy::HandleAdClicked);
        }
        // Must be subscribe if bound another event to destroy on dismissed.
        if (!NeedDestroy || OnAdDismissed.IsBound()) {
            NeedDestroy = false;
            UCASMobileAds::OnInterstitialAdDismissed.AddUObject(this, &UCASShowAdProxy::HandleAdDismissed);
        }

        if (PrivateFlag == 20 + kCASUType_INTER) {
            UCASMobileAds::AutoshowInterstitialAd();
        } else {
            UCASMobileAds::ShowInterstitialAd();
        }
    } else if (PrivateFlag % 10 == kCASUType_APP_OPEN) {
        if (OnAdDisplayed.IsBound()) {
            NeedDestroy = false;
            UCASMobileAds::OnAppOpenAdDisplayed.AddUObject(this, &UCASShowAdProxy::HandleAdDisplayed);
        }
        if (OnAdFailed.IsBound()) {
            NeedDestroy = false;
            UCASMobileAds::OnAppOpenAdShowFailed.AddUObject(this, &UCASShowAdProxy::HandleAdFailed);
        }
        if (OnAdClicked.IsBound()) {
            NeedDestroy = false;
            UCASMobileAds::OnAppOpenAdClicked.AddUObject(this, &UCASShowAdProxy::HandleAdClicked);
        }
        // Must be subscribe if bound another event to destroy on dismissed.
        if (!NeedDestroy || OnAdDismissed.IsBound()) {
            NeedDestroy = false;
            UCASMobileAds::OnAppOpenAdDismissed.AddUObject(this, &UCASShowAdProxy::HandleAdDismissed);
        }
        if (PrivateFlag == 20 + kCASUType_APP_OPEN) {
            UCASMobileAds::AutoshowAppOpenAd();
        } else {
            UCASMobileAds::ShowAppOpenAd();
        }
    }
    if (NeedDestroy) {
        SetReadyToDestroy();
    } else {
        RegisterWithGameInstance(WorldContextObject);
    }
}

void UCASShowAdProxy::HandleAdDisplayed() { OnAdDisplayed.Broadcast(ECASError::OK); }

void UCASShowAdProxy::HandleAdFailed(ECASError error) { OnAdFailed.Broadcast(error); }

void UCASShowAdProxy::HandleAdClicked() { OnAdClicked.Broadcast(ECASError::OK); }

void UCASShowAdProxy::HandleAdDismissed() {
    OnAdDismissed.Broadcast(ECASError::OK);
    if (PrivateFlag < 20) {
        SetReadyToDestroy();
    }
}
