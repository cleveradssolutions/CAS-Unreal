// Copyright CleverAdsSolutions LTD, CAS.AI. All Rights Reserved.

#include "BlueprintAsync/CASLoadBannerAdProxy.h"
#include "CASMobileAds.h"
#include "CASInternal.h"

UCASLoadBannerAdProxy* UCASLoadBannerAdProxy::LoadBannerAd(UObject* WorldContextObject,
                                                           ECASBannerSize AdSize,
                                                           bool EventOnce) {
    UCASLoadBannerAdProxy* Proxy = NewObject<UCASLoadBannerAdProxy>();
    Proxy->PrivateFlag = static_cast<int>(AdSize);
    Proxy->IsOnceEvent = EventOnce;
    Proxy->WorldContextObject = WorldContextObject;

    // Invoke in any case to reset previously instance
    CASInternal::KeepProxy(kCASUType_BANNER, Proxy);
    return Proxy;
}

UCASLoadBannerAdProxy* UCASLoadBannerAdProxy::LoadMrecAd(UObject* WorldContextObject, bool EventOnce) {
    UCASLoadBannerAdProxy* Proxy = NewObject<UCASLoadBannerAdProxy>();
    Proxy->PrivateFlag = kCASUSize_MREC;
    Proxy->IsOnceEvent = EventOnce;
    Proxy->WorldContextObject = WorldContextObject;

    // Invoke in any case to reset previously instance
    CASInternal::KeepProxy(kCASUType_MREC, Proxy);
    return Proxy;
}

void UCASLoadBannerAdProxy::Activate() {
    bool NeedDestroy = true;
    if (PrivateFlag == kCASUSize_MREC) {
        if (OnAdLoaded.IsBound()) {
            NeedDestroy = false;
            UCASMobileAds::OnMRecAdLoaded.AddUObject(this, &UCASLoadBannerAdProxy::HandleAdLoaded);
        }
        if (OnAdFailed.IsBound()) {
            NeedDestroy = false;
            UCASMobileAds::OnMRecAdFailed.AddUObject(this, &UCASLoadBannerAdProxy::HandleAdFailed);
        }
        if (OnAdClicked.IsBound()) {
            NeedDestroy = false;
            UCASMobileAds::OnMRecAdClicked.AddUObject(this, &UCASLoadBannerAdProxy::HandleAdClicked);
        }

        UCASMobileAds::LoadMRecAd();
    } else {
        if (OnAdLoaded.IsBound()) {
            NeedDestroy = false;
            UCASMobileAds::OnBannerAdLoaded.AddUObject(this, &UCASLoadBannerAdProxy::HandleAdLoaded);
        }
        if (OnAdFailed.IsBound()) {
            NeedDestroy = false;
            UCASMobileAds::OnBannerAdFailed.AddUObject(this, &UCASLoadBannerAdProxy::HandleAdFailed);
        }
        if (OnAdClicked.IsBound()) {
            NeedDestroy = false;
            UCASMobileAds::OnBannerAdClicked.AddUObject(this, &UCASLoadBannerAdProxy::HandleAdClicked);
        }

        UCASMobileAds::LoadBannerAd(static_cast<ECASBannerSize>(PrivateFlag));
    }
    if (NeedDestroy) {
        SetReadyToDestroy();
    } else {
        RegisterWithGameInstance(WorldContextObject);
    }
}

void UCASLoadBannerAdProxy::HandleAdClicked() { OnAdClicked.Broadcast(ECASError::OK); }
