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
    
    private native static void onCASInitThunkCpp();
    
    public static void InitActivity(NativeActivity appActivity){
        activity = appActivity;
    }
    
    public static void Init(String engineVersion){
        try {
            ApplicationInfo info = activity.getPackageManager().getApplicationInfo(activity.getPackageName(), PackageManager.GET_META_DATA);
            Bundle bundle = info.metaData;
            String AppID = bundle.getString("cas.sdk.appid");
            Boolean TestMode = bundle.containsKey("cas.sdk.testmode");
            Boolean DebugMode = bundle.containsKey("cas.sdk.debugmode");
            Boolean ConsentFlow = bundle.containsKey("cas.sdk.consentflow");
            Boolean EnableBanner = bundle.containsKey("cas.sdk.enablebanner");
            Boolean EnableInter = bundle.containsKey("cas.sdk.enableinter");
            Boolean EnableReward = bundle.containsKey("cas.sdk.enablereward");
            Boolean AllowInterstitialWhenLowRewards = bundle.containsKey("cas.sdk.interstitialwhenlowrewards");
            
            CAS.getSettings().setAllowInterstitialAdsWhenVideoCostAreLower(AllowInterstitialWhenLowRewards);
            
            CAS.getSettings().setDebugMode(DebugMode);
            
            int enableAdTypes = AdTypeFlags.None;
            
            if (EnableBanner)
                enableAdTypes |= AdTypeFlags.Banner;

            if (EnableInter)
                enableAdTypes |= AdTypeFlags.Interstitial;

            if (EnableReward)
                enableAdTypes |= AdTypeFlags.Rewarded;
            
            manager = CAS.buildManager()
                   // Set your CAS ID
                   .withCasId(AppID)
                   // List Ad formats used in app
                   .withEnabledAdTypes(enableAdTypes)
                   // Use Test ads or live ads
                   .withTestAdMode(TestMode)
                   .withFramework("Unreal", engineVersion)
                   .withConsentFlow(new ConsentFlow(ConsentFlow))
                   .withCompletionListener(config -> {
                        boolean successful = config.getError() == null;
                        if(successful) onCASInitThunkCpp();
                    })
                   .initialize(activity);
            
            CASUnrealInterstitial.Init(manager, activity);
            CASUnrealRewarded.Init(manager, activity);
            CASUnrealBanner.Init(manager, activity);
                    
        } catch (NameNotFoundException e) {}
    }
    
    public static void setLastPageAdInfo(String headline, String adText, String destinationURL, String imageURL, String iconURL){
        manager.setLastPageAdContent(new LastPageAdContent(headline, adText, destinationURL, imageURL, iconURL));
    }
    
    public static void addTestDevice(String testDevice){
        CAS.getSettings().getTestDeviceIDs().add(testDevice);
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
    
    public static void SetTaggedAudience(int audienceStatus){
        if(audienceStatus == 0) CAS.getSettings().setTaggedAudience(Audience.UNDEFINED);
        if(audienceStatus == 1) CAS.getSettings().setTaggedAudience(Audience.CHILDREN);
        if(audienceStatus == 2) CAS.getSettings().setTaggedAudience(Audience.NOT_CHILDREN);
    }
    
    public static void SetUserConsent(boolean accepted){
        CAS.getSettings().setUserConsent(accepted ? ConsentStatus.ACCEPTED : ConsentStatus.DENIED);
    }
    
    public static void SetCCPAStatus(boolean inSale){
        CAS.getSettings().setCcpaStatus(inSale ? CCPAStatus.OPT_IN_SALE : CCPAStatus.OPT_OUT_SALE);
    }
}
