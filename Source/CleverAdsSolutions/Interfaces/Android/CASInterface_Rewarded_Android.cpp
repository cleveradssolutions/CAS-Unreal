// Fill out your copyright notice in the Description page of Project Settings.

#if PLATFORM_ANDROID

#include "CASInterface_Rewarded_Android.h"

#include "JNIHelpers.h"

#include "Async/Async.h"

UCASInterface_Rewarded_Android* CASRewardedAndroid = nullptr;

#define REWARDED_CLASSNAME "com/unreal/cas/CASUnrealRewarded"

void UCASInterface_Rewarded_Android::Init()
{
	CASRewardedAndroid = this;
}

void UCASInterface_Rewarded_Android::Show()
{
	CASJNIHelpers::FJNIMethodInfo MethodInfo = CASJNIHelpers::GetJNIMethodInfo(
		REWARDED_CLASSNAME,
		"ShowRewarded",
		"()V");

	MethodInfo.Env->CallStaticVoidMethod(MethodInfo.Class, MethodInfo.Method);
}

bool UCASInterface_Rewarded_Android::IsReady()
{
	CASJNIHelpers::FJNIMethodInfo MethodInfo = CASJNIHelpers::GetJNIMethodInfo(
		REWARDED_CLASSNAME,
		"IsRewardedReady",
		"()Z");

	return MethodInfo.Env->CallStaticBooleanMethod(MethodInfo.Class, MethodInfo.Method);
}

void UCASInterface_Rewarded_Android::Load()
{
	CASJNIHelpers::FJNIMethodInfo MethodInfo = CASJNIHelpers::GetJNIMethodInfo(
		REWARDED_CLASSNAME,
		"LoadRewarded",
		"()V");

	MethodInfo.Env->CallStaticBooleanMethod(MethodInfo.Class, MethodInfo.Method);
}

// ---- JNI Callbacks

JNI_METHOD void Java_com_unreal_cas_CASUnrealRewarded_onRewardedAdLoadedThunkCpp(JNIEnv* jenv, jobject thiz)
{
	if(!CASRewardedAndroid) return;
	
	AsyncTask(ENamedThreads::GameThread, []()
	{
		CASRewardedAndroid->OnLoaded.Broadcast();
	});
}

JNI_METHOD void Java_com_unreal_cas_CASUnrealRewarded_onRewardedAdShownThunkCpp(JNIEnv* jenv, jobject thiz)
{
	if(!CASRewardedAndroid) return;
	
	AsyncTask(ENamedThreads::GameThread, []()
	{
		CASRewardedAndroid->OnShown.Broadcast();
	});
}

JNI_METHOD void Java_com_unreal_cas_CASUnrealRewarded_onRewardedAdClosedThunkCpp(JNIEnv* jenv, jobject thiz)
{
	if(!CASRewardedAndroid) return;
	
	AsyncTask(ENamedThreads::GameThread, []()
	{
		CASRewardedAndroid->OnClosed.Broadcast();
	});
}

JNI_METHOD void Java_com_unreal_cas_CASUnrealRewarded_onRewardedAdClickedThunkCpp(JNIEnv* jenv, jobject thiz)
{
	if(!CASRewardedAndroid) return;
	
	AsyncTask(ENamedThreads::GameThread, []()
	{
		CASRewardedAndroid->OnClicked.Broadcast();
	});
}

JNI_METHOD void Java_com_unreal_cas_CASUnrealRewarded_onRewardedAdCompletedThunkCpp(JNIEnv* jenv, jobject thiz)
{
	if(!CASRewardedAndroid) return;
	
	AsyncTask(ENamedThreads::GameThread, []()
	{
		CASRewardedAndroid->OnCompleted.Broadcast();
	});
}

JNI_METHOD void Java_com_unreal_cas_CASUnrealRewarded_onRewardedAdLoadFailedThunkCpp(JNIEnv* jenv, jobject thiz, jstring errorMessage)
{
	if(!CASRewardedAndroid) return;
	
	FString ErrorMsg = FJavaHelper::FStringFromParam(jenv, errorMessage);
	
	AsyncTask(ENamedThreads::GameThread, [ErrorMsg]()
	{
		CASRewardedAndroid->OnLoadError.Broadcast(ErrorMsg);
	});
}

JNI_METHOD void Java_com_unreal_cas_CASUnrealRewarded_onRewardedAdShowFailedThunkCpp(JNIEnv* jenv, jobject thiz, jstring errorMessage)
{
	if(!CASRewardedAndroid) return;
	
	FString ErrorMsg = FJavaHelper::FStringFromParam(jenv, errorMessage);
	
	AsyncTask(ENamedThreads::GameThread, [ErrorMsg]()
	{
		CASRewardedAndroid->OnShowError.Broadcast(ErrorMsg);
	});
}

#endif