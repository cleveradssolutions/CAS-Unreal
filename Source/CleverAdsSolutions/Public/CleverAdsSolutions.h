// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"

class UCASSettingsAndroid;
class UCASSettingsIOS;
class UCASSettings;
class UCASInterface;

class FCleverAdsSolutionsModule : public IModuleInterface
{
public:

	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;

	static inline FCleverAdsSolutionsModule& Get()
	{
		return FModuleManager::LoadModuleChecked<FCleverAdsSolutionsModule>("CleverAdsSolutions");
	}

	UCASSettingsAndroid* CASSettingsAndroid;
	UCASSettingsIOS* CASSettingsIOS;
	
	
};
