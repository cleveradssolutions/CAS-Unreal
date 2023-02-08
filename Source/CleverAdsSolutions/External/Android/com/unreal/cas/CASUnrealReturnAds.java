package com.unreal.cas;

import android.app.NativeActivity;

import androidx.annotation.Nullable;
import androidx.annotation.NonNull;

import com.cleversolutions.ads.*;
import com.cleversolutions.ads.android.CAS;

public class CASUnrealReturnAds {
    private static MediationManager manager;
    private static NativeActivity activity;
    
    // CPP callbacks
    private native static void onReturnAdsShownThunkCpp(AdStatusHandler impression);
    private native static void onReturnAdsShowFailedThunkCpp(String message);
    private native static void onReturnAdsClosedThunkCpp();
    private native static void onReturnAdsClickedThunkCpp();
    private native static void onReturnAdsCompletedThunkCpp();
    
    public static void Init(MediationManager mediationManager, NativeActivity appActivity){
        manager = mediationManager;
        activity = appActivity;
    }
    
    public static void enableRewardAds(){
        manager.enableAppReturnAds(new AdCallback() {  
            @Override  
            public void onShown(@NonNull AdStatusHandler impression) {  
                onReturnAdsShownThunkCpp(impression);
            }
          
            @Override  
            public void onShowFailed(@NonNull String message) {  
                onReturnAdsShowFailedThunkCpp(message);
            }  
          
            @Override  
            public void onClicked() {  
                onReturnAdsClickedThunkCpp();
            }  
          
            @Override  
            public void onComplete() {  
                onReturnAdsCompletedThunkCpp();
            }  
          
            @Override  
            public void onClosed() {  
                onReturnAdsClosedThunkCpp();
            }  
        });
    }
    
    public static void disableAppReturnAds(){
        manager.disableAppReturnAds();
    }
    
    public static void skipNextAppReturnAds(){
        manager.skipNextAppReturnAds();
    }
    
}