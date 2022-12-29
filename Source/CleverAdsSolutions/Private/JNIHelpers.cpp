#pragma once

#if PLATFORM_ANDROID

#include "Android/AndroidApplication.h"
#include "Android/AndroidJNI.h"
#include <android_native_app_glue.h>

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

#endif
