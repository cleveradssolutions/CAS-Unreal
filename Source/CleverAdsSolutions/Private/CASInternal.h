// Copyright CleverAdsSolutions LTD, CAS.AI. All Rights Reserved.

#pragma once

#include "CASDefines.h"
#include "CASExternDefines.h"
#include "CoreMinimal.h"
#include "Kismet/BlueprintAsyncActionBase.h"

DECLARE_LOG_CATEGORY_EXTERN(LogCAS_AI, Log, All);

#define CAS_LOG_D(message, ...)                                      \
    {                                                                \
        if (true) {                                                  \
            UE_LOG(LogCAS_AI, Display, TEXT(message), ##__VA_ARGS__) \
        }                                                            \
    }
#define CAS_LOG_W(message, ...) UE_LOG(LogCAS_AI, Warning, TEXT(message), ##__VA_ARGS__)
#define CAS_LOG_E(message, ...) UE_LOG(LogCAS_AI, Error, TEXT(message), ##__VA_ARGS__)

#define CAS_BOOL_TO_STR(value) (value ? TEXT("true") : TEXT("false"))

#define CAS_FORMAT_BANNER TEXT("Banner")
#define CAS_FORMAT_MREC TEXT("MediumRectangle")
#define CAS_FORMAT_INTERSTITIAL TEXT("Interstitial")
#define CAS_FORMAT_REWARDED TEXT("Rewarded")
#define CAS_FORMAT_RETURN_TO_APP TEXT("Return To App")

namespace CASInternal {
// region For each platform implementation in CASMobileAds.cpp
void FillImpressionInfo(int AdType, FCASImpressionInfo& Info);
void FillInitialConfig(FCASInitialConfig& Config);
void ShowConsentFlow(bool ifRequired);
// endregion

void HandleEventCallback(int adType, int callback, int error);

void KeepProxy(int32 Type, UBlueprintAsyncActionBase* Proxy);

extern FCASImpressionEvent OnAdsImpression;
}  // namespace CASInternal