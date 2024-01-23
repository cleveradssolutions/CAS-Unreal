// Copyright CleverAdsSolutions LTD, CAS.AI. All Rights Reserved.

#pragma once

#include "CASInterface_ScreenAd.h"
#include "CASInterface_Interstitial.generated.h"

UCLASS()
class CLEVERADSSOLUTIONS_API UCASInterface_Interstitial : public UCASInterface_ScreenAd
{
	GENERATED_BODY()

public:
    UCASInterface_Interstitial();
	
	virtual void Load() override;

	virtual bool IsReady() override;

	virtual void Show() override;

	/** Restarts the interval to enable Interstitial ads again. */
	UFUNCTION(BlueprintCallable, Category = "CleverAdsSolutions")
	void RestartInterval() const;

   protected:
    virtual void AdShownCallback(const FCASImpressionInfo& info) override;
};
