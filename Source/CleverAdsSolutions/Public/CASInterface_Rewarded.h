// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CASInterface_ScreenAd.h"
#include "CASInterface_Rewarded.generated.h"

/** Abstract CAS interface class for Rewarded.
 * Platform implementation inside derived classes
 */
UCLASS()
class CLEVERADSSOLUTIONS_API UCASInterface_Rewarded : public UCASInterface_ScreenAd
{
	GENERATED_BODY()
};
