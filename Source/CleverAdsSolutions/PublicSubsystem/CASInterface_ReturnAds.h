// Copyright CleverAdsSolutions LTD, CAS.AI. All Rights Reserved.

#pragma once

#include "CASInterface.h"
#include "CASInterface_ReturnAds.generated.h"

UCLASS()
class CLEVERADSSOLUTIONS_API UCASInterface_ReturnAds : public UCASInterface {
    GENERATED_BODY()

   public:  // Delegates
    /** Called when ad gets shown to a user */
    UPROPERTY(BlueprintAssignable)
    FCASImpressionInfoEvent OnShown;

    /** Called when ad failed to show */
    UPROPERTY(BlueprintAssignable)
    FCASErrorEvent OnShowError;

    /** Called when user clicks on ad. */
    UPROPERTY(BlueprintAssignable)
    FCASEvent OnClicked;

    /** Called when ad gets closed by a user */
    UPROPERTY(BlueprintAssignable)
    FCASEvent OnClosed;

   public:  // Functions
    UCASInterface_ReturnAds();

    /** Call this method when you do not want users to see ads when they return to your application */
    UFUNCTION(BlueprintCallable, Category = "CleverAdsSolutions")
    void SkipNextAppReturnAds();

    /** Disables return ads. Call when you no longer want to show ads to users after they return to the app */
    UFUNCTION(BlueprintCallable, Category = "CleverAdsSolutions")
    void EnableAppReturnAds();

    /** Disables return ads. Call when you no longer want to show ads to users after they return to the app */
    UFUNCTION(BlueprintCallable, Category = "CleverAdsSolutions")
    void DisableAppReturnAds();
    
   protected:
    UFUNCTION()
    void HandleAdFailed(ECASError Error);
    UFUNCTION()
    void HandleAdImpression(const FCASImpressionInfo& Info);
    UFUNCTION()
    void HandleAdClicked();
    UFUNCTION()
    void HandleAdClosed();
};
