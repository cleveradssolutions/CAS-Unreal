// Copyright CleverAdsSolutions LTD, CAS.AI. All Rights Reserved.

#pragma once

#include "CASInterface.h"
#include "CASInterface_ScreenAd.generated.h"

UCLASS()
class CLEVERADSSOLUTIONS_API UCASInterface_ScreenAd : public UCASInterface {
    GENERATED_BODY()

   public:  // Delegates
    /** Called when ad gets loaded and became ready to show. Use IsReady() to make additional check */
    UPROPERTY(BlueprintAssignable)
    FCASEvent OnLoaded;

    /** Called when system fails to load an ad.
     *@param ErrorMessage - String with an error message of what happened wrong
     */
    UPROPERTY(BlueprintAssignable)
    FCASErrorEvent OnLoadError;

    /** Called when ad gets shown to a user */
    UPROPERTY(BlueprintAssignable)
    FCASImpressionInfoEvent OnShown;

    /** Called when system fails to show an ad.
     *@param ErrorMessage - String with an error message of what happened wrong
     */
    UPROPERTY(BlueprintAssignable)
    FCASErrorEvent OnShowError;

    /** Called when user clicks on ad. */
    UPROPERTY(BlueprintAssignable)
    FCASEvent OnClicked;

    /** Called when ad completes to show */
    UPROPERTY(BlueprintAssignable)
    FCASEvent OnCompleted;

    /** Called when ad gets closed by a user */
    UPROPERTY(BlueprintAssignable)
    FCASEvent OnClosed;

   public:  // Functions
    /** Shows ad. Will do nothing if ad is not loaded. Use IsReady() to check if it is ready. */
    UFUNCTION(BlueprintCallable, Category = "CleverAdsSolutions")
    virtual void Show();

    /** Checks if ad is loaded and ready to show
     * @return True if ad is loaded and is ready
     */
    UFUNCTION(BlueprintPure, Category = "CleverAdsSolutions")
    virtual bool IsReady();

    /** Manually loads a new ad. By default new ads load automatically. */
    UFUNCTION(BlueprintCallable, Category = "CleverAdsSolutions")
    virtual void Load();

   protected:
    UFUNCTION()
    virtual void AdShownCallback(const FCASImpressionInfo& info);
    UFUNCTION()
    void AdLoadedCallback();
    UFUNCTION()
    void AdFailedCallback(ECASError error);
    UFUNCTION()
    void AdShowFailedCallback(ECASError error);
    UFUNCTION()
    void AdClickedCallback();
    UFUNCTION()
    void AdCompletedCallback();
    UFUNCTION()
    void AdClosedCallback();
};