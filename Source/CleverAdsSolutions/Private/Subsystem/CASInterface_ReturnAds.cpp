// Copyright CleverAdsSolutions LTD, CAS.AI. All Rights Reserved.

#include "CASInterface_ReturnAds.h"

#include "CASMobileAds.h"
#include "CASInternal.h"

UCASInterface_ReturnAds::UCASInterface_ReturnAds() : UCASInterface() {
    UCASMobileAds::OnReturnToAppAdClicked.AddUObject(this, &UCASInterface_ReturnAds::HandleAdClicked);
    UCASMobileAds::OnReturnToAppAdDismissed.AddUObject(this, &UCASInterface_ReturnAds::HandleAdClosed);
    UCASMobileAds::OnReturnToAppAdShowFailed.AddUObject(this, &UCASInterface_ReturnAds::HandleAdFailed);
    CASInternal::OnAdsImpression.AddDynamic(this, &UCASInterface_ReturnAds::HandleAdImpression);
}

void UCASInterface_ReturnAds::SkipNextAppReturnAds() { UCASMobileAds::SkipAdOnNextReturnToApp(); }

void UCASInterface_ReturnAds::EnableAppReturnAds() { UCASMobileAds::ShowAdOnReturnToApp(); }

void UCASInterface_ReturnAds::DisableAppReturnAds() { UCASMobileAds::DestroyReturnToAppAd(); }

void UCASInterface_ReturnAds::HandleAdFailed(ECASError Error) {
    OnShowError.Broadcast(UCASMobileAds::GetAdsErrorMessage(Error));
}

void UCASInterface_ReturnAds::HandleAdImpression(const FCASImpressionInfo& Info) {
    if (Info.PlacementType == CAS_FORMAT_RETURN_TO_APP) {
        OnShown.Broadcast(Info);
    }
}

void UCASInterface_ReturnAds::HandleAdClicked() { OnClicked.Broadcast(); }

void UCASInterface_ReturnAds::HandleAdClosed() { OnClosed.Broadcast(); }