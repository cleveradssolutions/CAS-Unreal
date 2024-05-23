// Copyright CleverAdsSolutions LTD, CAS.AI. All Rights Reserved.

#pragma once

#include "CASDefines.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "CASMobileAds.generated.h"

UCLASS()
class CLEVERADSSOLUTIONS_API UCASMobileAds : public UBlueprintFunctionLibrary {
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
     * The `FCASConsentFlowStatus` with which the form is dismissed will be passed to the event argument.
     */
    static FOnCASConsentFlowCallbackDelegate OnConsentFlowResult;

    UFUNCTION(BlueprintCallable, Category = "CAS Mobile Ads", meta = (ExpandBoolAsExecs = "ReturnValue"))
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
    UFUNCTION(BlueprintPure, Category = "CAS Mobile Ads", meta = (ReturnDisplayName = "Version"))
    static FString GetMobileAdsVersion();

    UFUNCTION(BlueprintPure, Category = "CAS Mobile Ads", meta = (ReturnDisplayName = "Message"))
    static FString GetAdsErrorMessage(ECASError Error);

    /**
     * Validates integration and outputs validating info in log files.
     */
    UFUNCTION(BlueprintCallable, Category = "CAS Mobile Ads")
    static void ValidateAdsIntegration();

    /**
     * The enabled Debug Mode will display a lot of useful information for debugging about the states of the sdk with
     * tag CAS. Disabling Debug Mode may improve application performance.
     *
     * Disabled by default.
     */
    UFUNCTION(BlueprintCallable, Category = "CAS Mobile Ads")
    static void SetVerboseAdsLogs(bool Enabled);

    /**
     * Indicates if the application’s audio is muted. Affects initial mute state for
     * all ads. Use this method only if your application has its own volume controls
     * (e.g., custom music or sound effect muting).
     *
     * Disabled by default.
     */
    UFUNCTION(BlueprintCallable, Category = "CAS Mobile Ads")
    static void SetAdsMuted(bool Mute);

    /**
     * Defines the time interval, in seconds, starting from the moment of the initial app installation,
     * during which users can use the application without ads being displayed while still retaining
     * access to the Rewarded Ads format.
     * Within this interval, users enjoy privileged access to the application's features without intrusive
     * advertisements.
     * - Default: 0 seconds
     * - Units: Seconds
     */
    UFUNCTION(BlueprintCallable, Category = "CAS Mobile Ads")
    static void SetTrialAdFreeInterval(int interval);

    /**
     * Shows the consent form only if it is required and the user has not responded previously.
     * If the consent status is required, the SDK loads a form and immediately presents it.
     * Subscribe to `OnConsentFlowDismissed` or `OnConsentFlowResult` event.
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
     * Use this method before include Privacy Options button in application settings menu.
     * 
     * Note that the real status will only be determined 
     * after calling the `ShowAdConsentFlowIfRequired()` or `InitializeMobileAds()`.
     */
    UFUNCTION(BlueprintCallable, Category = "CAS Mobile Ads", meta = (ExpandBoolAsExecs = "ReturnValue"))
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
     * Collect from the device the latitude and longitude coordinated truncated to the
     * hundredths decimal place.
     * Collect only if your application already has the relevant end-user permissions.
     * Does not collect if the target audience is children.
     *
     * Disabled by default.
     */
    UFUNCTION(BlueprintCallable, Category = "CAS Mobile Ads")
    static void SetUserLocationCollectionForAds(bool Enabled);

    /**
     * Meta Audience Network Data Processing Options for US Users
     * Limited Data Use is a data processing option that gives you more control over how your data
     * is used in Meta’s systems and better supports your compliance efforts with various US state
     * privacy regulations. To utilize this feature, you must proactively enable Limited Data Use.
     *
     * Set native the `FBAdSettings.setDataProcessingOptions` flag values:
     * - `ECASUserCCPAStatus::OptInSale` ([] empty) - To explicitly not enable Limited Data Use (LDU) mode
     * - `ECASUserCCPAStatus::OptOutSale` (['LDU'], 0, 0) - To enable LDU mode using geolocation
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
    static void BindEventToOnAdsImpression(const FCASImpressionDelegate &OnImpression);

    UFUNCTION(BlueprintCallable, Category = "CAS Mobile Ads")
    static void UnbindEventFromOnAdsImpression(const FCASImpressionDelegate &OnImpression);

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
    UFUNCTION(BlueprintCallable, Category = "CAS Mobile Ads", meta = (ExpandBoolAsExecs = "ReturnValue"))
    static bool IsBannerAdReady();

    /**
     * Load new Ad for banner view with the Ad Size.
     * Subscribe to OnBannerAdLoaded and OnBannerAdFailed events.
     * Blueprints allowed with same function name.
     */
    static void LoadBannerAd(ECASBannerSize AdSize = ECASBannerSize::Banner);

    UFUNCTION(BlueprintCallable, Category = "CAS Mobile Ads")
    static void ShowBannerAd();

    UFUNCTION(BlueprintCallable, Category = "CAS Mobile Ads")
    static void HideBannerAd();

    /**
     * Set position on the screen for Banner Ad.
     * @param AdPosition - New Banner Ad position on screen.
     */
    UFUNCTION(BlueprintCallable, Category = "CAS Mobile Ads", meta = (AdPosition = "ECASPosition::BottomCenter"))
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
    UFUNCTION(BlueprintCallable, Category = "CAS Mobile Ads", meta = (Interval = "30"))
    static void SetBannerAdRefreshInterval(int Interval);

    // MARK: Medium Rectangle Ads
   public:
    static FOnCASCallbackDelegate OnMRecAdLoaded;
    static FOnCASFailedDelegate OnMRecAdFailed;
    static FOnCASCallbackDelegate OnMRecAdClicked;

    /**
     * Checks if MediumRectangle ad is ready.
     */
    UFUNCTION(BlueprintCallable, Category = "CAS Mobile Ads", meta = (ExpandBoolAsExecs = "ReturnValue"))
    static bool IsMRecAdReady();

    /**
     * Load new Ad for MediumRectangle view.
     * Subscribe to OnMRecAdLoaded and OnMRecAdFailed events
     * Blueprints allowed with same function name.
     */
    static void LoadMRecAd();

    UFUNCTION(BlueprintCallable, Category = "CAS Mobile Ads")
    static void ShowMRecAd();

    UFUNCTION(BlueprintCallable, Category = "CAS Mobile Ads")
    static void HideMRecAd();

    /**
     * Set position on screen for MediumRectangle Ad.
     * @param AdPosition - New MediumRectangle Ad position on screen.
     */
    UFUNCTION(BlueprintCallable, Category = "CAS Mobile Ads", meta = (AdPosition = "ECASPosition::BottomCenter"))
    static void SetMRecAdPosition(ECASPosition AdPosition);

    /**
     * An ad unit automatic refresh rate (in seconds) determines
     * how often a new ad request is generated for that ad unit.
     * Set 0 to disable automatic ad refresh.
     * Ad refresh each 30 seconds by default.
     */
    UFUNCTION(BlueprintCallable, Category = "CAS Mobile Ads", meta = (Interval = "30"))
    static void SetMRecAdRefreshInterval(int Interval);

    /**
     * Disable and Destroys loaded MediumRectangle view from memory.
     * If you want simple hide MediumRectangle from screen then use HideMRecAd() instead.
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
     * Subscribe to `OnInterstitialAdLoaded` and `OnInterstitialAdLoadFailed` events.
     * Blueprints allowed with same function name.
     */
    static void LoadInterstitialAd();

    /**
     * Check if the interstitial is loaded and ready to be displayed.
     */
    UFUNCTION(BlueprintCallable, Category = "CAS Mobile Ads", meta = (ExpandBoolAsExecs = "ReturnValue"))
    static bool IsInterstitialAdReady();

    /**
     * Present loaded interstitial.
     * Subscribe to events:
     * `OnInterstitialAdDisplayed`, `OnInterstitialAdDismissed`, `OnInterstitialAdShowFailed`, OnInterstitialAdClicked`
     * Blueprints allowed with same function name.
     */
    static void ShowInterstitialAd();

    /**
     * You can limit the posting of an interstitial ad to a period of time in seconds
     * after the ad is closed, during which display attempts will fail.
     * Disabled by default.
     */
    UFUNCTION(BlueprintCallable, Category = "CAS Mobile Ads")
    static void SetInterstitialAdMinimumInterval(int Interval);

    /**
     * Restarts the interval to allow Interstitial ads impression after new interval.
     */
    UFUNCTION(BlueprintCallable, Category = "CAS Mobile Ads")
    static void RestartInterstitialAdInterval();

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
     * Check if the rewarded ad is loaded and ready to be displayed.
     */
    UFUNCTION(BlueprintCallable, Category = "CAS Mobile Ads", meta = (ExpandBoolAsExecs = "ReturnValue"))
    static bool IsRewardedAdReady();

    /**
     * Present loaded rewarded ad.
     * Subscribe to events: `OnRewardedAdEarnedReward`,
     * `OnRewardedAdDisplayed`, `OnRewardedAdDismissed`, `OnRewardedAdShowFailed`, `OnRewardedAdClicked`
     * Blueprints allowed with same function name.
     */
    static void ShowRewardedAd();

    // MARK: Return to App Ads
   public:
    static FOnCASFailedDelegate OnReturnToAppAdShowFailed;
    static FOnCASCallbackDelegate OnReturnToAppAdDisplayed;
    static FOnCASCallbackDelegate OnReturnToAppAdClicked;
    static FOnCASCallbackDelegate OnReturnToAppAdDismissed;

    /**
     * The Return Ads which is displayed when the user returns to your application
     * after a certain period of time.
     * Subscribe to events:
     * `OnReturnToAppAdDisplayed`, `OnReturnToAppAdDismissed`, `OnReturnToAppAdShowFailed`, `OnReturnToAppAdClicked`
     * Blueprints allowed with same function name.
     */
    static void ShowAdOnReturnToApp();

    /**
     * Call this method when you do not want users to see ads when they return to
     * your application single time
     */
    UFUNCTION(BlueprintCallable, Category = "CAS Mobile Ads")
    static void SkipAdOnNextReturnToApp();

    /**
     * The Return Ads is not displayed if it detroyed.
     */
    UFUNCTION(BlueprintCallable, Category = "CAS Mobile Ads")
    static void DestroyReturnToAppAd();
};