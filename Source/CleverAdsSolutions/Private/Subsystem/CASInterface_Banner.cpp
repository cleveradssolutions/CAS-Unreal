// Copyright CleverAdsSolutions LTD, CAS.AI. All Rights Reserved.

#include "CASInterface_Banner.h"

#include "CASMobileAds.h"
#include "CASInternal.h"

bool IsBannerVisible = false;

UCASInterface_Banner::UCASInterface_Banner()
    : UCASInterface(), AdSize(ECASBannerSize::Banner), AdPosition(ECASPosition::BottomCenter) {
    UCASMobileAds::OnBannerAdClicked.AddUObject(this, &UCASInterface_Banner::HandleAdClicked);
    UCASMobileAds::OnBannerAdLoaded.AddUObject(this, &UCASInterface_Banner::HandleAdLoaded);
    UCASMobileAds::OnBannerAdFailed.AddUObject(this, &UCASInterface_Banner::HandleAdFailed);
    CASInternal::OnAdsImpression.AddDynamic(this, &UCASInterface_Banner::HandleAdImpression);
}

void UCASInterface_Banner::CreateBanner(ECASBannerSize BannerSize) {
    AdSize = BannerSize;
    UCASMobileAds::LoadBannerAd(BannerSize);
    ToggleBannerVisibility(true);
}

void UCASInterface_Banner::UpdateBannerPosition(ECASPosition BannerPosition) {
    AdPosition = BannerPosition;
    UCASMobileAds::SetBannerAdPosition(BannerPosition);
}

void UCASInterface_Banner::LoadNextBanner() { UCASMobileAds::LoadBannerAd(AdSize); }

void UCASInterface_Banner::DestroyBanner() {
    OnDestroyed.Broadcast();
    UCASMobileAds::DestroyBannerAd();
}

void UCASInterface_Banner::ToggleBannerVisibility(bool Visible) {
    IsBannerVisible = Visible;
    if (Visible) {
        UCASMobileAds::ShowBannerAd();
    } else {
        UCASMobileAds::HideBannerAd();
    }
}

bool UCASInterface_Banner::IsReady() { return UCASMobileAds::IsBannerAdReady(); }

void UCASInterface_Banner::SetRefreshInterval(int Interval) { UCASMobileAds::SetBannerAdRefreshInterval(Interval); }

void UCASInterface_Banner::DisableRefreshInterval() { UCASMobileAds::SetBannerAdRefreshInterval(0); }

bool UCASInterface_Banner::IsVisible() { return IsBannerVisible; }

void UCASInterface_Banner::HandleAdLoaded() { OnLoaded.Broadcast(); }

void UCASInterface_Banner::HandleAdFailed(ECASError Error) {
    OnFail.Broadcast(UCASMobileAds::GetAdsErrorMessage(Error));
}

void UCASInterface_Banner::HandleAdImpression(const FCASImpressionInfo& Info) {
    if (Info.PlacementType == CAS_FORMAT_BANNER) {
        OnShow.Broadcast(Info);
    }
}

void UCASInterface_Banner::HandleAdClicked() { OnClicked.Broadcast(); }