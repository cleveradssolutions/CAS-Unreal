// Copyright CleverAdsSolutions LTD, CAS.AI. All Rights Reserved.

#include "CASInterface_ScreenAd.h"

#include "CASInternal.h"
#include "CASMobileAds.h"

void UCASInterface_ScreenAd::Show() {}

bool UCASInterface_ScreenAd::IsReady() { return false; }

void UCASInterface_ScreenAd::Load() {}

void UCASInterface_ScreenAd::AdShownCallback(const FCASImpressionInfo& info) { OnShown.Broadcast(info); }

void UCASInterface_ScreenAd::AdLoadedCallback() { OnLoaded.Broadcast(); }

void UCASInterface_ScreenAd::AdFailedCallback(ECASError error) {
    if (OnLoadError.IsBound()) {
        OnLoadError.Broadcast(UCASMobileAds::GetAdsErrorMessage(error));
    }
}

void UCASInterface_ScreenAd::AdShowFailedCallback(ECASError error) {
    if (OnShowError.IsBound()) {
        OnShowError.Broadcast(UCASMobileAds::GetAdsErrorMessage(error));
    }
}

void UCASInterface_ScreenAd::AdClickedCallback() { OnClicked.Broadcast(); }

void UCASInterface_ScreenAd::AdCompletedCallback() { OnCompleted.Broadcast(); }

void UCASInterface_ScreenAd::AdClosedCallback() { OnClosed.Broadcast(); }
