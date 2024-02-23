// Copyright CleverAdsSolutions LTD, CAS.AI. All Rights Reserved.

#pragma once

#include "CASDefines.h"
#include "UObject/Object.h"
#include "CASDefaultConfig.generated.h"

UCLASS(config = Engine, defaultconfig, perObjectConfig)
class CLEVERADSSOLUTIONS_API UCASDefaultConfig : public UObject {
   public:
    GENERATED_BODY()

    UCASDefaultConfig()
        : CASAppID(TEXT("demo")),
          TestAdsMode(true),
          Audience(ECASAudience::NotChildren),
          AutoConsentFlow(true),
          UseAdvertisingId(true),
          UserTrackingUsageDescription(TEXT(
              "Your data will remain confidential and will only be used to provide you a better and personalised ad experience")) {
    }
    // MARK: Integration Config
   public:
    UPROPERTY(
        Config,
        EditDefaultsOnly,
        Category = "Integration",
        meta =
            (DisplayName = "CAS App Id (App Bundle/iTunesId)",
             ToolTip =
                 "In most cases, a CAS Id is the same as the Android App's package name, and the iOS App's store Id"))
    FString CASAppID;

    UPROPERTY(Config, EditDefaultsOnly, Category = "Integration")
    bool AutoloadBannerAds;

    UPROPERTY(Config, EditDefaultsOnly, Category = "Integration")
    bool AutoloadMRecAds;

    UPROPERTY(Config, EditDefaultsOnly, Category = "Integration")
    bool AutoloadInterstitialAds;

    UPROPERTY(Config, EditDefaultsOnly, Category = "Integration")
    bool AutoloadRewardedAds;

    UPROPERTY(
        Config,
        EditDefaultsOnly,
        Category = "Integration",
        meta =
            (DisplayName = "TestAdsMode (Not for Shipping build)",
             ToolTip =
                 "We strongly suggest that you enable test ads mode when you validate the integration or develop the app."))
    bool TestAdsMode;

    UPROPERTY(Config,
              EditDefaultsOnly,
              Category = "Integration",
              meta = (NoElementDuplicate, ToolTip = "List of test devices to show tests ads to"))
    TArray<FString> TestDeviceIds;

    // MARK: Privacy Config
   public:
    UPROPERTY(Config,
              EditDefaultsOnly,
              Category = "Privacy",
              meta = (ToolTip = "Select the audience your game is targeting"))
    ECASAudience Audience;

    UPROPERTY(
        Config,
        EditDefaultsOnly,
        Category = "Privacy",
        meta =
            (ToolTip =
                 "The automatic ask for consent from the user on ads initialization. To get consent for collecting personal data of your users, we suggest you use a built-in Consent Flow, comes with a pre-made consent form that you can easily present to your users"))
    bool AutoConsentFlow;

    UPROPERTY(Config,
              EditDefaultsOnly,
              Category = "Privacy",
              meta = (EditCondition = "Audience != ECASAudience::Children || ConfigPlatformId != 1"))
    bool UseAdvertisingId;

    UPROPERTY(
        Config,
        EditAnywhere,
        Category = "Privacy",
        meta =
            (MultiLine = true,
             EditCondition = "UseAdvertisingId && ConfigPlatformId == 2",
             EditConditionHides,
             DisplayName = "User Tracking Usage Description",
             ToolTip =
                 "Required to display the App Tracking Transparency authorization request for accessing the IDFA. Set empty to disable"))
    FString UserTrackingUsageDescription;

    UPROPERTY(Config, EditDefaultsOnly, Category = "Privacy")
    FString PrivacyPolicyURL;

    // MARK: Ads Solutions
   public:
    UPROPERTY(
        Config,
        EditDefaultsOnly,
        Category = "Mediation",
        Meta =
            (DisplayName = "Include Optimal Ads Solution",
             ToolTip =
                 "An Optimal Ads Solutions contains a number of stable partner networks that are recommended for use in most applications."))
    bool IncludeOptimalAds = false;

    UPROPERTY(Config,
              EditDefaultsOnly,
              Category = "Mediation",
              Meta = (DisplayName = "Include Families Ads Solution",
                      ToolTip = "A Families Ads Solutions designed for applications tagged at a children's audience."))
    bool IncludeFamiliesAds = false;

    // Begin Adapters - autogeneration tag
	UPROPERTY(Config, EditDefaultsOnly, AdvancedDisplay, Category = "Mediation")
	bool IncludeCrossPromotion = false;

	UPROPERTY(Config, EditDefaultsOnly, AdvancedDisplay, Category = "Mediation", meta = (EditCondition = "!IncludeOptimalAds && !IncludeFamiliesAds"))
	bool IncludeIronSource = false;

	UPROPERTY(Config, EditDefaultsOnly, AdvancedDisplay, Category = "Mediation", meta = (EditCondition = "!IncludeOptimalAds && !IncludeFamiliesAds"))
	bool IncludeUnityAds = false;

	UPROPERTY(Config, EditDefaultsOnly, AdvancedDisplay, Category = "Mediation", meta = (DisplayName = "Include GoogleAds/AdMob", EditCondition = "!IncludeOptimalAds && !IncludeFamiliesAds"))
	bool IncludeGoogleAds = false;

	UPROPERTY(Config, EditDefaultsOnly, AdvancedDisplay, Category = "Mediation", meta = (EditCondition = "!IncludeOptimalAds && !IncludeFamiliesAds"))
	bool IncludeLiftoffMonetize = false;

	UPROPERTY(Config, EditDefaultsOnly, AdvancedDisplay, Category = "Mediation", meta = (EditCondition = "!IncludeOptimalAds && !IncludeFamiliesAds"))
	bool IncludeInMobi = false;

	UPROPERTY(Config, EditDefaultsOnly, AdvancedDisplay, Category = "Mediation", meta = (EditCondition = "!IncludeOptimalAds && !IncludeFamiliesAds"))
	bool IncludeChartboost = false;

	UPROPERTY(Config, EditDefaultsOnly, AdvancedDisplay, Category = "Mediation", meta = (EditCondition = "!IncludeOptimalAds && !IncludeFamiliesAds"))
	bool IncludeDTExchange = false;

	UPROPERTY(Config, EditDefaultsOnly, AdvancedDisplay, Category = "Mediation", meta = (EditCondition = "!IncludeFamiliesAds"))
	bool IncludeKidoz = false;

	UPROPERTY(Config, EditDefaultsOnly, AdvancedDisplay, Category = "Mediation", meta = (EditCondition = "!IncludeFamiliesAds"))
	bool IncludeSuperAwesome = false;

	UPROPERTY(Config, EditDefaultsOnly, AdvancedDisplay, Category = "Mediation", meta = (EditCondition = "!IncludeOptimalAds && !IncludeFamiliesAds || ConfigPlatformId != 2"))
	bool IncludeMintegral = false;

	UPROPERTY(Config, EditDefaultsOnly, AdvancedDisplay, Category = "Mediation", meta = (EditCondition = "!IncludeOptimalAds"))
	bool IncludeAppLovin = false;

	UPROPERTY(Config, EditDefaultsOnly, AdvancedDisplay, Category = "Mediation", meta = (EditCondition = "!IncludeOptimalAds"))
	bool IncludePangle = false;

	UPROPERTY(Config, EditDefaultsOnly, AdvancedDisplay, Category = "Mediation", meta = (EditCondition = "!IncludeOptimalAds"))
	bool IncludeBigo = false;

	UPROPERTY(Config, EditDefaultsOnly, AdvancedDisplay, Category = "Mediation", meta = (ToolTip = "Yandex Metrica will also be added to the app", EditCondition = "!IncludeOptimalAds && !IncludeFamiliesAds || ConfigPlatformId != 2"))
	bool IncludeYandexAds = false;

	UPROPERTY(Config, EditDefaultsOnly, AdvancedDisplay, Category = "Mediation", meta = (DisplayName = "Include AudienceNetwork/Meta", ToolTip = "Required implementation of `Data processing options for Users in California`", EditCondition = "!IncludeOptimalAds"))
	bool IncludeAudienceNetwork = false;

	UPROPERTY(Config, EditDefaultsOnly, AdvancedDisplay, Category = "Mediation", meta = (ToolTip = "Focused on CIS region only"))
	bool IncludeMyTarget = false;

	UPROPERTY(Config, EditDefaultsOnly, AdvancedDisplay, Category = "Mediation", meta = (DisplayName = "Include Madex (beta)", ToolTip = "Focused on CIS region only"))
	bool IncludeMadex = false;

	UPROPERTY(Config, EditDefaultsOnly, AdvancedDisplay, Category = "Mediation", meta = (DisplayName = "Include HyprMX (beta)", ToolTip = "Focused on USA region only"))
	bool IncludeHyprMX = false;

	UPROPERTY(Config, EditDefaultsOnly, AdvancedDisplay, Category = "Mediation", meta = (DisplayName = "Include Smaato (beta)", ToolTip = "Support Banner only"))
	bool IncludeSmaato = false;

    // End Adapters - autogeneration tag

#if WITH_EDITORONLY_DATA
   public:
    UPROPERTY(Transient)
    int32 ConfigPlatformId = 0;
#endif

#if WITH_EDITOR
   public:
    virtual const TCHAR *GetConfigOverridePlatform() const override {
        if (ConfigPlatformId == 0) {
            return nullptr;
        }
        return ConfigPlatformId == 1 ? TEXT("Android") : TEXT("IOS");
    }
    virtual void OverridePerObjectConfigSection(FString &SectionName) override;
    virtual void PostEditChangeProperty(FPropertyChangedEvent &PropertyChangedEvent) override;
#endif
};