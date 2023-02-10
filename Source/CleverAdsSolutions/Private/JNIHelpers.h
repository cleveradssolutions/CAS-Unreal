#pragma once

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

	inline FCASImpressionInfo ParseImpressionInfo(JNIEnv* Env, jobject Impression)
	{
		FCASImpressionInfo ImpressionInfo;

		if(!Env || !Impression) return ImpressionInfo;
		
		jclass Class = Env->GetObjectClass(Impression);

		if(!Class) return ImpressionInfo;

		{
			jmethodID Method = FJavaWrapper::FindMethod(Env, Class, "getCpm", "()D", false);
			ImpressionInfo.RevenueCPM = FJavaWrapper::CallDoubleMethod(Env, Impression, Method);
		}
		{
			jmethodID Method = FJavaWrapper::FindMethod(Env, Class, "getCreativeIdentifier", "()Ljava/lang/String;", false);
			jstring OutString = static_cast<jstring>(FJavaWrapper::CallObjectMethod(Env, Impression, Method));
			ImpressionInfo.CreativeID = FJavaHelper::FStringFromParam(Env, OutString);
		}
		{
			jmethodID Method = FJavaWrapper::FindMethod(Env, Class, "getNetwork", "()Ljava/lang/String;", false);
			jstring OutString = static_cast<jstring>(FJavaWrapper::CallObjectMethod(Env, Impression, Method));
			ImpressionInfo.Network = FJavaHelper::FStringFromParam(Env, OutString);
		}
		{
			jmethodID Method = FJavaWrapper::FindMethod(Env, Class, "getVersionInfo", "()Ljava/lang/String;", false);
			jstring OutString = static_cast<jstring>(FJavaWrapper::CallObjectMethod(Env, Impression, Method));
			ImpressionInfo.NetworkSDKVersion = FJavaHelper::FStringFromParam(Env, OutString);
		}
		{
			jmethodID Method = FJavaWrapper::FindMethod(Env, Class, "getIdentifier", "()Ljava/lang/String;", false);
			jstring OutString = static_cast<jstring>(FJavaWrapper::CallObjectMethod(Env, Impression, Method));
			ImpressionInfo.UnitID = FJavaHelper::FStringFromParam(Env, OutString);
		}
		{
			jmethodID Method = FJavaWrapper::FindMethod(Env, Class, "getAdType", "()Lcom/cleversolutions/ads/AdType;", false);
			jobject AdTypeObj = FJavaWrapper::CallObjectMethod(Env, Impression, Method);
			if(AdTypeObj)
			{
				jclass AdTypeClass = Env->GetObjectClass(AdTypeObj);
			
				jmethodID AdTypeMethod = FJavaWrapper::FindMethod(Env, AdTypeClass, "name", "()Ljava/lang/String;", false);
			
				jstring OutString = static_cast<jstring>(FJavaWrapper::CallObjectMethod(Env, AdTypeObj, AdTypeMethod));
			
				ImpressionInfo.PlacementType = FJavaHelper::FStringFromParam(Env, OutString);
			}
		}
		{
			jmethodID Method = FJavaWrapper::FindMethod(Env, Class, "getImpressionDepth", "()I", false);
			ImpressionInfo.TotalImpressions = FJavaWrapper::CallIntMethod(Env, Impression, Method);
		}
		{
			jmethodID Method = FJavaWrapper::FindMethod(Env, Class, "getLifetimeRevenue", "()D", false);
			ImpressionInfo.LifetimeRevenue = FJavaWrapper::CallDoubleMethod(Env, Impression, Method);
		}

		return ImpressionInfo;
	}
}

#endif
