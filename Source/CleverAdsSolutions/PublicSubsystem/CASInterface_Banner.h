// Copyright CleverAdsSolutions LTD, CAS.AI. All Rights Reserved.

#pragma once

#include "CASInterface.h"
#include "CASInterface_Banner.generated.h"

UCLASS()
class CLEVERADSSOLUTIONS_API UCASInterface_Banner : public UCASInterface {
    GENERATED_BODY()

   private:
    ECASBannerSize AdSize;
    ECASPosition AdPosition;

   public:  // Delegates
    /** Called when Banner ad gets loaded and became ready to show. Use IsReady() to make additional checks */
    UPROPERTY(BlueprintAssignable) FCASEvent OnLoaded;

    /** Called when system fails to show an Banner ad.
     *@param ErrorMessage - String with an error message of what happened wrong
     */
    UPROPERTY(BlueprintAssignable)
    FCASErrorEvent OnFail;

    /** Called when Banner ad gets created and shown to a user */
    UPROPERTY(BlueprintAssignable)
    FCASImpressionInfoEvent OnShow;

    /** Called when user clicks on Banner ad. */
    UPROPERTY(BlueprintAssignable)
    FCASEvent OnClicked;

    /** Called when Banner ad gets destroyed */
    UPROPERTY(BlueprintAssignable)
    FCASEvent OnDestroyed;

    UPROPERTY(BlueprintAssignable, meta = (DeprecatedProperty, DeprecationMessage = "No longer support"))
    FCASEvent OnCompleted;

   public:  // Functions
    UCASInterface_Banner();

    /** Shows ad.
     * @param BannerSize - Size of the banner
     */
    UFUNCTION(BlueprintCallable, Category = "CleverAdsSolutions")
    void CreateBanner(ECASBannerSize BannerSize);

    UFUNCTION(BlueprintCallable, Category = "CleverAdsSolutions", meta = (BannerPosition = "ECASPosition::BottomCenter"))
    void UpdateBannerPosition(ECASPosition BannerPosition);

    /** Loads next banner and discards current one.
     */
    UFUNCTION(BlueprintCallable, Category = "CleverAdsSolutions")
    void LoadNextBanner();

    /** Destroys banner completely. If you want to hide it, use ToggleBannerVisibility. */
    UFUNCTION(BlueprintCallable, Category = "CleverAdsSolutions")
    void DestroyBanner();

    /** Toggles Banner visibility
     * @param Visible - Reveal or hide the Banner
     */
    UFUNCTION(BlueprintCallable, Category = "CleverAdsSolutions")
    void ToggleBannerVisibility(bool Visible);

    /** Checks if Banner ad is ready.
     * @return True if Banner ad is ready
     */
    UFUNCTION(BlueprintPure, Category = "CleverAdsSolutions")
    bool IsReady();

    /** Change the banner automatic refresh rate. */
    UFUNCTION(BlueprintCallable, Category = "CleverAdsSolutions")
    void SetRefreshInterval(int Interval);

    /** Disables the banner automatic refresh rate. */
    UFUNCTION(BlueprintCallable, Category = "CleverAdsSolutions")
    void DisableRefreshInterval();

    UFUNCTION(BlueprintPure, Category = "CleverAdsSolutions")
    virtual bool IsVisible();

   protected:
    UFUNCTION()
    void HandleAdLoaded();
    UFUNCTION()
    void HandleAdFailed(ECASError Error);
    UFUNCTION()
    void HandleAdImpression(const FCASImpressionInfo& Info);
    UFUNCTION()
    void HandleAdClicked();
};
