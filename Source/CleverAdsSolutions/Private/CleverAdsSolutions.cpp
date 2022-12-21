// Copyright Epic Games, Inc. All Rights Reserved.

#include "CleverAdsSolutions.h"

#include "CASInterface.h"

#define LOCTEXT_NAMESPACE "FCleverAdsSolutionsModule"

void FCleverAdsSolutionsModule::StartupModule()
{
	CreateInterface();
}

void FCleverAdsSolutionsModule::ShutdownModule()
{
	
}

void FCleverAdsSolutionsModule::CreateInterface()
{
	UClass* CASInterfaceClass = nullptr;

#if PLATFORM_ANDROID
	CASInterfaceClass = UCASInterface_Android::StaticClass();
#endif

	if(!CASInterfaceClass) return;

	CASInterface = NewObject<UCASInterface>(GetTransientPackage(), CASInterfaceClass);
	CASInterface->AddToRoot();
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FCleverAdsSolutionsModule, CleverAdsSolutions)