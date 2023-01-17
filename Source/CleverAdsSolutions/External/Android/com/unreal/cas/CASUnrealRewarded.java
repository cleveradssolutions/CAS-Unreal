package com.unreal.cas;

import android.app.NativeActivity;

import androidx.annotation.Nullable;
import androidx.annotation.NonNull;

import com.cleversolutions.ads.*;
import com.cleversolutions.ads.android.CAS;

public class CASUnrealRewarded {
    private static MediationManager manager;
    private static NativeActivity activity;
    
    // CPP callbacks
    private native static void onRewardedAdLoadedThunkCpp();
    private native static void onRewardedAdShownThunkCpp();
    private native static void onRewardedAdClosedThunkCpp();
    private native static void onRewardedAdClickedThunkCpp();
    private native static void onRewardedAdCompletedThunkCpp();
    private native static void onRewardedAdLoadFailedThunkCpp(String errorMessage);
    private native static void onRewardedAdShowFailedThunkCpp(String errorMessage);
    
    public static void Init(MediationManager mediationManager, NativeActivity appActivity){
        manager = mediationManager;
        activity = appActivity;
        
        manager.getOnAdLoadEvent().add(new AdLoadCallback() {  
            @Override  
            public void onAdLoaded(@NonNull AdType type) {  
                if (type == AdType.Rewarded) {  
                    // Rewarded loaded  
                    onRewardedAdLoadedThunkCpp();
                }  
            }  
          
            @Override  
            public void onAdFailedToLoad(@NonNull AdType type, @Nullable String error) {  
                if (type == AdType.Rewarded) {  
                    // Rewarded failed to load with error  
                    onRewardedAdLoadFailedThunkCpp(error);
                }  
            }  
        });
    }
    
    public static void ShowRewarded(){
        AdCallback callback = new AdCallback() {  
            @Override  
            public void onShown(@NonNull AdStatusHandler ad) {  
                // Called when ad is shown.
                onRewardedAdShownThunkCpp();
            }  
          
            @Override  
            public void onShowFailed(@NonNull String message) {  
                // Called when ad fails to show. 
                onRewardedAdShowFailedThunkCpp(message);
            }  
          
            @Override  
            public void onClicked() {  
                // Called when a click is recorded for an ad.  
                onRewardedAdClickedThunkCpp();
            }  
          
            @Override  
            public void onComplete() {  
                // Called when ad is completed
                onRewardedAdCompletedThunkCpp();
            }  
          
            @Override  
            public void onClosed() {  
                // Called when ad is dismissed
                onRewardedAdClosedThunkCpp();
            }  
        };
        
        manager.showRewardedAd(activity, callback);
    }
    
    public static boolean IsRewardedReady(){
        return manager.isRewardedAdReady();
    }
}