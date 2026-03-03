// Copyright CleverAdsSolutions LTD, CAS.AI. All Rights Reserved.

#include "CASDefaultConfig.h"

#include "CASDefines.h"
#include "CoreGlobals.h"
#include "UObject/Package.h"

UCASDefaultConfig *UCASDefaultConfig::GetForPlatform(int32 ConfigPlatformId) {
    const UClass *SettingsClass = UCASDefaultConfig::StaticClass();
    const FString PlatformSettingsName =
        SettingsClass->GetName() + TEXT("_") + (ConfigPlatformId == 1 ? TEXT("Android") : TEXT("IOS"));

#if WITH_EDITOR
    auto *Existing = FindObject<UCASDefaultConfig>(GetTransientPackage(), *PlatformSettingsName);

    if (Existing) {
        return Existing;
    }
#endif

    UCASDefaultConfig *PlatformSettings =
        NewObject<UCASDefaultConfig>(GetTransientPackage(), SettingsClass, FName(*PlatformSettingsName));
    PlatformSettings->ConfigPlatformId = ConfigPlatformId;

#if WITH_EDITOR
    PlatformSettings->AddToRoot();
    PlatformSettings->LoadConfig();
#else
    // Note: Calling PostLoad will implicitly call LoadConfig.
    // Calling just LoadConfig does not call PostLoad.
    PlatformSettings->PostLoad();
#endif

    return PlatformSettings;
}

#if WITH_EDITOR
void UCASDefaultConfig::PostEditChangeProperty(FPropertyChangedEvent &PropertyChangedEvent) {
    const FName PropertyName =
        (PropertyChangedEvent.Property != nullptr) ? PropertyChangedEvent.Property->GetFName() : NAME_None;
    const FName MemberPropertyName =
        (PropertyChangedEvent.MemberProperty != nullptr) ? PropertyChangedEvent.MemberProperty->GetFName() : NAME_None;

    if (PropertyName == GET_MEMBER_NAME_CHECKED(UCASDefaultConfig, Audience)) {
        if (Audience != ECASAudience::Undefined) {
            UseAdvertisingId = Audience == ECASAudience::NotChildren;
            UpdateSinglePropertyInConfigFile(
                GetClass()->FindPropertyByName(GET_MEMBER_NAME_CHECKED(UCASDefaultConfig, UseAdvertisingId)),
                GetDefaultConfigFilename());
        }
    } else if (PropertyName == GET_MEMBER_NAME_CHECKED(UCASDefaultConfig, IncludeFamiliesAds)) {
        // Begin Families Ads solution - autogeneration tag
		if (ConfigPlatformId == 1) {
			IncludeGoogleAds = IncludeOptimalAds || IncludeFamiliesAds;
			IncludeLiftoffMonetize = IncludeOptimalAds || IncludeFamiliesAds;
			IncludeChartboost = IncludeOptimalAds || IncludeFamiliesAds;
			IncludeUnityAds = IncludeOptimalAds || IncludeFamiliesAds;
			IncludeInMobi = IncludeOptimalAds || IncludeFamiliesAds;
			IncludeIronSource = IncludeOptimalAds || IncludeFamiliesAds;
			IncludeKidoz = IncludeFamiliesAds;
		} else {
			IncludeLiftoffMonetize = IncludeOptimalAds || IncludeFamiliesAds;
			IncludeChartboost = IncludeOptimalAds || IncludeFamiliesAds;
			IncludeUnityAds = IncludeOptimalAds || IncludeFamiliesAds;
			IncludeInMobi = IncludeOptimalAds || IncludeFamiliesAds;
			IncludeIronSource = IncludeOptimalAds || IncludeFamiliesAds;
			IncludeYangoAds = IncludeOptimalAds || IncludeFamiliesAds;
			IncludeMintegral = IncludeOptimalAds || IncludeFamiliesAds;
			IncludeKidoz = IncludeFamiliesAds;
		}
        // End Families Ads Solution

#if UE_5_3_OR_LATER
        TryUpdateDefaultConfigFile(GetDefaultConfigFilename(), false);
#else
        UpdateDefaultConfigFile(GetDefaultConfigFilename());
#endif
    } else if (PropertyName == GET_MEMBER_NAME_CHECKED(UCASDefaultConfig, IncludeOptimalAds)) {
        // Begin Optimal Ads solution  - autogeneration tag
		if (ConfigPlatformId == 1) {
			IncludeGoogleAds = IncludeOptimalAds || IncludeFamiliesAds;
			IncludeLiftoffMonetize = IncludeOptimalAds || IncludeFamiliesAds;
			IncludeChartboost = IncludeOptimalAds || IncludeFamiliesAds;
			IncludeUnityAds = IncludeOptimalAds || IncludeFamiliesAds;
			IncludeInMobi = IncludeOptimalAds || IncludeFamiliesAds;
			IncludeIronSource = IncludeOptimalAds || IncludeFamiliesAds;
			IncludeAppLovin = IncludeOptimalAds;
			IncludeCASExchange = IncludeOptimalAds;
			IncludeAudienceNetwork = IncludeOptimalAds;
			IncludeYangoAds = IncludeOptimalAds;
			IncludeBigo = IncludeOptimalAds;
			IncludeMintegral = IncludeOptimalAds;
			IncludePangle = IncludeOptimalAds;
		} else {
			IncludeLiftoffMonetize = IncludeOptimalAds || IncludeFamiliesAds;
			IncludeChartboost = IncludeOptimalAds || IncludeFamiliesAds;
			IncludeUnityAds = IncludeOptimalAds || IncludeFamiliesAds;
			IncludeInMobi = IncludeOptimalAds || IncludeFamiliesAds;
			IncludeIronSource = IncludeOptimalAds || IncludeFamiliesAds;
			IncludeYangoAds = IncludeOptimalAds || IncludeFamiliesAds;
			IncludeMintegral = IncludeOptimalAds || IncludeFamiliesAds;
			IncludeAppLovin = IncludeOptimalAds;
			IncludeCASExchange = IncludeOptimalAds;
			IncludeAudienceNetwork = IncludeOptimalAds;
			IncludeBigo = IncludeOptimalAds;
			IncludePangle = IncludeOptimalAds;
		}
        // End Optimal Ads solution

#if UE_5_3_OR_LATER
        TryUpdateDefaultConfigFile(GetDefaultConfigFilename(), false);
#else
        UpdateDefaultConfigFile(GetDefaultConfigFilename());
#endif
    }

    Super::PostEditChangeProperty(PropertyChangedEvent);
}
#endif
