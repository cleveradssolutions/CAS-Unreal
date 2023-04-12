﻿#pragma once

#if PLATFORM_ANDROID

#include "Android/AndroidApplication.h"
#include "Android/AndroidJNI.h"
#include <android_native_app_glue.h>

#include "CASDefines.h"

namespace CASJNIHelpers
{
	struct FJNIMethodInfo
	{
		JNIEnv* Env;
		jclass Class;
		jmethodID Method;
	};

	inline FJNIMethodInfo GetJNIMethodInfo(const char* ClassName, const char* MethodName, const char* MethodSignature)
	{
		FJNIMethodInfo OutInfo;

		OutInfo.Env = FAndroidApplication::GetJavaEnv();
		if (OutInfo.Env != NULL)
		{
			OutInfo.Class = FAndroidApplication::FindJavaClassGlobalRef(ClassName);
			if(OutInfo.Class != NULL)
			{
				OutInfo.Method = FJavaWrapper::FindStaticMethod(OutInfo.Env, OutInfo.Class, MethodName, MethodSignature, false);
			}
		}

		return OutInfo;
	}

	inline FCASImpressionInfo ParseImpressionInfo(JNIEnv* Env, jobject Impression)
	{
		FCASImpressionInfo ImpressionInfo;

		if(Env == NULL || Impression == NULL) return ImpressionInfo;
		
		jclass Class = Env->GetObjectClass(Impression);

		if(Class == NULL) return ImpressionInfo;

		{
			jmethodID Method = Env->GetMethodID(Class, "getCpm", "()D");
			if(Method != NULL) ImpressionInfo.RevenueCPM = FJavaWrapper::CallDoubleMethod(Env, Impression, Method);
		}
		{
			jmethodID Method = Env->GetMethodID(Class, "getCreativeIdentifier", "()Ljava/lang/String;");
			if(Method != NULL)
			{
				jstring OutString = static_cast<jstring>(FJavaWrapper::CallObjectMethod(Env, Impression, Method));
				ImpressionInfo.CreativeID = FJavaHelper::FStringFromParam(Env, OutString);
			}
		}
		{
			jmethodID Method = Env->GetMethodID(Class, "getNetwork", "()Ljava/lang/String;");
			if(Method != NULL)
			{
				jstring OutString = static_cast<jstring>(FJavaWrapper::CallObjectMethod(Env, Impression, Method));
				ImpressionInfo.Network = FJavaHelper::FStringFromParam(Env, OutString);
			}
		}
		{
			jmethodID Method = Env->GetMethodID(Class, "getVersionInfo", "()Ljava/lang/String;");
			if(Method != NULL)
			{
				jstring OutString = static_cast<jstring>(FJavaWrapper::CallObjectMethod(Env, Impression, Method));
				ImpressionInfo.NetworkSDKVersion = FJavaHelper::FStringFromParam(Env, OutString);
			}
		}
		{
			jmethodID Method = Env->GetMethodID(Class, "getIdentifier", "()Ljava/lang/String;");
			if(Method != NULL)
			{
				jstring OutString = static_cast<jstring>(FJavaWrapper::CallObjectMethod(Env, Impression, Method));
				ImpressionInfo.UnitID = FJavaHelper::FStringFromParam(Env, OutString);
			}
		}
		{
			jmethodID Method = Env->GetMethodID(Class, "getAdType", "()Lcom/cleversolutions/ads/AdType;");
			jobject AdTypeObj = nullptr;
			if(Method != NULL) AdTypeObj = FJavaWrapper::CallObjectMethod(Env, Impression, Method);
			if(AdTypeObj != NULL)
			{
				jclass AdTypeClass = Env->GetObjectClass(AdTypeObj);
			
				jmethodID AdTypeMethod = Env->GetMethodID(AdTypeClass, "name", "()Ljava/lang/String;");

				if(AdTypeMethod != NULL) {
					jstring OutString = static_cast<jstring>(FJavaWrapper::CallObjectMethod(Env, AdTypeObj, AdTypeMethod));
					ImpressionInfo.PlacementType = FJavaHelper::FStringFromParam(Env, OutString);
				}
			}
		}
		{
			jmethodID Method = Env->GetMethodID(Class, "getImpressionDepth", "()I");
			if(Method != NULL) ImpressionInfo.TotalImpressions = FJavaWrapper::CallIntMethod(Env, Impression, Method);
		}
		{
			jmethodID Method = Env->GetMethodID(Class, "getLifetimeRevenue", "()D");
			if(Method != NULL) ImpressionInfo.LifetimeRevenue = FJavaWrapper::CallDoubleMethod(Env, Impression, Method);
		}

		return ImpressionInfo;
	}
}

#endif
