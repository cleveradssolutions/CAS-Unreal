// Copyright CleverAdsSolutions LTD, CAS.AI. All Rights Reserved.

#pragma once

#include "CASInterface_ScreenAd.h"
#include "CASInterface_Rewarded.generated.h"

UCLASS()
class CLEVERADSSOLUTIONS_API UCASInterface_Rewarded : public UCASInterface_ScreenAd {
    GENERATED_BODY()

   public:
    UCASInterface_Rewarded();

    virtual void Load() override;

    virtual bool IsReady() override;

    virtual void Show() override;

   protected:
    virtual void AdShownCallback(const FCASImpressionInfo& info) override;
};
