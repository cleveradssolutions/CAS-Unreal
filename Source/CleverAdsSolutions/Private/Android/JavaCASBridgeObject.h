// Copyright CleverAdsSolutions LTD, CAS.AI. All Rights Reserved.

#pragma once

#if PLATFORM_ANDROID
#include "CoreMinimal.h"

#include "Android/AndroidJava.h"
#include "Android/AndroidApplication.h"
#include "Android/AndroidJNI.h"

class FJavaCASBridgeObject : public FJavaClassObject {
   public:
    FJavaCASBridgeObject();
    virtual ~FJavaCASBridgeObject();

    float CallFloatMethod(FJavaClassMethod Method, ...);

    FJavaClassMethod IsInitializedAds;
    FJavaClassMethod InitializeMobileAds;
    FJavaClassMethod GetMobileAdsVersion;
    FJavaClassMethod GetAdsErrorMessage;
    FJavaClassMethod ValidateAdsIntegration;
    FJavaClassMethod SetVerboseAdsLogs;
    FJavaClassMethod SetAdsMuted;
    FJavaClassMethod SetTrialAdFreeInterval;
    FJavaClassMethod SetUserAudienceForAds;
    FJavaClassMethod SetUserConsentForAds;
    FJavaClassMethod SetUserOptOutSaleForAds;
    FJavaClassMethod SetUserAgeForAds;
    FJavaClassMethod SetUserGenderForAds;
    FJavaClassMethod SetLocationCollectionEnabled;
    FJavaClassMethod SetMetaDataProcessingOptions;
    FJavaClassMethod IsBannerAdReady;
    FJavaClassMethod LoadBannerAd;
    FJavaClassMethod ShowBannerAd;
    FJavaClassMethod HideBannerAd;
    FJavaClassMethod SetBannerAdPosition;
    FJavaClassMethod DestroyBannerAd;
    FJavaClassMethod SetBannerAdRefreshInterval;
    FJavaClassMethod IsMRecAdReady;
    FJavaClassMethod LoadMRecAd;
    FJavaClassMethod ShowMRecAd;
    FJavaClassMethod HideMRecAd;
    FJavaClassMethod SetMRecAdPosition;
    FJavaClassMethod SetMRecAdRefreshInterval;
    FJavaClassMethod DestroyMRecAd;
    FJavaClassMethod LoadInterstitialAd;
    FJavaClassMethod IsInterstitialAdReady;
    FJavaClassMethod ShowInterstitialAd;
    FJavaClassMethod SetInterstitialAdMinimumInterval;
    FJavaClassMethod RestartInterstitialAdInterval;
    FJavaClassMethod LoadRewardedAd;
    FJavaClassMethod IsRewardedAdReady;
    FJavaClassMethod ShowRewardedAd;
    FJavaClassMethod ShowAdOnReturnToApp;
    FJavaClassMethod SkipAdOnNextReturnToApp;
    FJavaClassMethod DestroyReturnToAppAd;
    FJavaClassMethod SetPluginConfig;
    FJavaClassMethod SetAutoloadFormats;
    FJavaClassMethod SetTestAdModeForInit;
    FJavaClassMethod SetInitializationExtras;
    FJavaClassMethod SetConsentFlowEnabled;
    FJavaClassMethod ShowConsentFlow;
    FJavaClassMethod GetImpressionCPM;
    FJavaClassMethod GetImpressionNetwork;
    FJavaClassMethod GetImpressionAdUnit;
    FJavaClassMethod GetImpressionCreative;
    FJavaClassMethod GetImpressionDepth;
    FJavaClassMethod GetImpressionLifetimeRevenue;
    FJavaClassMethod GetInitializationError;
    FJavaClassMethod GetUserCountryCode;
    FJavaClassMethod GetUserConsentForAds;
    FJavaClassMethod IsUserConsentRequired;
    FJavaClassMethod AddTestDeviceId;
};
#endif