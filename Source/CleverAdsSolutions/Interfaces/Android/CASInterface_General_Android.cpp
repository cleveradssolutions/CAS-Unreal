// Fill out your copyright notice in the Description page of Project Settings.

#if PLATFORM_ANDROID

#include "CASInterface_General_Android.h"

#include "JNIHelpers.h"

#include "CASSettings.h"

#include "Misc/EngineVersion.h"
#include "Async/Async.h"

#define MANAGER_CLASSNAME "com/unreal/cas/CASUnrealManager"

UCASInterface_General_Android* CASGeneralAndroid = nullptr;

void UCASInterface_General_Android::Init()
{
	Super::Init();

	CASGeneralAndroid = this;

	const UCASSettingsAndroid* CASSettings = GetDefault<UCASSettingsAndroid>();
	
	{ // Test devices
		CASJNIHelpers::FJNIMethodInfo MethodInfo = CASJNIHelpers::GetJNIMethodInfo(
			MANAGER_CLASSNAME,
			"addTestDevice",
			"(Ljava/lang/String;)V"
		);
		
		for(const FString& DeviceId : CASSettings->TestDevices)
		{
			jstring IDParam = MethodInfo.Env->NewStringUTF(TCHAR_TO_UTF8(*DeviceId));
			
			MethodInfo.Env->CallStaticVoidMethod(MethodInfo.Class, MethodInfo.Method, IDParam);

			MethodInfo.Env->DeleteLocalRef(IDParam);
		}
	}
	{ // Init
		CASJNIHelpers::FJNIMethodInfo MethodInfo = CASJNIHelpers::GetJNIMethodInfo(
		MANAGER_CLASSNAME,
		"Init",
		"(Ljava/lang/String;)V");
		
		const FEngineVersion& EngineVersion = FEngineVersion::Current();
		const FString VersionString = FString::Printf(TEXT("%u.%u"), EngineVersion.GetMajor(), EngineVersion.GetMinor());

		jstring VersionParam = MethodInfo.Env->NewStringUTF(TCHAR_TO_UTF8(*VersionString));
		
		MethodInfo.Env->CallStaticVoidMethod(MethodInfo.Class, MethodInfo.Method, VersionParam);

		MethodInfo.Env->DeleteLocalRef(VersionParam);
	}
	{ // Last page ad
		CASJNIHelpers::FJNIMethodInfo MethodInfo = CASJNIHelpers::GetJNIMethodInfo(
			MANAGER_CLASSNAME,
			"setLastPageAdInfo",
			"(Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;)V"
		);

		jstring HeadlineParam = MethodInfo.Env->NewStringUTF(TCHAR_TO_UTF8(*CASSettings->Headline));
		jstring AdTextParam = MethodInfo.Env->NewStringUTF(TCHAR_TO_UTF8(*CASSettings->AdText));
		jstring DestinationParam = MethodInfo.Env->NewStringUTF(TCHAR_TO_UTF8(*CASSettings->DestinationURL));
		jstring ImageParam = MethodInfo.Env->NewStringUTF(TCHAR_TO_UTF8(*CASSettings->ImageURL));
		jstring IconParam = MethodInfo.Env->NewStringUTF(TCHAR_TO_UTF8(*CASSettings->IconURL));

		MethodInfo.Env->CallStaticVoidMethod(MethodInfo.Class, MethodInfo.Method,
			HeadlineParam,
			AdTextParam,
			DestinationParam,
			ImageParam,
			IconParam
		);

		MethodInfo.Env->DeleteLocalRef(HeadlineParam);
		MethodInfo.Env->DeleteLocalRef(AdTextParam);
		MethodInfo.Env->DeleteLocalRef(DestinationParam);
		MethodInfo.Env->DeleteLocalRef(ImageParam);
		MethodInfo.Env->DeleteLocalRef(IconParam);
	}
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

// ---- JNI Callbacks

JNI_METHOD void Java_com_unreal_cas_CASUnrealManager_onCASInitThunkCpp(JNIEnv* jenv, jobject thiz)
{
	if(!CASGeneralAndroid) return;
	
	AsyncTask(ENamedThreads::GameThread, []()
	{
		CASGeneralAndroid->OnInitialized.Execute();
	});
}

#endif