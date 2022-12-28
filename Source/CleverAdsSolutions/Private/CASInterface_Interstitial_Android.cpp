// Fill out your copyright notice in the Description page of Project Settings.

#if PLATFORM_ANDROID

#include "CASInterface_Interstitial_Android.h"

#include "Android/AndroidApplication.h"
#include "Android/AndroidJNI.h"
#include <android_native_app_glue.h>

#include "Async/Async.h"

UCASInterface_Interstitial_Android* CASIntestitialAndroid = nullptr;

// ---- JNI Helpers

#define CLASSNAME_INTER "com/unreal/cas/CASUnrealInterstitial"

struct FJNIMethodInfo
{
	JNIEnv* Env;
	jclass Class;
	jmethodID Method;
};

FJNIMethodInfo GetJNIMethodInfo(const char* ClassName, const char* MethodName, const char* MethodSignature)
{
	FJNIMethodInfo OutInfo;

	OutInfo.Env = FAndroidApplication::GetJavaEnv();
	if (OutInfo.Env)
	{
		OutInfo.Class = FAndroidApplication::FindJavaClassGlobalRef(ClassName);
		if(OutInfo.Class)
		{
			OutInfo.Method = FJavaWrapper::FindStaticMethod(OutInfo.Env, OutInfo.Class, MethodName, MethodSignature, false);
		}
	}

	return OutInfo;
}

void UCASInterface_Interstitial_Android::Init()
{
	CASIntestitialAndroid = this;
}

void UCASInterface_Interstitial_Android::Show()
{
	FJNIMethodInfo MethodInfo = GetJNIMethodInfo(
		CLASSNAME_INTER,
		"ShowInterstitial",
		"()V");

	MethodInfo.Env->CallStaticVoidMethod(MethodInfo.Class, MethodInfo.Method);
}

bool UCASInterface_Interstitial_Android::IsReady()
{
	FJNIMethodInfo MethodInfo = GetJNIMethodInfo(
		CLASSNAME_INTER,
		"IsInterstitialReady",
		"()Z");

	return MethodInfo.Env->CallStaticBooleanMethod(MethodInfo.Class, MethodInfo.Method);
}

// ---- JNI Callbacks

JNI_METHOD void Java_com_unreal_cas_CASUnrealInterstitial_onInterstitialAdLoadedThunkCpp(JNIEnv* jenv, jobject thiz)
{
	if(!CASIntestitialAndroid) return;
	
	AsyncTask(ENamedThreads::GameThread, []()
	{
		CASIntestitialAndroid->OnInterstitialLoaded.Broadcast();
	});
}

JNI_METHOD void Java_com_unreal_cas_CASUnrealInterstitial_onInterstitialAdShownThunkCpp(JNIEnv* jenv, jobject thiz)
{
	if(!CASIntestitialAndroid) return;
	
	AsyncTask(ENamedThreads::GameThread, []()
	{
		CASIntestitialAndroid->OnInterstitialShown.Broadcast();
	});
}

JNI_METHOD void Java_com_unreal_cas_CASUnrealInterstitial_onInterstitialAdClosedThunkCpp(JNIEnv* jenv, jobject thiz)
{
	if(!CASIntestitialAndroid) return;
	
	AsyncTask(ENamedThreads::GameThread, []()
	{
		CASIntestitialAndroid->OnInterstitialClosed.Broadcast();
	});
}

JNI_METHOD void Java_com_unreal_cas_CASUnrealInterstitial_onInterstitialAdClickedThunkCpp(JNIEnv* jenv, jobject thiz)
{
	if(!CASIntestitialAndroid) return;
	
	AsyncTask(ENamedThreads::GameThread, []()
	{
		CASIntestitialAndroid->OnInterstitialClicked.Broadcast();
	});
}

JNI_METHOD void Java_com_unreal_cas_CASUnrealInterstitial_onInterstitialAdCompletedThunkCpp(JNIEnv* jenv, jobject thiz)
{
	if(!CASIntestitialAndroid) return;
	
	AsyncTask(ENamedThreads::GameThread, []()
	{
		CASIntestitialAndroid->OnInterstitialCompleted.Broadcast();
	});
}

JNI_METHOD void Java_com_unreal_cas_CASUnrealInterstitial_onInterstitialAdLoadFailedThunkCpp(JNIEnv* jenv, jobject thiz, jstring errorMessage)
{
	if(!CASIntestitialAndroid) return;
	
	FString ErrorMsg = FJavaHelper::FStringFromParam(jenv, errorMessage);
	
	AsyncTask(ENamedThreads::GameThread, [ErrorMsg]()
	{
		CASIntestitialAndroid->OnInterstitialLoadError.Broadcast(ErrorMsg);
	});
}

JNI_METHOD void Java_com_unreal_cas_CASUnrealInterstitial_onInterstitialAdShowFailedThunkCpp(JNIEnv* jenv, jobject thiz, jstring errorMessage)
{
	if(!CASIntestitialAndroid) return;
	
	FString ErrorMsg = FJavaHelper::FStringFromParam(jenv, errorMessage);
	
	AsyncTask(ENamedThreads::GameThread, [ErrorMsg]()
	{
		CASIntestitialAndroid->OnInterstitialShowError.Broadcast(ErrorMsg);
	});
}

#endif