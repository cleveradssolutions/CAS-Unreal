// Copyright CleverAdsSolutions LTD, CAS.AI. All Rights Reserved.

#include "BlueprintAsync/CASInitializeAdProxy.h"
#include "CASMobileAds.h"

UCASInitializeAdProxy* UCASInitializeAdProxy::InitializeMobileAds(UObject* WorldContextObject) {
    UCASInitializeAdProxy* Proxy = NewObject<UCASInitializeAdProxy>();
    Proxy->WorldContextObject = WorldContextObject;
    return Proxy;
}

void UCASInitializeAdProxy::Activate() {
    if (UCASMobileAds::OnAdsInitialized.IsBound()) {
        UCASMobileAds::OnAdsInitialized.AddUObject(this, &UCASInitializeAdProxy::HandleResult);
        RegisterWithGameInstance(WorldContextObject);
    } else {
        SetReadyToDestroy();
    }
    UCASMobileAds::InitializeMobileAds();
}

void UCASInitializeAdProxy::HandleResult(const FCASInitialConfig& Config) {
    if (Config.Error.IsEmpty()) {
        OnCompleted.Broadcast(Config);
        SetReadyToDestroy();
    } else {
        OnFailed.Broadcast(Config);
    }
}
