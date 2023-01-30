// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CASInterface.h"
#include "CASInterface_General.generated.h"

/** Used to determine a gender in CAS > SetGender */
UENUM(BlueprintType)
enum class ECASGender : uint8
{
	Undefined,
	Male,
	Female
};

/** User age category */
UENUM(BlueprintType)
enum class ECASChildrenAudience : uint8
{
	Undefined,
	Children,
	NotChildren
};

/** User Consent status */
UENUM(BlueprintType)
enum class ECASUserConsentStatus : uint8
{
	Undefined,
	Denied,
	Accepted
};

/** User CCPA status */
UENUM(BlueprintType)
enum class ECASUserCCPAStatus : uint8
{
	Undefined,
	OptInSale,
	OptOutSale
};

/** Ads loading mode. Use CAS > SetLoadingMode to change the mode */
UENUM(BlueprintType)
enum class ECASLoadingMode : uint8
{
	FastestRequests,
	FastRequests,
	Optimal,
	HighPerformance,
	HighestPerformance,
	Manual
};

/** Abstract CAS interface class for general stuff.
 * Platform implementation inside derived classes
 */
UCLASS()
class CLEVERADSSOLUTIONS_API UCASInterface_General : public UCASInterface
{
	GENERATED_BODY()
	
public:
	
	/** Returns CAS library version.
	 * @return String with version number
	 */
	UFUNCTION(BlueprintCallable, Category="CleverAdsSolutions|General")
	virtual FString GetCASVersion() const {return "";}

	/** Validates integration and outputs validating info in log files. */
	UFUNCTION(BlueprintCallable, Category="CleverAdsSolutions|General")
	virtual void ValidateIntegration() {};
	
	/** Indicates if the application’s audio is muted. Affects initial mute state for all ads.
	 * Use this method only if your application has its own volume controls.
	 * @param Mute - Mute ads audio by default
	 */
	UFUNCTION(BlueprintCallable, Category="CleverAdsSolutions|General")
	virtual void SetMuteAdSounds(bool Mute) {};

	/** Sets loading mode for providing ads
	 *	-Mode-				-Load-		-Impact on performance-		-Memory usage-	-Actual ads-
	 *	FastestRequests		Auto		Very high					High			Most relevant
	 *	FastRequests		Auto		High						Balance			High relevance
	 *	Optimal (Default)	Auto		Balance						Balance			Balance
	 *	HighPerformance		Auto		Low							Low				Possible loss
	 *	HighestPerformance	Auto		Very low					Low				Possible loss
	 *	Manual				Manual		Very low					Low				Depends on the frequency
	 * @param Mode - New loading mode
	 */
	UFUNCTION(BlueprintCallable, Category="CleverAdsSolutions|General")
	virtual void SetLoadingMode(ECASLoadingMode Mode) {}

	/** Sets users children audience category.
	 * If you know that the user falls within an age-restricted category (i.e., under the age of 16), you must set the "Children" audience flag
	 * @param ChildrenAudienceStatus - User children audience status
	 */
	UFUNCTION(BlueprintCallable, Category="CleverAdsSolutions|General")
	virtual void SetTaggedAudience(ECASChildrenAudience ChildrenAudienceStatus) {}

	/** Sets Consent Flag in GDPR and Other Regions
	 * Use it if you are using your own or a third-party party consent mechanism.
	 * @param ConsentStatus - User Consent Status
	 */
	UFUNCTION(BlueprintCallable, Category="CleverAdsSolutions|General")
	virtual void SetUserConsent(ECASUserConsentStatus ConsentStatus) {}
	
	/** Sets California Consumer Privacy Act status for a user
	 * If a user does NOT opt out of interest-based advertising, set the OptInSale flag
	 * If a user does opt out of interest-based advertising, set the OptOutSale flag
	 * @param CCPAStatus - User CCPA Status
	 */
	UFUNCTION(BlueprintCallable, Category="CleverAdsSolutions|General")
	virtual void SetCCPAStatus(ECASUserCCPAStatus CCPAStatus) {}
	
};
