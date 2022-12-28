package com.unreal.cas;

import android.app.NativeActivity;

import com.cleversolutions.ads.*;
import com.cleversolutions.ads.android.CAS;

import com.unreal.cas.CASUnrealInterstitial;

public class CASUnrealManager {
    public static MediationManager manager;
    
    public static void Init(NativeActivity appActivity){
        manager = CAS.buildManager()
               // Set your CAS ID
               .withCasId("TODO: Get Id from configs")
               // List Ad formats used in app
               // TODO: Get ad types from config
               .withAdTypes(AdType.Banner, AdType.Interstitial, AdType.Rewarded)
               // Use Test ads or live ads
               .withTestAdMode(true)
               .initialize(appActivity);
        
        CASUnrealInterstitial.Init(manager, appActivity);
    }
}
