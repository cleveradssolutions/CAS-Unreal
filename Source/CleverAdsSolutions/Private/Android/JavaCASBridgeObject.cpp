// Copyright CleverAdsSolutions LTD, CAS.AI. All Rights Reserved.

#if PLATFORM_ANDROID
#include "JavaCASBridgeObject.h"

#define BRIDGE_CLASS_NAME "com/cleveradssolutions/plugin/unreal/CASBridge"
#define SIG_VOID__ACTIVITY "(Landroid/app/Activity;)V"
#define SIG_VOID__VOID "()V"
#define SIG_VOID__BOOL "(Z)V"
#define SIG_VOID__INT "(I)V"
#define SIG_VOID__INT_INT_INT "(III)V"
#define SIG_BOOL__VOID "()Z"
#define SIG_INT__VOID "()I"
#define SIG_INT__INT "(I)I"
#define SIG_FLOAT__INT "(I)F"
#define SIG_STRING__INT "(I)Ljava/lang/String;"
#define SIG_STRING__VOID "()Ljava/lang/String;"
#define SIG_VOID__STRING "(Ljava/lang/String;)V"
#define SIG_VOID__BOOL_STRING "(ZLjava/lang/String;)V"
#define SIG_VOID__STRING_STRING "(Ljava/lang/String;Ljava/lang/String;)V"
#define SIG_VOID__STRING_BOOL_BOOL_BOOL_BOOL "(Ljava/lang/String;ZZZZ)V"

FJavaCASBridgeObject::FJavaCASBridgeObject()
    : FJavaClassObject(FName(BRIDGE_CLASS_NAME), SIG_VOID__ACTIVITY, FAndroidApplication::GetGameActivityThis()),
      IsInitializedPlugin(GetClassMethod("isInitializedPlugin", SIG_BOOL__VOID)),
      IsInitializedAds(GetClassMethod("isInitializedAds", SIG_BOOL__VOID)),
      InitializeMobileAds(GetClassMethod("initializeMobileAds", SIG_VOID__STRING_BOOL_BOOL_BOOL_BOOL)),
      GetMobileAdsVersion(GetClassMethod("getMobileAdsVersion", SIG_STRING__VOID)),
      ValidateAdsIntegration(GetClassMethod("validateAdsIntegration", SIG_VOID__VOID)),
      SetVerboseAdsLogs(GetClassMethod("setVerboseAdsLogs", SIG_VOID__BOOL)),
      SetAdsMuted(GetClassMethod("setAdsMuted", SIG_VOID__BOOL)),
      SetTrialAdFreeInterval(GetClassMethod("setTrialAdFreeInterval", SIG_VOID__INT)),
      SetUserAudienceForAds(GetClassMethod("setUserAudienceForAds", SIG_VOID__INT)),
      SetUserConsentForAds(GetClassMethod("setUserConsentForAds", SIG_VOID__INT)),
      SetUserOptOutSaleForAds(GetClassMethod("setUserOptOutSaleForAds", SIG_VOID__INT)),
      SetUserAgeForAds(GetClassMethod("setUserAgeForAds", SIG_VOID__INT)),
      SetUserGenderForAds(GetClassMethod("setUserGenderForAds", SIG_VOID__INT)),
      SetMetaDataProcessingOptions(GetClassMethod("setMetaDataProcessingOptions", SIG_VOID__INT)),
      IsBannerAdReady(GetClassMethod("isBannerAdReady", SIG_BOOL__VOID)),
      LoadBannerAd(GetClassMethod("loadBannerAd", SIG_VOID__INT)),
      ShowBannerAd(GetClassMethod("showBannerAd", SIG_VOID__VOID)),
      HideBannerAd(GetClassMethod("hideBannerAd", SIG_VOID__VOID)),
      SetBannerAdPosition(GetClassMethod("setBannerAdPosition", SIG_VOID__INT_INT_INT)),
      DestroyBannerAd(GetClassMethod("destroyBannerAd", SIG_VOID__VOID)),
      SetBannerAdRefreshInterval(GetClassMethod("setBannerAdRefreshInterval", SIG_VOID__INT)),
      IsMRecAdReady(GetClassMethod("isMRecAdReady", SIG_BOOL__VOID)),
      LoadMRecAd(GetClassMethod("loadMRecAd", SIG_VOID__VOID)),
      ShowMRecAd(GetClassMethod("showMRecAd", SIG_VOID__VOID)),
      HideMRecAd(GetClassMethod("hideMRecAd", SIG_VOID__VOID)),
      SetMRecAdPosition(GetClassMethod("setMRecAdPosition", SIG_VOID__INT_INT_INT)),
      SetMRecAdRefreshInterval(GetClassMethod("setMRecAdRefreshInterval", SIG_VOID__INT)),
      DestroyMRecAd(GetClassMethod("destroyMRecAd", SIG_VOID__VOID)),
      LoadInterstitialAd(GetClassMethod("loadInterstitialAd", SIG_VOID__VOID)),
      IsInterstitialAdReady(GetClassMethod("isInterstitialAdReady", SIG_BOOL__VOID)),
      ShowInterstitialAd(GetClassMethod("showInterstitialAd", SIG_VOID__VOID)),
      SetInterstitialAdMinimumInterval(GetClassMethod("setInterstitialAdMinimumInterval", SIG_VOID__INT)),
      RestartInterstitialAdInterval(GetClassMethod("restartInterstitialAdInterval", SIG_VOID__VOID)),
      LoadRewardedAd(GetClassMethod("loadRewardedAd", SIG_VOID__VOID)),
      IsRewardedAdReady(GetClassMethod("isRewardedAdReady", SIG_BOOL__VOID)),
      ShowRewardedAd(GetClassMethod("showRewardedAd", SIG_VOID__VOID)),
      ShowAdOnReturnToApp(GetClassMethod("showAdOnReturnToApp", SIG_VOID__VOID)),
      SkipAdOnNextReturnToApp(GetClassMethod("skipAdOnNextReturnToApp", SIG_VOID__VOID)),
      DestroyReturnToAppAd(GetClassMethod("destroyReturnToAppAd", SIG_VOID__VOID)),
      SetTestAdModeForInit(GetClassMethod("setTestAdModeForInit", SIG_VOID__VOID)),
      SetUnrealVersion(GetClassMethod("setUnrealVersion", SIG_VOID__STRING)),
      SetInitializationExtras(GetClassMethod("setInitializationExtras", SIG_VOID__STRING_STRING)),
      SetConsentFlowEnabled(GetClassMethod("setConsentFlowEnabled", SIG_VOID__BOOL_STRING)),
      ShowConsentFlow(GetClassMethod("showConsentFlow", SIG_VOID__STRING)),
      GetImpressionCPM(GetClassMethod("getImpressionCPM", SIG_FLOAT__INT)),
      GetImpressionNetwork(GetClassMethod("getImpressionNetwork", SIG_STRING__INT)),
      GetImpressionAdUnit(GetClassMethod("getImpressionAdUnit", SIG_STRING__INT)),
      GetImpressionCreative(GetClassMethod("getImpressionCreative", SIG_STRING__INT)),
      GetImpressionDepth(GetClassMethod("getImpressionDepth", SIG_INT__INT)),
      GetImpressionLifetimeRevenue(GetClassMethod("getImpressionLifetimeRevenue", SIG_FLOAT__INT)),
      GetInitializationError(GetClassMethod("getInitializationError", SIG_STRING__VOID)),
      GetUserCountryCode(GetClassMethod("getUserCountryCode", SIG_STRING__VOID)),
      GetUserConsentForAds(GetClassMethod("getUserConsentForAds", SIG_INT__VOID)),
      AddTestDeviceId(GetClassMethod("addTestDeviceId", SIG_VOID__STRING)) {}

FJavaCASBridgeObject::~FJavaCASBridgeObject() {}

float FJavaCASBridgeObject::CallFloatMethod(FJavaClassMethod Method, ...)
{
    JNIEnv*	JEnv = AndroidJavaEnv::GetJavaEnv();
	va_list Params;
	va_start(Params, Method);
	jfloat Return = JEnv->CallFloatMethodV(GetJObject(), Method.Method, Params);
	va_end(Params);

	return (float)Return;
}

#endif