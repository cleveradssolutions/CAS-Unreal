// Copyright CleverAdsSolutions LTD, CAS.AI. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "CASImpressionInfo.h"
#include "CASDefines.generated.h"

/** User age category */
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
    Undefined,
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
    Children,
    /**
     * Audiences over the age of 13 NOT subject to the restrictions of child protection laws.
     */
    NotChildren
};

/** User Consent status */
UENUM(BlueprintType)
enum class ECASUserConsentStatus : uint8 {
    /**
     * Mediation ads network behavior
     */
    Undefined,
    /**
     * User consents to behavioral targeting in compliance with GDPR.
     */
    Accepted,
    /**
     * User does not consent to behavioral targeting in compliance with GDPR.
     */
    Denied
};

/** User CCPA status */
UENUM(BlueprintType)
enum class ECASUserCCPAStatus : uint8 {
    /**
     * Mediation ads network behavior
     */
    Undefined,
    /**
     * User does not consent to the sale of his or her personal information in compliance with CCPA.
     */
    OptOutSale,
    /**
     * User consents to the sale of his or her personal information in compliance with CCPA.
     */
    OptInSale
};

/** Used to determine a gender in CAS > SetGender */
UENUM(BlueprintType)
enum class ECASGender : uint8 { Undefined, Male, Female };

/**
 * Ads Loading/Impression errors.
 * Use CASLibrary::GetAdsErrorMessage(ECASError) to get error message.
 */
UENUM(BlueprintType)
enum class ECASError : uint8 {
    Internal = 0,
    /**
     * Loading ads cannot be successful without an internet connection.
     */
    NoConnection = 2,
    /**
     * This means we are not able to serve ads to this person.
     * Note that if you can see ads while you are testing with enabled Test Ads Mode,
     * your implementation works correctly and people will be able to see ads in your app once it's live.
     */
    NoFill = 3,
    /**
     * A configuration error has been detected in one of the mediation ad networks.
     * Please report error message to your manager support.
     */
    Configuration = 6,
    /**
     * Ad are not ready to show.
     * You need to call Load ads or use one of the automatic cache mode.
     * If you are already using automatic cache mode then just wait a little longer.
     * You can always check if ad is ready to show.
     */
    NotReady = 11,
    /**
     * The manager you want to use is not active at the moment.
     */
    ManagerIsDisabled = 12,
    /**
     * Ad creative has reached its daily cap for user.
     * The reason is for cross promo only.
     */
    ReachedCap = 14,
    /**
     * The interval between impressions of Interstitial Ad has not yet passed.
     */
    IntervalNotYetPassed = 21,
    /**
     * You can not show ads because another fullscreen ad is being displayed at the moment.
     * Please check your ad call logic to eliminate duplicate impressions.
     */
    AlreadyDisplayed = 22,
    /**
     * Ads cannot be shown as the application is currently not visible to the user.
     */
    AppNotForeground = 23,
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

USTRUCT(BlueprintType)
struct FCASInitialConfig {
    GENERATED_BODY()

   public:
    /**
     * Get the CAS initialization error, or empty if the initialization succses.
     */
    UPROPERTY(BlueprintReadOnly)
    FString Error;

    /**
     * Get the user's ISO-2 country code, or empty if the location fails.
     */
    UPROPERTY(BlueprintReadOnly)
    FString CountryCode;

    /**
     * Get the user Consent status after initialization and Consent Messaging Platform.
     */
    UPROPERTY(BlueprintReadOnly)
    ECASUserConsentStatus UserConsentStatus;
};

// MARK: Multicast delegate
DECLARE_MULTICAST_DELEGATE(FOnCASCallbackDelegate);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnCASInitializedDelegate, const FCASInitialConfig & /*Config*/);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnCASImpressionDelegate, const FCASImpressionInfo & /*AdImpression*/);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnCASFailedDelegate, const ECASError /*Error*/);

// MARK: Dynamic Multicast Delegates
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FCASEvent);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FCASResultEvent, const ECASError, Error);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FCASInitResultEvent, const FCASInitialConfig &, Config);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FCASImpressionEvent, const FCASImpressionInfo &, AdImpression);
DECLARE_DYNAMIC_DELEGATE_OneParam(FCASImpressionDelegate, const FCASImpressionInfo &, AdImpression);

// MARK: Deprecated Dynamic Multicast Delegates
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FCASErrorEvent, FString, ErrorMessage);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FCASImpressionInfoEvent, FCASImpressionInfo, ImpressionInfo);

DEFINE_LOG_CATEGORY_STATIC(LogCAS, Log, All);