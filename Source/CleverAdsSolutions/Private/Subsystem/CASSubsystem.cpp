// Copyright CleverAdsSolutions LTD, CAS.AI. All Rights Reserved.

#include "CASSubsystem.h"

#include "CASInterface_Banner.h"
#include "CASInterface_General.h"
#include "CASInterface_Interstitial.h"
#include "CASInterface_ReturnAds.h"
#include "CASInterface_Rewarded.h"
#include "CASMobileAds.h"
#include "Engine/World.h"

void UCAS::Initialize(FSubsystemCollectionBase &Collection) {
    Super::Initialize(Collection);

    GeneralInterface = NewObject<UCASInterface_General>(this);
}

void UCAS::Init() {
    UCASMobileAds::OnAdsInitialized.AddUObject(this, &UCAS::HandleInitialized);
    UCASMobileAds::InitializeMobileAds();
}

UCAS *UCAS::Get(UObject *WorldContext) {
    if (!WorldContext) return nullptr;

    if (const UCASInterface *CASInterface = Cast<UCASInterface>(WorldContext)) {
        return Cast<UCAS>(CASInterface->GetOuter());
    }

    UWorld *World = WorldContext->GetWorld();
    if (!World || !World->GetGameInstance()) return nullptr;

    return World->GetGameInstance()->GetSubsystem<UCAS>();
}

bool UCAS::IsInitialized() const { return UCASMobileAds::IsInitializedAds(); }

UCASInterface_General *UCAS::GetGeneralInterface() const { return GeneralInterface; }

UCASInterface_Interstitial *UCAS::GetInterstitialInterface() {
    if (InterstitialInterface == nullptr) {
        InterstitialInterface = NewObject<UCASInterface_Interstitial>(this);
    }
    return InterstitialInterface;
}

UCASInterface_Rewarded *UCAS::GetRewardedInterface() {
    if (RewardedInterface == nullptr) {
        RewardedInterface = NewObject<UCASInterface_Rewarded>(this);
    }
    return RewardedInterface;
}

UCASInterface_Banner *UCAS::GetBannerInterface() {
    if (BannerInterface == nullptr) {
        BannerInterface = NewObject<UCASInterface_Banner>(this);
    }
    return BannerInterface;
}

UCASInterface_ReturnAds *UCAS::GetReturnAdsInterface() {
    if (ReturnAdsInterface == nullptr) {
        ReturnAdsInterface = NewObject<UCASInterface_ReturnAds>(this);
    }
    return ReturnAdsInterface;
}

void UCAS::HandleInitialized(const FCASInitialConfig &Config) { OnInitialized.Broadcast(); }
