// Copyright Epic Games, Inc. All Rights Reserved.

#include "CleverAdsSolutions.h"

#include "CASSettings.h"
#include "ISettingsModule.h"
#include "CoreMinimal.h"

#define LOCTEXT_NAMESPACE "FCleverAdsSolutionsModule"

void FCleverAdsSolutionsModule::StartupModule()
{
	ISettingsModule* SettingsModule = FModuleManager::GetModulePtr<ISettingsModule>("Settings");
	if(!SettingsModule) return;
	
	CASSettingsIOS = NewObject<UCASSettingsIOS>(GetTransientPackage(), "CASSettingsIOS", RF_Standalone);
	CASSettingsIOS->AddToRoot();

	// IOS
	SettingsModule->RegisterSettings("Project", "Plugins", "CleverAdsSolutionsIOS",
		LOCTEXT("CASSettingsNameIOS", "Clever Ads Solutions IOS"),
		LOCTEXT("CASSettingsDescriptionIOS", "Configure Clever Ads Solutions plugin"),
		CASSettingsIOS);

	// Android
	CASSettingsAndroid = NewObject<UCASSettingsAndroid>(GetTransientPackage(), "CASSettingsAndroid", RF_Standalone);
	CASSettingsAndroid->AddToRoot();
	
	SettingsModule->RegisterSettings("Project", "Plugins", "CleverAdsSolutionsAndroid",
		LOCTEXT("CASSettingsNameAndroid", "Clever Ads Solutions Android"),
		LOCTEXT("CASSettingsDescriptionAndroid", "Configure Clever Ads Solutions plugin"),
		CASSettingsAndroid);
}

void FCleverAdsSolutionsModule::ShutdownModule()
{
	if (ISettingsModule* SettingsModule = FModuleManager::GetModulePtr<ISettingsModule>("Settings"))
	{
		SettingsModule->UnregisterSettings("Project", "Plugins", "CleverAdsSolutionsAndroid");
		SettingsModule->UnregisterSettings("Project", "Plugins", "CleverAdsSolutionsIOS");
	}

	if (!GExitPurge)
	{
		// If we're in exit purge, this object has already been destroyed
		CASSettingsAndroid->RemoveFromRoot();
		CASSettingsIOS->RemoveFromRoot();
	}
	else
	{
		CASSettingsAndroid = nullptr;
		CASSettingsIOS = nullptr;
	}
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FCleverAdsSolutionsModule, CleverAdsSolutions)