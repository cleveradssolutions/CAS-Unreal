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
    public static NativeActivity activity;
    
    public static void Init(NativeActivity appActivity){
        activity = appActivity;
        try {
            ApplicationInfo info = activity.getPackageManager().getApplicationInfo(activity.getPackageName(), PackageManager.GET_META_DATA);
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
                   .initialize(activity);
            
            CASUnrealInterstitial.Init(manager, activity);
            CASUnrealRewarded.Init(manager, activity);
            CASUnrealBanner.Init(manager, activity);
                    
        } catch (NameNotFoundException e) {}
    }
    
    public static String GetCASVersion(){
        return CAS.getSDKVersion();
    }
    
    public static void ToggleAdSoundMute(boolean mute){
        CAS.getSettings().setMutedAdSounds(mute);
    }
    
    public static void SetLoadingMode(int mode){
        CAS.getSettings().setLoadingMode(mode);
    }
    
    public static void ValidateIntegration(){
        CAS.validateIntegration(activity);
    }
}
