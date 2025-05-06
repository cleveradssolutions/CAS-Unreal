// Copyright CleverAdsSolutions LTD, CAS.AI. All Rights Reserved.

#if PLATFORM_ANDROID
#include "JavaCASBridgeObject.h"

#define BRIDGE_CLASS_NAME "com/cleveradssolutions/plugin/unreal/CASBridge"
#define SIG_VOID__ACTIVITY "(Landroid/app/Activity;)V"
#define SIG_VOID__VOID "()V"
#define SIG_VOID__BOOL "(Z)V"
#define SIG_VOID__INT "(I)V"
#define SIG_VOID__INT_BOOL "(IZ)V"
#define SIG_VOID__INT_INT_INT "(III)V"
#define SIG_BOOL__VOID "()Z"
#define SIG_BOOL__INT "(I)Z"
#define SIG_INT__VOID "()I"
#define SIG_INT__INT "(I)I"
#define SIG_FLOAT__INT "(I)F"
#define SIG_STRING__INT "(I)Ljava/lang/String;"
#define SIG_STRING__VOID "()Ljava/lang/String;"
#define SIG_VOID__STRING "(Ljava/lang/String;)V"
#define SIG_VOID__BOOL_INT_STRING "(ZILjava/lang/String;)V"
#define SIG_VOID__STRING_STRING "(Ljava/lang/String;Ljava/lang/String;)V"

FJavaCASBridgeObject::FJavaCASBridgeObject()
    : FJavaClassObject(FName(BRIDGE_CLASS_NAME), SIG_VOID__ACTIVITY, FAndroidApplication::GetGameActivityThis()),
      IsInitializedAds(GetClassMethod("isInitializedAds", SIG_BOOL__VOID)),
      InitializeMobileAds(GetClassMethod("initializeMobileAds", SIG_VOID__VOID)),
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
      SetLocationCollectionEnabled(GetClassMethod("setLocationCollectionEnabled", SIG_VOID__BOOL)),
      SetMetaDataProcessingOptions(GetClassMethod("setMetaDataProcessingOptions", SIG_VOID__INT)),
      LoadBannerAd(GetClassMethod("loadBannerAd", SIG_VOID__INT)),
      SetBannerAdPosition(GetClassMethod("setBannerAdPosition", SIG_VOID__INT_INT_INT)),
      SetBannerAdRefreshInterval(GetClassMethod("setBannerAdRefreshInterval", SIG_VOID__INT)),
      SetMRecAdPosition(GetClassMethod("setMRecAdPosition", SIG_VOID__INT_INT_INT)),
      SetMRecAdRefreshInterval(GetClassMethod("setMRecAdRefreshInterval", SIG_VOID__INT)),
      SetInterstitialAdMinimumInterval(GetClassMethod("setInterstitialAdMinimumInterval", SIG_VOID__INT)),
      RestartInterstitialAdInterval(GetClassMethod("restartInterstitialAdInterval", SIG_VOID__VOID)),
      SetPluginConfig(GetClassMethod("setPluginConfig", SIG_VOID__STRING_STRING)),
      SetTestAdModeForInit(GetClassMethod("setTestAdModeForInit", SIG_VOID__VOID)),
      SetInitializationExtras(GetClassMethod("setInitializationExtras", SIG_VOID__STRING_STRING)),
      SetConsentFlowEnabled(GetClassMethod("setConsentFlowEnabled", SIG_VOID__BOOL_INT_STRING)),
      ShowConsentFlow(GetClassMethod("showConsentFlow", SIG_VOID__BOOL)),
      GetImpressionCPM(GetClassMethod("getImpressionCPM", SIG_FLOAT__INT)),
      GetImpressionNetwork(GetClassMethod("getImpressionNetwork", SIG_STRING__INT)),
      GetImpressionAdUnit(GetClassMethod("getImpressionAdUnit", SIG_STRING__INT)),
      GetImpressionCreative(GetClassMethod("getImpressionCreative", SIG_STRING__INT)),
      GetImpressionDepth(GetClassMethod("getImpressionDepth", SIG_INT__INT)),
      GetImpressionLifetimeRevenue(GetClassMethod("getImpressionLifetimeRevenue", SIG_FLOAT__INT)),
      GetInitializationError(GetClassMethod("getInitializationError", SIG_STRING__VOID)),
      GetUserCountryCode(GetClassMethod("getUserCountryCode", SIG_STRING__VOID)),
      GetUserConsentForAds(GetClassMethod("getUserConsentForAds", SIG_INT__VOID)),
      IsUserConsentRequired(GetClassMethod("isUserConsentRequired", SIG_BOOL__VOID)),
      GetUserConsentFlowStatus(GetClassMethod("getUserConsentFlowStatus", SIG_INT__VOID)),
      SetUserIDForAds(GetClassMethod("setUserIDForAds", SIG_VOID__STRING)),
      AddTestDeviceId(GetClassMethod("addTestDeviceId", SIG_VOID__STRING)),
      SetAutoloadEnabled(GetClassMethod("setAutoloadEnabled", SIG_VOID__INT_BOOL)),
      SetAutoshowEnabled(GetClassMethod("setAutoshowEnabled", SIG_VOID__INT_BOOL)),
      IsAdLoaded(GetClassMethod("isAdLoaded", SIG_BOOL__INT)),
      LoadAd(GetClassMethod("loadAd", SIG_VOID__INT)),
      ShowAd(GetClassMethod("showAd", SIG_VOID__INT)),
      HideAd(GetClassMethod("hideAd", SIG_VOID__INT)),
      DestroyAd(GetClassMethod("destroyAd", SIG_VOID__INT)) {}

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