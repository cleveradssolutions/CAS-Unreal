// Copyright Epic Games, Inc. All Rights Reserved.

#include "CleverAdsSolutions.h"

#include "CASSettings.h"
#include "ISettingsModule.h"
#include "CoreMinimal.h"

#define LOCTEXT_NAMESPACE "FCleverAdsSolutionsModule"

void FCleverAdsSolutionsModule::StartupModule()
{
	CASSettings = NewObject<UCASSettings>(GetTransientPackage(), "CASSettings", RF_Standalone);
	CASSettings->AddToRoot();

	// Register settings
	if (ISettingsModule* SettingsModule = FModuleManager::GetModulePtr<ISettingsModule>("Settings"))
	{
		SettingsModule->RegisterSettings("Project", "Plugins", "CleverAdsSolutions",
			LOCTEXT("RuntimeSettingsName", "Clever Ads Solutions"),
			LOCTEXT("RuntimeSettingsDescription", "Configure Clever Ads Solutions plugin"),
			CASSettings);
	}
}

void FCleverAdsSolutionsModule::ShutdownModule()
{
	if (ISettingsModule* SettingsModule = FModuleManager::GetModulePtr<ISettingsModule>("Settings"))
	{
		SettingsModule->UnregisterSettings("Project", "Plugins", "CleverAdsSolutions");
	}

	if (!GExitPurge)
	{
		// If we're in exit purge, this object has already been destroyed
		CASSettings->RemoveFromRoot();
	}
	else
	{
		CASSettings = nullptr;
	}
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FCleverAdsSolutionsModule, CleverAdsSolutions)