// Copyright CleverAdsSolutions LTD, CAS.AI. All Rights Reserved.

#include "BlueprintAsync/CASShowAdConsentFlowProxy.h"
#include "CASMobileAds.h"

UCASShowAdConsentFlowProxy* UCASShowAdConsentFlowProxy::ShowAdConsentFlow(UObject* WorldContextObject) {
    UCASShowAdConsentFlowProxy* Proxy = NewObject<UCASShowAdConsentFlowProxy>();
    Proxy->WorldContextObject = WorldContextObject;
    return Proxy;
}

void UCASShowAdConsentFlowProxy::Activate() {
    if (OnCompleted.IsBound()) {
        UCASMobileAds::OnConsentFlowDismissed.AddUObject(this, &UCASShowAdConsentFlowProxy::HandleResult);
        RegisterWithGameInstance(WorldContextObject);
    } else {
        SetReadyToDestroy();
    }
    UCASMobileAds::ShowAdConsentFlow();
}

void UCASShowAdConsentFlowProxy::HandleResult() {
    OnCompleted.Broadcast();
    SetReadyToDestroy();
}
