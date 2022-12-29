// Fill out your copyright notice in the Description page of Project Settings.

#if PLATFORM_ANDROID

#include "CASInterface_Banner_Android.h"

#include "JNIHelpers.cpp"

#include "Async/Async.h"

UCASInterface_Banner_Android* CASBannerAndroid = nullptr;

#define BANNER_CLASSNAME "com/unreal/cas/CASUnrealBanner"

void UCASInterface_Banner_Android::Init()
{
	CASBannerAndroid = this;
}

void UCASInterface_Banner_Android::CreateBanner()
{
	FJNIMethodInfo MethodInfo = GetJNIMethodInfo(BANNER_CLASSNAME, "CreateBanner", "()V");

	MethodInfo.Env->CallStaticVoidMethod(MethodInfo.Class, MethodInfo.Method);
}

bool UCASInterface_Banner_Android::IsReady() const
{
	FJNIMethodInfo MethodInfo = GetJNIMethodInfo(BANNER_CLASSNAME, "IsBannerReady", "()Z");

	return MethodInfo.Env->CallStaticBooleanMethod(MethodInfo.Class, MethodInfo.Method);
}

void UCASInterface_Banner_Android::ToggleBannerVisibility(bool Visible)
{
	FJNIMethodInfo MethodInfo = GetJNIMethodInfo(BANNER_CLASSNAME, "ToggleBannerVisibility", "(Z)V");

	MethodInfo.Env->CallStaticVoidMethod(MethodInfo.Class, MethodInfo.Method, Visible);
}

void UCASInterface_Banner_Android::DestroyBanner()
{
	FJNIMethodInfo MethodInfo = GetJNIMethodInfo(BANNER_CLASSNAME, "DestroyBanner", "()V");

	MethodInfo.Env->CallStaticVoidMethod(MethodInfo.Class, MethodInfo.Method);
}

void UCASInterface_Banner_Android::LoadNextBanner()
{
	FJNIMethodInfo MethodInfo = GetJNIMethodInfo(BANNER_CLASSNAME, "LoadNextAd", "()V");

	MethodInfo.Env->CallStaticVoidMethod(MethodInfo.Class, MethodInfo.Method);
}

bool UCASInterface_Banner_Android::IsVisible() const
{
	FJNIMethodInfo MethodInfo = GetJNIMethodInfo(BANNER_CLASSNAME, "IsBannerVisible", "()Z");

	return MethodInfo.Env->CallStaticBooleanMethod(MethodInfo.Class, MethodInfo.Method);
}

// ---- JNI Callbacks

JNI_METHOD void Java_com_unreal_cas_CASUnrealBanner_onBannerAdLoadedThunkCpp(JNIEnv* jenv, jobject thiz)
{
	if(!CASBannerAndroid) return;
	
	AsyncTask(ENamedThreads::GameThread, []()
	{
		CASBannerAndroid->OnLoaded.Broadcast();
	});
}

JNI_METHOD void Java_com_unreal_cas_CASUnrealBanner_onBannerAdShownThunkCpp(JNIEnv* jenv, jobject thiz)
{
	if(!CASBannerAndroid) return;
	
	AsyncTask(ENamedThreads::GameThread, []()
	{
		CASBannerAndroid->OnCreated.Broadcast();
	});
}

JNI_METHOD void Java_com_unreal_cas_CASUnrealBanner_onBannerAdClickedThunkCpp(JNIEnv* jenv, jobject thiz)
{
	if(!CASBannerAndroid) return;
	
	AsyncTask(ENamedThreads::GameThread, []()
	{
		CASBannerAndroid->OnClicked.Broadcast();
	});
}

JNI_METHOD void Java_com_unreal_cas_CASUnrealBanner_onBannerAdDestroyedThunkCpp(JNIEnv* jenv, jobject thiz)
{
	if(!CASBannerAndroid) return;
	
	AsyncTask(ENamedThreads::GameThread, []()
	{
		CASBannerAndroid->OnDestroyed.Broadcast();
	});
}

JNI_METHOD void Java_com_unreal_cas_CASUnrealBanner_onBannerAdShowFailedThunkCpp(JNIEnv* jenv, jobject thiz, jstring errorMessage)
{
	if(!CASBannerAndroid) return;
	
	FString ErrorMsg = FJavaHelper::FStringFromParam(jenv, errorMessage);
	
	AsyncTask(ENamedThreads::GameThread, [ErrorMsg]()
	{
		CASBannerAndroid->OnShowError.Broadcast(ErrorMsg);
	});
}

#endif