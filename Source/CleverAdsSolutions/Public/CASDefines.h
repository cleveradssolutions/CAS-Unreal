// Copyright CleverAdsSolutions LTD, CAS.AI. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "CASImpressionInfo.h"

#include "CASDefines.generated.h"

UENUM(BlueprintType)
enum class ECASAudience : uint8 {
    /**
     * The user's age has not been determined.
     *
     * If your app's target age groups include both children and older audiences,
     * any ads that may be shown to children must comply with Google Play's Families Ads Program.
     * A neutral age screen must be implemented so that any ads not suitable for children are only shown to older
     * audiences. A neutral age screen is a mechanism to verify a user's age in a way that doesn't encourage them to
     * falsify their age and gain access to areas of your app that aren't designed for children, for example, an age
     * gate.
     *
     * You could change the audience at runtime after determining the user's age.
     */
    Undefined = 0,
    /**
     * Audiences under the age of 13 who subject of COPPA.
     *
     * When using this feature, a Tag For Users under the Age of Consent in Europe (TFUA) parameter
     * will be included in the ad request.
     * Also the state of GDPR and CCPA will be overridden automatically
     * to ECASUserConsentStatus::Denied and ECASUserCCPAStatus::OptOutSale
     *
     * It also allows application to comply with the Google Play Families Policy requirements:
     * - Ads displayed to those users do not involve interest-based advertising or remarketing;
     * - Ads displayed to those users present content that is appropriate for children;
     * - Ads displayed to those users follow the Families ad format requirements;
     * - Compliance with all applicable legal regulations and industry standards relating to advertising to
     * children.</para>
     */
    Children = 1,
    /**
     * Audiences over the age of 13 NOT subject to the restrictions of child protection laws.
     */
    NotChildren = 2
};

UENUM(BlueprintType)
enum class ECASUserConsentStatus : uint8 {
    /**
     * Mediation ads network behavior
     */
    Undefined = 0,
    /**
     * User consents to behavioral targeting in compliance with GDPR.
     */
    Accepted = 1,
    /**
     * User does not consent to behavioral targeting in compliance with GDPR.
     */
    Denied = 2
};

UENUM(BlueprintType)
enum class ECASUserCCPAStatus : uint8 {
    /**
     * Mediation ads network behavior
     */
    Undefined = 0,
    /**
     * User does not consent to the sale of his or her personal information in compliance with CCPA.
     */
    OptOutSale = 1,
    /**
     * User consents to the sale of his or her personal information in compliance with CCPA.
     */
    OptInSale = 2
};

UENUM(BlueprintType)
enum class ECASGender : uint8 { Undefined, Male, Female };

UENUM(BlueprintType)
enum class ECASUserDebugGeography : uint8 {
    /**
     * Debug geography disabled.
     */
    Disabled = 0,
    /**
     * Geography appears as in European Economic Area.
     */
    EEA = 1,
    /**
     * Renamed to Other
     */
    NotEEA = 2,
    /**
     * Geography appears as in a regulated US State for debug devices.
     */
    RegulatedUSState = 3,
    /**
     * Geography appears as in a region with no regulation in force.
     */
    Other = 4
};

/**
 * Ads Loading/Impression errors.
 * Use CASLibrary::GetAdsErrorMessage(ECASError) to get error message.
 */
UENUM(BlueprintType)
enum class ECASError : uint8 {
    /**
     * Indicates an internal error occurred.
     */
    Internal = 0,
    /**
     * Indicates that ads are not ready to be shown.
     * Ensure to call the appropriate ad loading method or use automatic load mode.
     * If using automatic load mode, wait a little longer for ads to be ready.
     */
    NotReady = 1,
    /**
     * Indicates that the device is rejected for services.
     * Services may not be available for some devices that do not meet the requirements.
     * For example, the country or version of the OS.
     */
    Rejected = 2,
    /**
     * Indicates that no ads are available to be served.
     * If ads are visible in demo mode, your implementation is correct, and ads will be served once live.
     */
    NoFill = 3,
    /**
     * Indicates that the ad creative has reached its daily cap for the user.
     * This is typically relevant for cross-promotion ads only.
     */
    ReachedCap = 6,
    /**
     * Indicates that the CAS is not initialized.
     */
    NotInitialized = 7,
    /**
     * Indicates a timeout error occurred because the advertising source did not respond in time.
     * The system will continue waiting for a response, which may delay ad loading or cause a loading error.
     */
    Timeout = 8,
    /**
     * Indicates that there is no internet connection available, which prevents ads from loading.
     */
    NoConnection = 9,
    /**
     * Indicates that there is a configuration error in one of the mediation ad sources.
     * Report this error to your support manager for further assistance.
     */
    Configuration = 10,
    /**
     * Indicates that the interval between impressions of interstitial ads has not yet passed.
     * To change the interval, use the AdsSettings.interstitialInterval method.
     * This error may also occur if a trial ad-free interval has been defined and has not yet passed since app start.
     */
    IntervalNotYetPassed = 11,
    /**
     * Indicates that another fullscreen ad is currently being displayed, preventing new ads from showing.
     * Review your ad display logic to avoid duplicate impressions.
     */
    AlreadyDisplayed = 12,
    /**
     * Indicates that ads cannot be shown because the application is not currently in the foreground.
     */
    AppNotForeground = 13,
    /**
     * Deprecated name of NotInitialized
     */
    ManagerIsDisabled = NotInitialized,
    /**
     * Not Error status.
     */
    OK = 30
};

UENUM(BlueprintType)
enum class ECASBannerSize : uint8 {
    /**
     * Standard banner size 320dp width and 50dp height
     */
    Banner = 0,
    /**
     * Pick Adaptive AdSize for screen width.
     * Pick the best ad size, adaptive banners use fixed aspect ratios instead of fixed heights.
     * The height of adaptive banners cannot be less than 50 dp and more than 250 dp.
     * This results in banner ads that occupy a more consistent portion
     * of the screen across devices and provide opportunities for improved performance.
     */
    Adaptive = 1,
    /**
     * Typically, Smart Banners on phones have a ECASBannerSize::Banner size.
     * Or on tablets a ECASBannerSize::Leaderboard size.
     */
    Smart = 2,
    /**
     * Leaderboard banner size 728dp width and 90dp height
     */
    Leaderboard = 3
};

UENUM(BlueprintType)
enum class ECASPosition : uint8 {
    TopCenter = 0,
    TopLeft = 1,
    TopRight = 2,
    BottomCenter = 3,
    BottomLeft = 4,
    BottomRight = 5,
};

UENUM(BlueprintType)
enum class ECASConsentFlowStatus : uint8 {
    /**
     * No used
     */
    Undefined = 0,
    /**
     * User consent obtained. Personalized vs non-personalized undefined.
     */
    Obtained = 3,
    /**
     * User consent not required.
     */
    NotRequired = 4,
    /**
     * User consent unavailable.
     */
    Unavailable = 5,
    /**
     * There was the internal error.
     */
    InternalError = 10,
    /**
     * There was an error loading data from the network.
     */
    NetworkError = 11,
    /**
     * There was the app not foreground error.
     */
    NotForegroundApp = 12,
    /**
     * There was an error with another form is still being displayed.
     */
    FlowStillPresenting = 13,
};

USTRUCT(BlueprintType)
struct FCASInitialConfig {
    GENERATED_BODY()

   public:
    FCASInitialConfig()
        : UserConsentStatus(ECASUserConsentStatus::Undefined),
        ConsentFlowStatus(ECASConsentFlowStatus::Undefined) {}

    /**
     * Get the CAS initialization error, or empty if the initialization succses.
     */
    UPROPERTY(BlueprintReadOnly, Category = "CAS Mobile Ads")
    FString Error;

    /**
     * Get the user's ISO-2 country code, or empty if the location fails.
     */
    UPROPERTY(BlueprintReadOnly, Category = "CAS Mobile Ads")
    FString CountryCode;

    /**
     * Get the user Consent status after initialization and Consent Messaging Platform.
     */
    UPROPERTY(BlueprintReadOnly, Category = "CAS Mobile Ads")
    ECASUserConsentStatus UserConsentStatus;

    /**
     * Get the consent flow status.
     */
    UPROPERTY(BlueprintReadOnly, Category = "CAS Mobile Ads")
    ECASConsentFlowStatus ConsentFlowStatus;
};

// MARK: Multicast delegate
DECLARE_MULTICAST_DELEGATE(FOnCASCallbackDelegate);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnCASInitializedDelegate, const FCASInitialConfig & /*Config*/);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnCASImpressionDelegate, const FCASImpressionInfo & /*AdImpression*/);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnCASFailedDelegate, const ECASError /*Error*/);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnCASConsentFlowCallbackDelegate, const ECASConsentFlowStatus /*Error*/);

// MARK: Dynamic Multicast Delegates
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FCASEvent);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FCASResultEvent, const ECASError, Error);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FCASInitResultEvent, const FCASInitialConfig &, Config);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FCASImpressionEvent, const FCASImpressionInfo &, AdImpression);
DECLARE_DYNAMIC_DELEGATE_OneParam(FCASImpressionDelegate, const FCASImpressionInfo &, AdImpression);
