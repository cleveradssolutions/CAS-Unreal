// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CASInterface_Banner.h"
#include "CASInterface_Banner_Android.generated.h"

/** CAS Banner interface class for Android. */
UCLASS()
class CLEVERADSSOLUTIONS_API UCASInterface_Banner_Android : public UCASInterface_Banner
{
	GENERATED_BODY()

#if PLATFORM_ANDROID
	
	virtual void Init();
	
	virtual void CreateBanner() override;
	
	virtual void LoadNextBanner() override;

	virtual void DestroyBanner() override;
	
	virtual void ToggleBannerVisibility(bool Visible) override;

	virtual bool IsVisible() const override;
	
	virtual bool IsReady() const override;
#endif
};
