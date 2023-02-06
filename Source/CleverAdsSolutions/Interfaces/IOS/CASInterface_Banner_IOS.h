// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CASInterface_Banner.h"
#include "CASInterface_Banner_IOS.generated.h"

/** CAS Banner interface class for Android. */
UCLASS()
class CLEVERADSSOLUTIONS_API UCASInterface_Banner_IOS : public UCASInterface_Banner
{
	GENERATED_BODY()

#if PLATFORM_IOS
	
	virtual void Init();
	
	virtual void CreateBanner() override;
	
	virtual void LoadNextBanner() override;

	virtual void DestroyBanner() override;
	
	virtual void ToggleBannerVisibility(bool Visible) override;

	virtual bool IsVisible() const override;
	
	virtual bool IsReady() const override;
	
	virtual void SetRefreshInterval(int Interval) override;
	
	virtual void DisableRefreshInterval() override;
#endif
};
