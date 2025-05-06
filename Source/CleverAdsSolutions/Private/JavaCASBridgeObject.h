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
    FJavaClassMethod LoadBannerAd;
    FJavaClassMethod SetBannerAdPosition;
    FJavaClassMethod SetBannerAdRefreshInterval;
    FJavaClassMethod SetMRecAdPosition;
    FJavaClassMethod SetMRecAdRefreshInterval;
    FJavaClassMethod SetInterstitialAdMinimumInterval;
    FJavaClassMethod RestartInterstitialAdInterval;
    FJavaClassMethod SetPluginConfig;
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
    FJavaClassMethod SetUserIDForAds;
    FJavaClassMethod GetUserConsentFlowStatus;
    FJavaClassMethod AddTestDeviceId;
    FJavaClassMethod SetAutoloadEnabled;
    FJavaClassMethod SetAutoshowEnabled;
    FJavaClassMethod IsAdLoaded;
    FJavaClassMethod LoadAd;
    FJavaClassMethod ShowAd;
    FJavaClassMethod HideAd;
    FJavaClassMethod DestroyAd;
};
#endif