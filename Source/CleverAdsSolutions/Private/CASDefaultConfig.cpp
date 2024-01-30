// Copyright CleverAdsSolutions LTD, CAS.AI. All Rights Reserved.

#include "CASDefaultConfig.h"

#if WITH_EDITOR
#include "CASDefines.h"
#include "CoreGlobals.h"

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
		if(ConfigPlatformId == 1) {
			IncludeUnityAds = IncludeOptimalAds || IncludeFamiliesAds;
			IncludeGoogleAds = IncludeOptimalAds || IncludeFamiliesAds;
			IncludeIronSource = IncludeOptimalAds || IncludeFamiliesAds;
			IncludeInMobi = IncludeOptimalAds || IncludeFamiliesAds;
			IncludeChartboost = IncludeOptimalAds || IncludeFamiliesAds;
			IncludeLiftoffMonetize = IncludeOptimalAds || IncludeFamiliesAds;
			IncludeDTExchange = IncludeOptimalAds || IncludeFamiliesAds;
			IncludeKidoz = IncludeFamiliesAds;
			IncludeSuperAwesome = IncludeFamiliesAds;
		} else {
			IncludeMintegral = IncludeOptimalAds || IncludeFamiliesAds;
			IncludeUnityAds = IncludeOptimalAds || IncludeFamiliesAds;
			IncludeGoogleAds = IncludeOptimalAds || IncludeFamiliesAds;
			IncludeIronSource = IncludeOptimalAds || IncludeFamiliesAds;
			IncludeInMobi = IncludeOptimalAds || IncludeFamiliesAds;
			IncludeChartboost = IncludeOptimalAds || IncludeFamiliesAds;
			IncludeLiftoffMonetize = IncludeOptimalAds || IncludeFamiliesAds;
			IncludeDTExchange = IncludeOptimalAds || IncludeFamiliesAds;
			IncludeYandexAds = IncludeOptimalAds || IncludeFamiliesAds;
			IncludeKidoz = IncludeFamiliesAds;
			IncludeSuperAwesome = IncludeFamiliesAds;
		}
        // End Families Ads Solution

#if UE_5_3_OR_LATER
        TryUpdateDefaultConfigFile(GetDefaultConfigFilename(), false);
#else
        UpdateDefaultConfigFile(GetDefaultConfigFilename());
#endif
    } else if (PropertyName == GET_MEMBER_NAME_CHECKED(UCASDefaultConfig, IncludeOptimalAds)) {
        // Begin Optimal Ads solution  - autogeneration tag
		if(ConfigPlatformId == 1) {
			IncludeUnityAds = IncludeOptimalAds || IncludeFamiliesAds;
			IncludeGoogleAds = IncludeOptimalAds || IncludeFamiliesAds;
			IncludeIronSource = IncludeOptimalAds || IncludeFamiliesAds;
			IncludeInMobi = IncludeOptimalAds || IncludeFamiliesAds;
			IncludeChartboost = IncludeOptimalAds || IncludeFamiliesAds;
			IncludeLiftoffMonetize = IncludeOptimalAds || IncludeFamiliesAds;
			IncludeDTExchange = IncludeOptimalAds || IncludeFamiliesAds;
			IncludeMintegral = IncludeOptimalAds;
			IncludeAudienceNetwork = IncludeOptimalAds;
			IncludeAppLovin = IncludeOptimalAds;
			IncludePangle = IncludeOptimalAds;
			IncludeYandexAds = IncludeOptimalAds;
			IncludeBigo = IncludeOptimalAds;
		} else {
			IncludeMintegral = IncludeOptimalAds || IncludeFamiliesAds;
			IncludeUnityAds = IncludeOptimalAds || IncludeFamiliesAds;
			IncludeGoogleAds = IncludeOptimalAds || IncludeFamiliesAds;
			IncludeIronSource = IncludeOptimalAds || IncludeFamiliesAds;
			IncludeInMobi = IncludeOptimalAds || IncludeFamiliesAds;
			IncludeChartboost = IncludeOptimalAds || IncludeFamiliesAds;
			IncludeLiftoffMonetize = IncludeOptimalAds || IncludeFamiliesAds;
			IncludeDTExchange = IncludeOptimalAds || IncludeFamiliesAds;
			IncludeYandexAds = IncludeOptimalAds || IncludeFamiliesAds;
			IncludeAudienceNetwork = IncludeOptimalAds;
			IncludeAppLovin = IncludeOptimalAds;
			IncludePangle = IncludeOptimalAds;
			IncludeBigo = IncludeOptimalAds;
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

void UCASDefaultConfig::OverridePerObjectConfigSection(FString &SectionName) {
    SectionName = TEXT("/Script/CleverAdsSolutions.CASDefaultConfig");
}
#endif