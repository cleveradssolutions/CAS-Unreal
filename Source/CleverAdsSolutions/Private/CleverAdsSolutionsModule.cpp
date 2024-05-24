// Copyright CleverAdsSolutions LTD, CAS.AI. All Rights Reserved.

#include "CleverAdsSolutionsModule.h"

#include "ISettingsModule.h"
#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "CASConfigContainer.h"

#define LOCTEXT_NAMESPACE "FCleverAdsSolutionsModule"

void FCleverAdsSolutionsModule::StartupModule() {
#if WITH_EDITOR
    ISettingsModule* SettingsModule = FModuleManager::GetModulePtr<ISettingsModule>("Settings");
    if (SettingsModule != nullptr) {
        
        UCASDefaultConfig* CASSettinsIOS = UCASConfigContainer::Get()->GetIOS();
        SettingsModule->RegisterSettings(
            "Project", "Plugins", "CleverAdsSolutionsIOS", LOCTEXT("CASSIOSettingsName", "CAS.AI Config - IOS"),
            LOCTEXT("CASSIOSettingsDescription", "Configure CAS.AI plugin for iOS Platform"), CASSettinsIOS);

        UCASDefaultConfig* CASSettinsAndroid = UCASConfigContainer::Get()->GetAndroid();
        SettingsModule->RegisterSettings(
            "Project", "Plugins", "CleverAdsSolutionsAndroid",
            LOCTEXT("CASAndroidSettingsName", "CAS.AI Config - Android"),
            LOCTEXT("CASAndroidSettingsDescription", "Configure CAS.AI plugin for Android Platform"),
            CASSettinsAndroid);
    }
#endif
}

void FCleverAdsSolutionsModule::ShutdownModule() {
#if WITH_EDITOR
    ISettingsModule* SettingsModule = FModuleManager::GetModulePtr<ISettingsModule>("Settings");
    if (SettingsModule != nullptr) {
        SettingsModule->UnregisterSettings("Project", "Plugins", "CleverAdsSolutionsAndroid");
        SettingsModule->UnregisterSettings("Project", "Plugins", "CleverAdsSolutionsIOS");
    }
#endif
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FCleverAdsSolutionsModule, CleverAdsSolutions)