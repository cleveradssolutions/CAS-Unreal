// Fill out your copyright notice in the Description page of Project Settings.

#if PLATFORM_ANDROID

#include "CASInterface_ReturnAds_Android.h"

#include "CASSettings.h"

#include "JNIHelpers.h"

#define RETURNADS_CLASSNAME "com/unreal/cas/CASUnrealReturnAds"

UCASInterface_ReturnAds_Android* CASReturnAdsAndroid = nullptr;

void UCASInterface_ReturnAds_Android::Init()
{
	CASReturnAdsAndroid = this;

	const UCASSettingsAndroid* CASSettings = GetDefault<UCASSettingsAndroid>();

	if(CASSettings->ReturnAds)
	{
		CASJNIHelpers::FJNIMethodInfo MethodInfo = CASJNIHelpers::GetJNIMethodInfo(
		RETURNADS_CLASSNAME,
		"enableRewardAds",
		"()V");

		MethodInfo.Env->CallStaticVoidMethod(MethodInfo.Class, MethodInfo.Method);
	}
}

void UCASInterface_ReturnAds_Android::SkipNextAppReturnAds()
{
	CASJNIHelpers::FJNIMethodInfo MethodInfo = CASJNIHelpers::GetJNIMethodInfo(
		RETURNADS_CLASSNAME,
		"skipNextAppReturnAds",
		"()V");

	MethodInfo.Env->CallStaticVoidMethod(MethodInfo.Class, MethodInfo.Method);
}

void UCASInterface_ReturnAds_Android::DisableAppReturnAds()
{
	CASJNIHelpers::FJNIMethodInfo MethodInfo = CASJNIHelpers::GetJNIMethodInfo(
		RETURNADS_CLASSNAME,
		"disableAppReturnAds",
		"()V");

	MethodInfo.Env->CallStaticVoidMethod(MethodInfo.Class, MethodInfo.Method);
}

JNI_METHOD void Java_com_unreal_cas_CASUnrealReturnAds_onReturnAdsAdShownThunkCpp(JNIEnv* jenv, jobject thiz, jobject impression)
{
	if(!CASReturnAdsAndroid) return;

	FCASImpressionInfo ImpressionInfo = CASJNIHelpers::ParseImpressionInfo(jenv, impression);
	
	AsyncTask(ENamedThreads::GameThread, [ImpressionInfo]()
	{
		CASReturnAdsAndroid->OnShown.Broadcast(ImpressionInfo);
	});
}

JNI_METHOD void Java_com_unreal_cas_CASUnrealReturnAds_onReturnAdsClosedThunkCpp(JNIEnv* jenv, jobject thiz)
{
	if(!CASReturnAdsAndroid) return;
	
	AsyncTask(ENamedThreads::GameThread, []()
	{
		CASReturnAdsAndroid->OnClosed.Broadcast();
	});
}

JNI_METHOD void Java_com_unreal_cas_CASUnrealReturnAds_onReturnAdsAdClickedThunkCpp(JNIEnv* jenv, jobject thiz)
{
	if(!CASReturnAdsAndroid) return;
	
	AsyncTask(ENamedThreads::GameThread, []()
	{
		CASReturnAdsAndroid->OnClicked.Broadcast();
	});
}

JNI_METHOD void Java_com_unreal_cas_CASUnrealReturnAds_onReturnAdsAdCompletedThunkCpp(JNIEnv* jenv, jobject thiz)
{
	if(!CASReturnAdsAndroid) return;
	
	AsyncTask(ENamedThreads::GameThread, []()
	{
		CASReturnAdsAndroid->OnCompleted.Broadcast();
	});
}

JNI_METHOD void Java_com_unreal_cas_CASUnrealReturnAds_onReturnAdShowFailedThunkCpp(JNIEnv* jenv, jobject thiz, jstring errorMessage)
{
	if(!CASReturnAdsAndroid) return;
	
	FString ErrorMsg = FJavaHelper::FStringFromParam(jenv, errorMessage);
	
	AsyncTask(ENamedThreads::GameThread, [ErrorMsg]()
	{
		CASReturnAdsAndroid->OnShowError.Broadcast(ErrorMsg);
	});
}

#endif
