// Copyright CleverAdsSolutions LTD, CAS.AI. All Rights Reserved.

#pragma once

#include "CASDefines.h"
#include "Kismet/BlueprintFunctionLibrary.h"

#include "CASMobileAds.generated.h"

UCLASS()
class CLEVERADSSOLUTIONS_API UCASMobileAds: public UBlueprintFunctionLibrary {
    GENERATED_BODY()

    // MARK: General
public:
    static FOnCASInitializedDelegate OnAdsInitialized;

    /**
     * The event is fired after the consent form is dismissed.
     * If consent is not required, the event is fired immediately.
     * Same event as the `OnConsentFlowResult` one.
     */
    static FOnCASCallbackDelegate OnConsentFlowDismissed;

    /**
     * The event is fired after the consent form is dismissed.
     * If consent is not required, the event is fired immediately.
     * Same event as the `OnConsentFlowDismissed` one.
     * The `FCASConsentFlowStatus` with which the form is dismissed will be passed
     * to the event argument.
     */
    static FOnCASConsentFlowCallbackDelegate OnConsentFlowResult;

    /**
     * Sets additional mediation settings before `InitializeMobileAds`
     */
    UFUNCTION(BlueprintCallable, Category = "CAS Mobile Ads")
    static void SetInitializationAdsExtras(FString key, FString value);

    UFUNCTION(BlueprintCallable, Category = "CAS Mobile Ads",
              meta = (ExpandBoolAsExecs = "ReturnValue"))
    static bool IsInitializedAds();

    /**
     * Initialize mobile Ads once per session.
     * Subscribe to `OnAdsInitialized` event
     */
    static void InitializeMobileAds();

    /**
     * Returns CAS native SDK version.
     * @return String with version number such as '1.2.3'
     */
    UFUNCTION(BlueprintPure, Category = "CAS Mobile Ads",
              meta = (ReturnDisplayName = "Version"))
    static FString GetMobileAdsVersion();

    UFUNCTION(BlueprintPure, Category = "CAS Mobile Ads",
              meta = (ReturnDisplayName = "Message"))
    static FString GetAdsErrorMessage(ECASError Error);

    /**
     * Validates integration and outputs validating info in log files.
     */
    UFUNCTION(BlueprintCallable, Category = "CAS Mobile Ads")
    static void ValidateAdsIntegration();

    /**
     * The enabled Debug Mode will display a lot of useful information for
     * debugging about the states of the sdk with tag CAS. Disabling Debug Mode
     * may improve application performance.
     *
     * Disabled by default.
     */
    UFUNCTION(BlueprintCallable, Category = "CAS Mobile Ads")
    static void SetVerboseAdsLogs(bool Enabled);

    /**
     * Indicates if the application’s audio is muted. Affects initial mute state
     * for all ads. Use this method only if your application has its own volume
     * controls (e.g., custom music or sound effect muting).
     *
     * Disabled by default.
     */
    UFUNCTION(BlueprintCallable, Category = "CAS Mobile Ads")
    static void SetAdsMuted(bool Mute);

    /**
     * Defines the time interval, in seconds, starting from the moment of the
     * initial app installation, during which users can use the application
     * without ads being displayed while still retaining access to the Rewarded
     * Ads format. Within this interval, users enjoy privileged access to the
     * application's features without intrusive advertisements.
     * - Default: 0 seconds
     * - Units: Seconds
     */
    UFUNCTION(BlueprintCallable, Category = "CAS Mobile Ads")
    static void SetTrialAdFreeInterval(int interval);

    /**
     * Shows the consent form only if it is required and the user has not
     * responded previously. If the consent status is required, the SDK loads a
     * form and immediately presents it. Subscribe to `OnConsentFlowDismissed` or
     * `OnConsentFlowResult` event.
     */
    static void ShowAdConsentFlowIfRequired();

    /**
     * Force shows the form to modify user  consent at any time.
     *
     * When a user interacts with your UI element, call function to show the form
     * so the user can update their privacy options at any time.
     * Subscribe to `OnConsentFlowDismissed` or `OnConsentFlowResult` event.
     */
    static void ShowAdConsentFlow();

    /**
     * Ad filters by Audience.
     * If you know that the user falls within an age-restricted category (i.e.,
     * under the age of 13), you must set the "Children" audience flag.
     */
    UFUNCTION(BlueprintCallable, Category = "CAS Mobile Ads")
    static void SetUserAudienceForAds(ECASAudience Audience);

    /**
     * Check if a Consent Flow is required for the user.
     * Use this method before include Privacy Options button in application
     * settings menu.
     *
     * Note that the real status will only be determined
     * after calling the `ShowAdConsentFlowIfRequired()` or
     * `InitializeMobileAds()`.
     */
    UFUNCTION(BlueprintCallable, Category = "CAS Mobile Ads",
              meta = (ExpandBoolAsExecs = "ReturnValue"))
    static bool IsUserAdConsentRequired();

    /**
     * Sets Consent Flag in GDPR and Other Regions
     * Use it if you are using your own or a third-party party consent mechanism.
     */
    UFUNCTION(BlueprintCallable, Category = "CAS Mobile Ads")
    static void SetUserConsentForAds(ECASUserConsentStatus ConsentStatus);

    /**
     * Sets California Consumer Privacy Act status for a user
     * If a user does NOT opt out of interest-based advertising, set the OptInSale
     * flag If a user does opt out of interest-based advertising, set the
     * OptOutSale flag
     */
    UFUNCTION(BlueprintCallable, Category = "CAS Mobile Ads")
    static void SetUserOptOutSaleForAds(ECASUserCCPAStatus CCPAStatus);

    /**
     * The userID is a unique identifier supplied by your application and must be
     * static for each user across sessions. Your userID should not contain any
     * personally identifiable information such as an email address, screen name,
     * IDFV, AdvertisingID or IDFA.
     */
    UFUNCTION(BlueprintCallable, Category = "CAS Mobile Ads")
    static void SetUserIDForAds(FString UserID);

    /**
     * Set targeting to user’s age
     * Limitation: 1-99 and 0 is 'unknown'
     */
    UFUNCTION(BlueprintCallable, Category = "CAS Mobile Ads")
    static void SetUserAgeForAds(int UserAge);

    /**
     * Set targeting to user’s gender
     */
    UFUNCTION(BlueprintCallable, Category = "CAS Mobile Ads")
    static void SetUserGenderForAds(ECASGender UserGender);

    /**
     * Collect from the device the latitude and longitude coordinated truncated to
     * the hundredths decimal place. Collect only if your application already has
     * the relevant end-user permissions. Does not collect if the target audience
     * is children.
     *
     * Disabled by default.
     */
    UFUNCTION(BlueprintCallable, Category = "CAS Mobile Ads")
    static void SetUserLocationCollectionForAds(bool Enabled);

    /**
     * Meta Audience Network Data Processing Options for US Users
     * Limited Data Use is a data processing option that gives you more control
     * over how your data is used in Meta’s systems and better supports your
     * compliance efforts with various US state privacy regulations. To utilize
     * this feature, you must proactively enable Limited Data Use.
     *
     * Set native the `FBAdSettings.setDataProcessingOptions` flag values:
     * - `ECASUserCCPAStatus::OptInSale` ([] empty) - To explicitly not enable
     * Limited Data Use (LDU) mode
     * - `ECASUserCCPAStatus::OptOutSale` (['LDU'], 0, 0) - To enable LDU mode
     * using geolocation
     * - `ECASUserCCPAStatus::Undefined` - Do nothing
     *
     * Attention: Flag will be applied before CAS Initialization only.
     *
     * Visit Meta’s developer documentation for details:
     * https://developers.facebook.com/docs/marketing-apis/data-processing-options
     */
    UFUNCTION(BlueprintCallable, Category = "CAS Mobile Ads")
    static void SetMetaDataProcessingOptions(ECASUserCCPAStatus MetaOptions);

    // MARK: Ads Impression
    static FOnCASImpressionDelegate OnAdsImpression;

    UFUNCTION(BlueprintCallable, Category = "CAS Mobile Ads")
    static void
    BindEventToOnAdsImpression(const FCASImpressionDelegate &OnImpression);

    UFUNCTION(BlueprintCallable, Category = "CAS Mobile Ads")
    static void
    UnbindEventFromOnAdsImpression(const FCASImpressionDelegate &OnImpression);

    UFUNCTION(BlueprintCallable, Category = "CAS Mobile Ads")
    static void UnbindAllEventsFromOnAdsImpression();

    // MARK: Banner Ads
public:
    static FOnCASCallbackDelegate OnBannerAdLoaded;
    static FOnCASFailedDelegate OnBannerAdFailed;
    static FOnCASCallbackDelegate OnBannerAdClicked;

    /**
     * Checks if Banner ad is ready.
     */
    UFUNCTION(BlueprintCallable, Category = "CAS Mobile Ads",
              meta = (ExpandBoolAsExecs = "ReturnValue"))
    static bool IsBannerAdReady();

    /**
     * Load new Ad for banner view with the Ad Size.
     * Subscribe to OnBannerAdLoaded and OnBannerAdFailed events.
     * Blueprints allowed with same function name.
     */
    static void LoadBannerAd(ECASBannerSize AdSize = ECASBannerSize::Banner);

    /**
     * Gets or sets whether autoloading of ads is enabled.
     * You should call `LoadBannerAd()` once to create Ad Instance.
     *
     * If enabled, the ad will automatically load new content when the current ad
     * is dismissed or completed. Additionally, it will automatically retry
     * loading the ad if an error occurs during the loading process.
     */
    UFUNCTION(BlueprintCallable, Category = "CAS Mobile Ads")
    static void SetAutoloadBannerAd(bool Enabled);

    UFUNCTION(BlueprintCallable, Category = "CAS Mobile Ads")
    static void ShowBannerAd();

    UFUNCTION(BlueprintCallable, Category = "CAS Mobile Ads")
    static void HideBannerAd();

    /**
     * Set position on the screen for Banner Ad.
     * @param AdPosition - New Banner Ad position on screen.
     */
    UFUNCTION(BlueprintCallable, Category = "CAS Mobile Ads",
              meta = (AdPosition = "ECASPosition::BottomCenter"))
    static void SetBannerAdPosition(ECASPosition AdPosition);

    /**
     * Disable and Destroys loaded Banner view from memory.
     * If you want simple hide banner from screen then use HideBannerAd()
     * instead.
     */
    UFUNCTION(BlueprintCallable, Category = "CAS Mobile Ads")
    static void DestroyBannerAd();

    /**
     * An ad unit automatic refresh rate (in seconds) determines
     * how often a new ad request is generated for that ad unit.
     * Set 0 to disable automatic ad refresh.
     * Ad refresh each 30 seconds by default.
     */
    UFUNCTION(BlueprintCallable, Category = "CAS Mobile Ads",
              meta = (Interval = "30"))
    static void SetBannerAdRefreshInterval(int Interval);

    // MARK: Medium Rectangle Ads
public:
    static FOnCASCallbackDelegate OnMRecAdLoaded;
    static FOnCASFailedDelegate OnMRecAdFailed;
    static FOnCASCallbackDelegate OnMRecAdClicked;

    /**
     * Checks if MediumRectangle ad is ready.
     */
    UFUNCTION(BlueprintCallable, Category = "CAS Mobile Ads",
              meta = (ExpandBoolAsExecs = "ReturnValue"))
    static bool IsMRecAdReady();

    /**
     * Load new Ad for MediumRectangle view.
     * Subscribe to OnMRecAdLoaded and OnMRecAdFailed events
     * Blueprints allowed with same function name.
     */
    static void LoadMRecAd();

    /**
     * Gets or sets whether autoloading of ads is enabled.
     * You should call `LoadMRecAd()` once to create Ad Instance.
     *
     * If enabled, the ad will automatically load new content when the current ad
     * is dismissed or completed. Additionally, it will automatically retry
     * loading the ad if an error occurs during the loading process.
     */
    UFUNCTION(BlueprintCallable, Category = "CAS Mobile Ads")
    static void SetAutoloadMRecAd(bool Enabled);

    UFUNCTION(BlueprintCallable, Category = "CAS Mobile Ads")
    static void ShowMRecAd();

    UFUNCTION(BlueprintCallable, Category = "CAS Mobile Ads")
    static void HideMRecAd();

    /**
     * Set position on screen for MediumRectangle Ad.
     * @param AdPosition - New MediumRectangle Ad position on screen.
     */
    UFUNCTION(BlueprintCallable, Category = "CAS Mobile Ads",
              meta = (AdPosition = "ECASPosition::BottomCenter"))
    static void SetMRecAdPosition(ECASPosition AdPosition);

    /**
     * An ad unit automatic refresh rate (in seconds) determines
     * how often a new ad request is generated for that ad unit.
     * Set 0 to disable automatic ad refresh.
     * Ad refresh each 30 seconds by default.
     */
    UFUNCTION(BlueprintCallable, Category = "CAS Mobile Ads",
              meta = (Interval = "30"))
    static void SetMRecAdRefreshInterval(int Interval);

    /**
     * Disable and Destroys loaded MediumRectangle view from memory.
     * If you want simple hide MediumRectangle from screen then use HideMRecAd()
     * instead.
     */
    UFUNCTION(BlueprintCallable, Category = "CAS Mobile Ads")
    static void DestroyMRecAd();

    // MARK: Interstitial Ads
public:
    static FOnCASCallbackDelegate OnInterstitialAdLoaded;
    static FOnCASFailedDelegate OnInterstitialAdLoadFailed;
    static FOnCASFailedDelegate OnInterstitialAdShowFailed;
    static FOnCASCallbackDelegate OnInterstitialAdDisplayed;
    static FOnCASCallbackDelegate OnInterstitialAdClicked;
    static FOnCASCallbackDelegate OnInterstitialAdDismissed;

    /**
     * Start loading an interstitial.
     * Subscribe to `OnInterstitialAdLoaded` and `OnInterstitialAdLoadFailed`
     * events. Blueprints allowed with same function name.
     */
    static void LoadInterstitialAd();

    /**
     * Gets or sets whether autoloading of ads is enabled.
     * You should call `LoadInterstitialAd()` once to create Ad Instance.
     *
     * If enabled, the ad will automatically load new content when the current ad
     * is dismissed or completed. Additionally, it will automatically retry
     * loading the ad if an error occurs during the loading process.
     */
    UFUNCTION(BlueprintCallable, Category = "CAS Mobile Ads")
    static void SetAutoloadInterstitialAd(bool Enabled);

    /**
     * Check if the interstitial is loaded and ready to be displayed.
     */
    UFUNCTION(BlueprintCallable, Category = "CAS Mobile Ads",
              meta = (ExpandBoolAsExecs = "ReturnValue"))
    static bool IsInterstitialAdReady();

    /**
     * Present loaded interstitial.
     * Subscribe to events:
     * `OnInterstitialAdDisplayed`, `OnInterstitialAdDismissed`,
     * `OnInterstitialAdShowFailed`, OnInterstitialAdClicked` Blueprints allowed
     * with same function name.
     */
    static void ShowInterstitialAd();

    /**
     * Loaded Interstitial ad should be automatically displayed when the user
     * returns to the app.
     *
     * Note that the ad must be ready for display at the time of returning to the
     * app.
     *
     * Subscribe to events:
     * `OnInterstitialAdDisplayed`, `OnInterstitialAdDismissed`,
     * `OnInterstitialAdShowFailed`, OnInterstitialAdClicked` Blueprints allowed
     * with same function name.
     */
    static void AutoshowInterstitialAd();

    /**
     * Disable automatically displayed Interstitial ad.
     */
    UFUNCTION(BlueprintCallable, Category = "CAS Mobile Ads")
    static void DisableAutoshowInterstitialAd();

    /**
     * You can limit the posting of an interstitial ad to a period of time in
     * seconds after the ad is closed, during which display attempts will fail.
     * Disabled by default.
     */
    UFUNCTION(BlueprintCallable, Category = "CAS Mobile Ads")
    static void SetInterstitialAdMinimumInterval(int Interval);

    /**
     * Restarts the interval to allow Interstitial ads impression after new
     * interval.
     */
    UFUNCTION(BlueprintCallable, Category = "CAS Mobile Ads")
    static void RestartInterstitialAdInterval();

    /**
     * Destroys the ad content and releases any associated resources.
     *
     * Call this method when the ad is no longer needed to clean up resources and
     * prevent memory leaks.
     *
     * Call `LoadInterstitialAd` to load new ad and restore autoload mode if used.
     */
    UFUNCTION(BlueprintCallable, Category = "CAS Mobile Ads")
    static void DestroyInterstitialAd();

    // MARK: Rewarded Ads
public:
    static FOnCASCallbackDelegate OnRewardedAdLoaded;
    static FOnCASFailedDelegate OnRewardedAdLoadFailed;
    static FOnCASCallbackDelegate OnRewardedAdEarnedReward;
    static FOnCASFailedDelegate OnRewardedAdShowFailed;
    static FOnCASCallbackDelegate OnRewardedAdDisplayed;
    static FOnCASCallbackDelegate OnRewardedAdClicked;
    static FOnCASCallbackDelegate OnRewardedAdDismissed;

    /**
     * Start loading a rewarded ad.
     * Subscribe to events: `OnRewardedAdLoaded`, `OnRewardedAdLoadFailed`.
     * Blueprints allowed with same function name.
     */
    static void LoadRewardedAd();

    /**
     * Gets or sets whether autoloading of ads is enabled.
     * You should call `LoadRewardedAd()` once to create Ad Instance.
     *
     * If enabled, the ad will automatically load new content when the current ad
     * is dismissed or completed. Additionally, it will automatically retry
     * loading the ad if an error occurs during the loading process.
     */
    UFUNCTION(BlueprintCallable, Category = "CAS Mobile Ads")
    static void SetAutoloadRewardedAd(bool Enabled);

    /**
     * Check if the rewarded ad is loaded and ready to be displayed.
     */
    UFUNCTION(BlueprintCallable, Category = "CAS Mobile Ads",
              meta = (ExpandBoolAsExecs = "ReturnValue"))
    static bool IsRewardedAdReady();

    /**
     * Present loaded rewarded ad.
     * Subscribe to events: `OnRewardedAdEarnedReward`,
     * `OnRewardedAdDisplayed`, `OnRewardedAdDismissed`, `OnRewardedAdShowFailed`,
     * `OnRewardedAdClicked` Blueprints allowed with same function name.
     */
    static void ShowRewardedAd();

    /**
     * Destroys the ad content and releases any associated resources.
     *
     * Call this method when the ad is no longer needed to clean up resources and
     * prevent memory leaks.
     *
     * Call `LoadRewardedAd` to load new ad and restore autoload mode if used.
     */
    UFUNCTION(BlueprintCallable, Category = "CAS Mobile Ads")
    static void DestroyRewardedAd();

    // MARK: AppOpen Ads
public:
    static FOnCASCallbackDelegate OnAppOpenAdLoaded;
    static FOnCASFailedDelegate OnAppOpenAdLoadFailed;
    static FOnCASFailedDelegate OnAppOpenAdShowFailed;
    static FOnCASCallbackDelegate OnAppOpenAdDisplayed;
    static FOnCASCallbackDelegate OnAppOpenAdClicked;
    static FOnCASCallbackDelegate OnAppOpenAdDismissed;

    /**
     * Start loading a AppOpen ad.
     * Subscribe to events: `OnAppOpenAdLoaded`, `OnAppOpenAdLoadFailed`.
     * Blueprints allowed with same function name.
     */
    static void LoadAppOpenAd();

    /**
     * Gets or sets whether autoloading of ads is enabled.
     * You should call `LoadAppOpenAd()` once to create Ad Instance.
     *
     * If enabled, the ad will automatically load new content when the current ad
     * is dismissed or completed. Additionally, it will automatically retry
     * loading the ad if an error occurs during the loading process.
     */
    UFUNCTION(BlueprintCallable, Category = "CAS Mobile Ads")
    static void SetAutoloadAppOpenAd(bool Enabled);

    /**
     * Check if the AppOpen ad is loaded and ready to be displayed.
     */
    UFUNCTION(BlueprintCallable, Category = "CAS Mobile Ads",
              meta = (ExpandBoolAsExecs = "ReturnValue"))
    static bool IsAppOpenAdReady();

    /**
     * Present loaded AppOpen ad.
     * Subscribe to events:
     * `OnAppOpenAdDisplayed`, `OnAppOpenAdDismissed`, `OnAppOpenAdShowFailed`,
     * `OnAppOpenAdClicked` Blueprints allowed with same function name.
     */
    static void ShowAppOpenAd();

    /**
     * Loaded AppOpen ad should be automatically displayed when the user returns
     * to the app.
     *
     * Note that the ad must be ready for display at the time of returning to the
     * app.
     *
     * Subscribe to events:
     * `OnAppOpenAdDisplayed`, `OnAppOpenAdDismissed`, `OnAppOpenAdShowFailed`,
     * `OnAppOpenAdClicked` Blueprints allowed with same function name.
     */
    static void AutoshowAppOpenAd();

    /**
     * Disable automatically displayed AppOpen ad.
     */
    UFUNCTION(BlueprintCallable, Category = "CAS Mobile Ads")
    static void DisableAutoshowAppOpenAd();

    /**
     * Destroys the ad content and releases any associated resources.
     *
     * Call this method when the ad is no longer needed to clean up resources and
     * prevent memory leaks.
     *
     * Call `LoadAppOpenAd` to load new ad and restore autoload mode if used.
     */
    UFUNCTION(BlueprintCallable, Category = "CAS Mobile Ads")
    static void DestroyAppOpenAd();

    // MARK: Return to App Ads
public:
    /**
     * deprecated: Use alternative for Interstitial or AppOpen ads
     */
    static FOnCASFailedDelegate OnReturnToAppAdShowFailed;
    /**
     * deprecated: Use alternative for Interstitial or AppOpen ads
     */
    static FOnCASCallbackDelegate OnReturnToAppAdDisplayed;
    /**
     * deprecated: Use alternative for Interstitial or AppOpen ads
     */
    static FOnCASCallbackDelegate OnReturnToAppAdClicked;
    /**
     * deprecated: Use alternative for Interstitial or AppOpen ads
     */
    static FOnCASCallbackDelegate OnReturnToAppAdDismissed;

    /**
     * deprecated: Use new AutoshowInterstitialAd() or AutoshowAppOpenAd()
     */
    static void ShowAdOnReturnToApp();

    /**
     * deprecated: No longer support
     */
    UFUNCTION(BlueprintCallable, Category = "CAS Mobile Ads",
              meta = (DeprecatedFunction, DeprecationMessage = "No longer support"))
    static void SkipAdOnNextReturnToApp();

    /**
     * deprecated: Use new DisableAutoshowInterstitialAd() and
     * DisableAutoshowAppOpenAd()
     */
    UFUNCTION(BlueprintCallable, Category = "CAS Mobile Ads",
              meta = (DeprecatedFunction, DeprecationMessage = "Replaced with new DisableAutoshowInterstitialAd() and DisableAutoshowAppOpenAd()."))
    static void DestroyReturnToAppAd();
};
