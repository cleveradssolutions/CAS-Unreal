// Copyright CleverAdsSolutions LTD, CAS.AI. All Rights Reserved.

#include "CASConfigContainer.h"
#include "UObject/UObjectGlobals.h"

#if WITH_EDITOR
UCASConfigContainer* UCASConfigContainer::Get() {
    static const TCHAR* SettingsContainerName = TEXT("CASConfigContainer");

    auto* Outer = FindObject<UCASConfigContainer>((UObject*)GetTransientPackage(), SettingsContainerName);
    if (!Outer) {
        Outer = NewObject<UCASConfigContainer>((UObject*)GetTransientPackage(), UCASConfigContainer::StaticClass(),
                                               SettingsContainerName);
        Outer->AddToRoot();
    }
    return Outer;
}

UCASDefaultConfig* UCASConfigContainer::GetAndroid() {
    if (!ConfigAndroid) {
        ConfigAndroid = NewObject<UCASDefaultConfig>((UObject*)GetTransientPackage());
        ConfigAndroid->ConfigPlatformId = 1;
        ConfigAndroid->LoadConfig();
        ConfigAndroid->AddToRoot();
    }
    return ConfigAndroid;
}

UCASDefaultConfig* UCASConfigContainer::GetIOS() {
    if (!ConfigIOS) {
        ConfigIOS = NewObject<UCASDefaultConfig>((UObject*)GetTransientPackage());
        ConfigIOS->ConfigPlatformId = 2;
        ConfigIOS->LoadConfig();
        ConfigIOS->AddToRoot();
    }
    return ConfigIOS;
}
#endif

const UCASDefaultConfig* UCASConfigContainer::GetConfig() {
#if WITH_EDITOR
    return Get()->GetAndroid();
#else
    return GetDefault<UCASDefaultConfig>();
#endif
}