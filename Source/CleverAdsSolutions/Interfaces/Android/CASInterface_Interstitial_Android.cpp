// Fill out your copyright notice in the Description page of Project Settings.

#if PLATFORM_ANDROID

#include "CASInterface_Interstitial_Android.h"

#include "JNIHelpers.h"

#include "Async/Async.h"

#include "CASSettings.h"

UCASInterface_Interstitial_Android* CASInterstitialAndroid = nullptr;

#define INTER_CLASSNAME "com/unreal/cas/CASUnrealInterstitial"

void UCASInterface_Interstitial_Android::PreInit()
{
	CASInterstitialAndroid = this;

	const UCASSettingsAndroid* CASSettings = GetDefault<UCASSettingsAndroid>();
	SetMinimumInterval(CASSettings->MinimumInterstitialInterval);
}

void UCASInterface_Interstitial_Android::Show()
{
	CASJNIHelpers::FJNIMethodInfo MethodInfo = CASJNIHelpers::GetJNIMethodInfo(
		INTER_CLASSNAME,
		"ShowInterstitial",
		"()V");

	MethodInfo.Env->CallStaticVoidMethod(MethodInfo.Class, MethodInfo.Method);
}

bool UCASInterface_Interstitial_Android::IsReady()
{
	CASJNIHelpers::FJNIMethodInfo MethodInfo = CASJNIHelpers::GetJNIMethodInfo(
		INTER_CLASSNAME,
		"IsInterstitialReady",
		"()Z");

	return MethodInfo.Env->CallStaticBooleanMethod(MethodInfo.Class, MethodInfo.Method);
}

void UCASInterface_Interstitial_Android::Load()
{
	CASJNIHelpers::FJNIMethodInfo MethodInfo = CASJNIHelpers::GetJNIMethodInfo(
		INTER_CLASSNAME,
		"LoadInterstitial",
		"()V");

	MethodInfo.Env->CallStaticVoidMethod(MethodInfo.Class, MethodInfo.Method);
}

void UCASInterface_Interstitial_Android::SetMinimumInterval(int Interval)
{
	CASJNIHelpers::FJNIMethodInfo MethodInfo = CASJNIHelpers::GetJNIMethodInfo(
		INTER_CLASSNAME,
		"setMinimumInterval",
		"(I)V");

	MethodInfo.Env->CallStaticVoidMethod(MethodInfo.Class, MethodInfo.Method, Interval);
}

void UCASInterface_Interstitial_Android::RestartInterval()
{
	CASJNIHelpers::FJNIMethodInfo MethodInfo = CASJNIHelpers::GetJNIMethodInfo(
		INTER_CLASSNAME,
		"restartInterval",
		"()V");

	MethodInfo.Env->CallStaticVoidMethod(MethodInfo.Class, MethodInfo.Method);
}

// ---- JNI Callbacks

JNI_METHOD void Java_com_unreal_cas_CASUnrealInterstitial_onInterstitialAdLoadedThunkCpp(JNIEnv* jenv, jobject thiz)
{
	if(!CASInterstitialAndroid) return;
	
	AsyncTask(ENamedThreads::GameThread, []()
	{
		CASInterstitialAndroid->OnLoaded.Broadcast();
	});
}

JNI_METHOD void Java_com_unreal_cas_CASUnrealInterstitial_onInterstitialAdShownThunkCpp(JNIEnv* jenv, jobject thiz, jobject impression)
{
	if(!CASInterstitialAndroid) return;

	FCASImpressionInfo ImpressionInfo = CASJNIHelpers::ParseImpressionInfo(jenv, impression);
	
	AsyncTask(ENamedThreads::GameThread, [ImpressionInfo]()
	{
		CASInterstitialAndroid->OnShown.Broadcast(ImpressionInfo);
	});
}

JNI_METHOD void Java_com_unreal_cas_CASUnrealInterstitial_onInterstitialAdClosedThunkCpp(JNIEnv* jenv, jobject thiz)
{
	if(!CASInterstitialAndroid) return;
	
	AsyncTask(ENamedThreads::GameThread, []()
	{
		CASInterstitialAndroid->OnClosed.Broadcast();
	});
}

JNI_METHOD void Java_com_unreal_cas_CASUnrealInterstitial_onInterstitialAdClickedThunkCpp(JNIEnv* jenv, jobject thiz)
{
	if(!CASInterstitialAndroid) return;
	
	AsyncTask(ENamedThreads::GameThread, []()
	{
		CASInterstitialAndroid->OnClicked.Broadcast();
	});
}

JNI_METHOD void Java_com_unreal_cas_CASUnrealInterstitial_onInterstitialAdCompletedThunkCpp(JNIEnv* jenv, jobject thiz)
{
	if(!CASInterstitialAndroid) return;
	
	AsyncTask(ENamedThreads::GameThread, []()
	{
		CASInterstitialAndroid->OnCompleted.Broadcast();
	});
}

JNI_METHOD void Java_com_unreal_cas_CASUnrealInterstitial_onInterstitialAdLoadFailedThunkCpp(JNIEnv* jenv, jobject thiz, jstring errorMessage)
{
	if(!CASInterstitialAndroid) return;
	
	FString ErrorMsg = FJavaHelper::FStringFromParam(jenv, errorMessage);
	
	AsyncTask(ENamedThreads::GameThread, [ErrorMsg]()
	{
		CASInterstitialAndroid->OnLoadError.Broadcast(ErrorMsg);
	});
}

JNI_METHOD void Java_com_unreal_cas_CASUnrealInterstitial_onInterstitialAdShowFailedThunkCpp(JNIEnv* jenv, jobject thiz, jstring errorMessage)
{
	if(!CASInterstitialAndroid) return;
	
	FString ErrorMsg = FJavaHelper::FStringFromParam(jenv, errorMessage);
	
	AsyncTask(ENamedThreads::GameThread, [ErrorMsg]()
	{
		CASInterstitialAndroid->OnShowError.Broadcast(ErrorMsg);
	});
}

#endif