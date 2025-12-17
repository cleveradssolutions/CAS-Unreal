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
		if (ConfigPlatformId == 1) {
			IncludeGoogleAds = IncludeOptimalAds || IncludeFamiliesAds;
			IncludeLiftoffMonetize = IncludeOptimalAds || IncludeFamiliesAds;
			IncludeChartboost = IncludeOptimalAds || IncludeFamiliesAds;
			IncludeUnityAds = IncludeOptimalAds || IncludeFamiliesAds;
			IncludeInMobi = IncludeOptimalAds || IncludeFamiliesAds;
			IncludeDTExchange = IncludeOptimalAds || IncludeFamiliesAds;
			IncludeIronSource = IncludeOptimalAds || IncludeFamiliesAds;
			IncludeKidoz = IncludeFamiliesAds;
		} else {
			IncludeGoogleAds = IncludeOptimalAds || IncludeFamiliesAds;
			IncludeLiftoffMonetize = IncludeOptimalAds || IncludeFamiliesAds;
			IncludeChartboost = IncludeOptimalAds || IncludeFamiliesAds;
			IncludeUnityAds = IncludeOptimalAds || IncludeFamiliesAds;
			IncludeAppLovin = IncludeOptimalAds || IncludeFamiliesAds;
			IncludeCASExchange = IncludeOptimalAds || IncludeFamiliesAds;
			IncludeAudienceNetwork = IncludeOptimalAds || IncludeFamiliesAds;
			IncludeInMobi = IncludeOptimalAds || IncludeFamiliesAds;
			IncludeDTExchange = IncludeOptimalAds || IncludeFamiliesAds;
			IncludeIronSource = IncludeOptimalAds || IncludeFamiliesAds;
			IncludeYangoAds = IncludeOptimalAds || IncludeFamiliesAds;
			IncludeBigo = IncludeOptimalAds || IncludeFamiliesAds;
			IncludeMintegral = IncludeOptimalAds || IncludeFamiliesAds;
			IncludePangle = IncludeOptimalAds || IncludeFamiliesAds;
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
			IncludeDTExchange = IncludeOptimalAds || IncludeFamiliesAds;
			IncludeIronSource = IncludeOptimalAds || IncludeFamiliesAds;
			IncludeAppLovin = IncludeOptimalAds;
			IncludeCASExchange = IncludeOptimalAds;
			IncludeAudienceNetwork = IncludeOptimalAds;
			IncludeYangoAds = IncludeOptimalAds;
			IncludeBigo = IncludeOptimalAds;
			IncludeMintegral = IncludeOptimalAds;
			IncludePangle = IncludeOptimalAds;
		} else {
			IncludeGoogleAds = IncludeOptimalAds || IncludeFamiliesAds;
			IncludeLiftoffMonetize = IncludeOptimalAds || IncludeFamiliesAds;
			IncludeChartboost = IncludeOptimalAds || IncludeFamiliesAds;
			IncludeUnityAds = IncludeOptimalAds || IncludeFamiliesAds;
			IncludeAppLovin = IncludeOptimalAds || IncludeFamiliesAds;
			IncludeCASExchange = IncludeOptimalAds || IncludeFamiliesAds;
			IncludeAudienceNetwork = IncludeOptimalAds || IncludeFamiliesAds;
			IncludeInMobi = IncludeOptimalAds || IncludeFamiliesAds;
			IncludeDTExchange = IncludeOptimalAds || IncludeFamiliesAds;
			IncludeIronSource = IncludeOptimalAds || IncludeFamiliesAds;
			IncludeYangoAds = IncludeOptimalAds || IncludeFamiliesAds;
			IncludeBigo = IncludeOptimalAds || IncludeFamiliesAds;
			IncludeMintegral = IncludeOptimalAds || IncludeFamiliesAds;
			IncludePangle = IncludeOptimalAds || IncludeFamiliesAds;
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