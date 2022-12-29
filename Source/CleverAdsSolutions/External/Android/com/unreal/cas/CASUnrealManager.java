package com.unreal.cas;

import android.app.NativeActivity;
import android.content.pm.ApplicationInfo;
import android.content.pm.PackageManager;
import android.content.pm.PackageManager.NameNotFoundException;
import android.os.Bundle;

import com.cleversolutions.ads.*;
import com.cleversolutions.ads.android.CAS;

import com.unreal.cas.CASUnrealInterstitial;
import com.unreal.cas.CASUnrealRewarded;
import com.unreal.cas.CASUnrealBanner;

public class CASUnrealManager {
    public static MediationManager manager;
    
    public static void Init(NativeActivity appActivity){
        try {
            ApplicationInfo info = appActivity.getPackageManager().getApplicationInfo(appActivity.getPackageName(), PackageManager.GET_META_DATA);
            Bundle bundle = info.metaData;
            String AppID = bundle.getString("cas.sdk.appid");
            Boolean TestMode = bundle.containsKey("cas.sdk.testmode");
            
            manager = CAS.buildManager()
                   // Set your CAS ID
                   .withCasId(AppID)
                   // List Ad formats used in app
                   // TODO: Get ad types from config
                   .withAdTypes(AdType.Banner, AdType.Interstitial, AdType.Rewarded)
                   // Use Test ads or live ads
                   .withTestAdMode(TestMode)
                   .initialize(appActivity);
            
            CASUnrealInterstitial.Init(manager, appActivity);
            CASUnrealRewarded.Init(manager, appActivity);
            CASUnrealBanner.Init(manager, appActivity);
                    
        } catch (NameNotFoundException e) {}
    }
}
