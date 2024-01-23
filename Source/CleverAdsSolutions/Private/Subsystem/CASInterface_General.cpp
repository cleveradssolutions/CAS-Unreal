// Copyright CleverAdsSolutions LTD, CAS.AI. All Rights Reserved.

#include "CASInterface_General.h"

#include "CASMobileAds.h"

FString UCASInterface_General::GetCASVersion() const { return UCASMobileAds::GetMobileAdsVersion(); }

void UCASInterface_General::ValidateIntegration() const { UCASMobileAds::ValidateAdsIntegration(); }

void UCASInterface_General::SetMuteAdSounds(bool Mute) const { UCASMobileAds::SetAdsMuted(Mute); }

void UCASInterface_General::SetTaggedAudience(ECASAudience Audience) const {
    UCASMobileAds::SetUserAudienceForAds(Audience);
}

void UCASInterface_General::SetUserConsent(ECASUserConsentStatus ConsentStatus) const {
    UCASMobileAds::SetUserConsentForAds(ConsentStatus);
}

void UCASInterface_General::SetCCPAStatus(ECASUserCCPAStatus CCPAStatus) const {
    UCASMobileAds::SetUserOptOutSaleForAds(CCPAStatus);
}