package com.unreal.cas;

import android.app.NativeActivity;

import androidx.annotation.Nullable;
import androidx.annotation.NonNull;

import com.cleversolutions.ads.*;
import com.cleversolutions.ads.android.CAS;

import android.util.Log;
import java.lang.reflect.Method;

public class CASUnrealInterstitial {
    private static MediationManager manager;
    private static NativeActivity activity;
    
    // CPP callbacks
    private native static void onInterstitialAdLoadedThunkCpp();
    private native static void onInterstitialAdShownThunkCpp(AdStatusHandler impression);
    private native static void onInterstitialAdClosedThunkCpp();
    private native static void onInterstitialAdClickedThunkCpp();
    private native static void onInterstitialAdCompletedThunkCpp();
    private native static void onInterstitialAdLoadFailedThunkCpp(String errorMessage);
    private native static void onInterstitialAdShowFailedThunkCpp(String errorMessage);
    
    public static void Init(MediationManager mediationManager, NativeActivity appActivity){
        manager = mediationManager;
        activity = appActivity;
        
        manager.getOnAdLoadEvent().add(new AdLoadCallback() {  
            @Override  
            public void onAdLoaded(@NonNull AdType type) {  
                if (type == AdType.Interstitial) {  
                    // Interstitial loaded  
                    onInterstitialAdLoadedThunkCpp();
                }  
            }  
          
            @Override  
            public void onAdFailedToLoad(@NonNull AdType type, @Nullable String error) {  
                if (type == AdType.Interstitial) {  
                    // Interstitial failed to load with error  
                    onInterstitialAdLoadFailedThunkCpp(error);
                }  
            }  
        });
    }
    
    public static void ShowInterstitial(){
        AdCallback callback = new AdCallback() {  
            @Override  
            public void onShown(@NonNull AdStatusHandler impression) {  
                // Called when ad is shown.
                
                Class<?> objectClass = impression.getClass();
                Log.d("CASLog", "Class: " + objectClass.getName());

                // Get and print all methods
                Method[] methods = objectClass.getDeclaredMethods();
                Log.d("CASLog", "\nMethods:");
                for (Method method : methods) {
                    Log.d("CASLog", " - " + method.toString());
                }
                       
                onInterstitialAdShownThunkCpp(impression);
            }  
          
            @Override  
            public void onShowFailed(@NonNull String message) {  
                // Called when ad fails to show. 
                onInterstitialAdShowFailedThunkCpp(message);
            }  
          
            @Override  
            public void onClicked() {  
                // Called when a click is recorded for an ad.  
                onInterstitialAdClickedThunkCpp();
            }  
          
            @Override  
            public void onComplete() {  
                // Called when ad is completed
                onInterstitialAdCompletedThunkCpp();
            }  
          
            @Override  
            public void onClosed() {  
                // Called when ad is dismissed
                onInterstitialAdClosedThunkCpp();
            }  
        };
        
        manager.showInterstitial(activity, callback);
    }
    
    public static boolean IsInterstitialReady(){
        return manager.isInterstitialReady();
    }
    
    public static void LoadInterstitial(){
        manager.loadInterstitial();
    }
    
    public static void setMinimumInterval(int interval){
        CAS.getSettings().setInterstitialInterval(interval);
    }
    
    public static void restartInterval(){
        CAS.getSettings().restartInterstitialInterval();
    }
}