// Fill out your copyright notice in the Description page of Project Settings.

#if PLATFORM_ANDROID && WITH_CAS_FACEBOOK

#include "CASInterface_AudienceNetwork_Android.h"

#include "JNIHelpers.h"

void UCASInterface_AudienceNetwork_Android::SetDataProcessingOptions(ECASANDataProcessingOption Option)
{
	CASJNIHelpers::FJNIMethodInfo MethodInfo;
	
	switch(Option)
	{
	case ECASANDataProcessingOption::DisableLDU:
		MethodInfo = CASJNIHelpers::GetJNIMethodInfo(
			"com/unreal/facebook/CASUnrealFacebook",
			"disableLDU",
			"()V"
		);
		break;
	case ECASANDataProcessingOption::EnableLDU:
		MethodInfo = CASJNIHelpers::GetJNIMethodInfo(
			"com/unreal/facebook/CASUnrealFacebook",
			"enableLDU",
			"()V"
		);
		break;
	case ECASANDataProcessingOption::EnableLDU_1_1000:
		MethodInfo = CASJNIHelpers::GetJNIMethodInfo(
			"com/unreal/facebook/CASUnrealFacebook",
			"enableLDULocation",
			"()V"
		);
		break;
	default: break;
	}

	MethodInfo.Env->CallStaticVoidMethod(MethodInfo.Class, MethodInfo.Method);
}

#endif
