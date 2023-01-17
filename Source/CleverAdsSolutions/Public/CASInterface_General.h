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

	/** Returns CAS library version.
	 * @return String with version number
	 */
	UFUNCTION(BlueprintCallable, Category="CAS|General")
	virtual FString GetCASVersion() const {return "";}
	
	/** Indicates if the application’s audio is muted. Affects initial mute state for all ads.
	 * Use this method only if your application has its own volume controls.
	 * @param Mute - Mute ads audio by default
	 */
	UFUNCTION(BlueprintCallable, Category="CAS|General")
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
	UFUNCTION(BlueprintCallable, Category="CAS|General")
	virtual void SetLoadingMode(ECASLoadingMode Mode) {};
};
