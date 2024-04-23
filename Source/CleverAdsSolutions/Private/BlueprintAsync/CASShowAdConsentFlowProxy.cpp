// Copyright CleverAdsSolutions LTD, CAS.AI. All Rights Reserved.

#include "BlueprintAsync/CASShowAdConsentFlowProxy.h"
#include "CASMobileAds.h"

UCASShowAdConsentFlowProxy* UCASShowAdConsentFlowProxy::ShowAdConsentFlow(UObject* WorldContextObject,
                                                                          bool IfRequired) {
    UCASShowAdConsentFlowProxy* Proxy = NewObject<UCASShowAdConsentFlowProxy>();
    Proxy->WorldContextObject = WorldContextObject;
    Proxy->IfRequiredOnly = IfRequired;
    return Proxy;
}

void UCASShowAdConsentFlowProxy::Activate() {
    if (OnCompleted.IsBound()) {
        UCASMobileAds::OnConsentFlowResult.AddUObject(this, &UCASShowAdConsentFlowProxy::HandleResult);
        RegisterWithGameInstance(WorldContextObject);
    } else {
        SetReadyToDestroy();
    }
    if (IfRequiredOnly) {
        UCASMobileAds::ShowAdConsentFlowIfRequired();
    } else {
        UCASMobileAds::ShowAdConsentFlow();
    }
}

void UCASShowAdConsentFlowProxy::HandleResult(ECASConsentFlowStatus Status) {
    if (Status == ECASConsentFlowStatus::Obtained || Status == ECASConsentFlowStatus::NotRequired ||
        Status == ECASConsentFlowStatus::Unavailable) {
        OnCompleted.Broadcast();
    } else {
        OnFailed.Broadcast();
    }
    SetReadyToDestroy();
}
