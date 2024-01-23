// Copyright CleverAdsSolutions LTD, CAS.AI. All Rights Reserved.

#pragma once

#include "CASInterface.h"
#include "CASDefines.h"
#include "CASInterface_General.generated.h"

UCLASS()
class CLEVERADSSOLUTIONS_API UCASInterface_General : public UCASInterface {
    GENERATED_BODY()

   public:
    /** Returns CAS library version.
     * @return String with version number
     */
    UFUNCTION(BlueprintCallable, Category = "CleverAdsSolutions")
    FString GetCASVersion() const;

    /** Validates integration and outputs validating info in log files. */
    UFUNCTION(BlueprintCallable, Category = "CleverAdsSolutions")
    void ValidateIntegration() const;

    /** Indicates if the application’s audio is muted. Affects initial mute state for all ads.
     * Use this method only if your application has its own volume controls.
     * @param Mute - Mute ads audio by default
     */
    UFUNCTION(BlueprintCallable, Category = "CleverAdsSolutions")
    void SetMuteAdSounds(bool Mute) const;

    /** Sets users children audience category.
     * If you know that the user falls within an age-restricted category (i.e., under the age of 16), you must set the
     * "Children" audience flag
     * @param Audience - User children audience status
     */
    UFUNCTION(BlueprintCallable, Category = "CleverAdsSolutions")
    void SetTaggedAudience(ECASAudience Audience) const;

    /** Sets Consent Flag in GDPR and Other Regions
     * Use it if you are using your own or a third-party party consent mechanism.
     * @param ConsentStatus - User Consent Status
     */
    UFUNCTION(BlueprintCallable, Category = "CleverAdsSolutions")
    void SetUserConsent(ECASUserConsentStatus ConsentStatus) const;

    /** Sets California Consumer Privacy Act status for a user
     * If a user does NOT opt out of interest-based advertising, set the OptInSale flag
     * If a user does opt out of interest-based advertising, set the OptOutSale flag
     * @param CCPAStatus - User CCPA Status
     */
    UFUNCTION(BlueprintCallable, Category = "CleverAdsSolutions")
    void SetCCPAStatus(ECASUserCCPAStatus CCPAStatus) const;
};
