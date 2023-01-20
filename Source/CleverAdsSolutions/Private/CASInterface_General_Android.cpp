// Fill out your copyright notice in the Description page of Project Settings.

#if PLATFORM_ANDROID

#include "CASInterface_General_Android.h"

#include "JNIHelpers.h"

#include "Async/Async.h"

#define MANAGER_CLASSNAME "com/unreal/cas/CASUnrealManager"

void UCASInterface_General_Android::Init()
{
	Super::Init();

	// We init everything on OnCreate in UPL > CASUnrealManager.java
}

FString UCASInterface_General_Android::GetCASVersion() const
{
	CASJNIHelpers::FJNIMethodInfo MethodInfo = CASJNIHelpers::GetJNIMethodInfo(
		MANAGER_CLASSNAME,
		"GetCASVersion",
		"()Ljava/lang/String;");

	jstring OutVersionString = static_cast<jstring>(MethodInfo.Env->CallStaticObjectMethod(MethodInfo.Class, MethodInfo.Method));

	return FJavaHelper::FStringFromParam(MethodInfo.Env, OutVersionString);
}

void UCASInterface_General_Android::ValidateIntegration()
{
	CASJNIHelpers::FJNIMethodInfo MethodInfo = CASJNIHelpers::GetJNIMethodInfo(
		MANAGER_CLASSNAME,
		"ValidateIntegration",
		"()V");

	MethodInfo.Env->CallStaticVoidMethod(MethodInfo.Class, MethodInfo.Method);
}

void UCASInterface_General_Android::SetMuteAdSounds(bool Mute)
{
	CASJNIHelpers::FJNIMethodInfo MethodInfo = CASJNIHelpers::GetJNIMethodInfo(
		MANAGER_CLASSNAME,
		"ToggleAdSoundMute",
		"(Z)V");

	MethodInfo.Env->CallStaticVoidMethod(MethodInfo.Class, MethodInfo.Method, Mute);
}

void UCASInterface_General_Android::SetLoadingMode(ECASLoadingMode Mode)
{
	CASJNIHelpers::FJNIMethodInfo MethodInfo = CASJNIHelpers::GetJNIMethodInfo(
		MANAGER_CLASSNAME,
		"SetLoadingMode",
		"(I)V");

	MethodInfo.Env->CallStaticVoidMethod(MethodInfo.Class, MethodInfo.Method, Mode);
}

void UCASInterface_General_Android::SetTaggedAudience(ECASChildrenAudience ChildrenAudienceStatus)
{
	CASJNIHelpers::FJNIMethodInfo MethodInfo = CASJNIHelpers::GetJNIMethodInfo(
		MANAGER_CLASSNAME,
		"SetTaggedAudience",
		"(I)V");

	MethodInfo.Env->CallStaticVoidMethod(MethodInfo.Class, MethodInfo.Method, ChildrenAudienceStatus);
}

void UCASInterface_General_Android::SetUserConsent(ECASUserConsentStatus ConsentStatus)
{
	CASJNIHelpers::FJNIMethodInfo MethodInfo = CASJNIHelpers::GetJNIMethodInfo(
		MANAGER_CLASSNAME,
		"SetUserConsent",
		"(Z)V");

	MethodInfo.Env->CallStaticVoidMethod(
		MethodInfo.Class,
		MethodInfo.Method,
		ConsentStatus == ECASUserConsentStatus::Accepted);
}

void UCASInterface_General_Android::SetCCPAStatus(ECASUserCCPAStatus CCPAStatus)
{
	CASJNIHelpers::FJNIMethodInfo MethodInfo = CASJNIHelpers::GetJNIMethodInfo(
		MANAGER_CLASSNAME,
		"SetCCPAStatus",
		"(Z)V");

	MethodInfo.Env->CallStaticVoidMethod(MethodInfo.Class, MethodInfo.Method, CCPAStatus == ECASUserCCPAStatus::OptInSale);
}

#endif