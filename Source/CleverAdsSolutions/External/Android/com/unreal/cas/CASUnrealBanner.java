package com.unreal.cas;

import android.app.NativeActivity;

import androidx.annotation.Nullable;
import androidx.annotation.NonNull;
import android.view.View;
import android.widget.RelativeLayout;
import android.app.Activity;

import java.lang.Runnable;

import com.cleversolutions.ads.*;
import com.cleversolutions.ads.android.CAS;
import com.cleversolutions.ads.android.CASBannerView;

public class CASUnrealBanner {
    private static MediationManager manager;
    private static NativeActivity activity;
    private static CASBannerView bannerView;
    
    // CPP callbacks
    private native static void onBannerAdLoadedThunkCpp();
    private native static void onBannerAdShownThunkCpp(AdStatusHandler impression);
    private native static void onBannerAdClickedThunkCpp();
    private native static void onBannerAdDestroyedThunkCpp();
    private native static void onBannerAdShowFailedThunkCpp(String errorMessage);
    
    public static void Init(MediationManager mediationManager, NativeActivity appActivity){
        manager = mediationManager;
        activity = appActivity;
    }
    
    public static void CreateBanner(int bannerSize){
        if(bannerView != null) return;
        
        bannerView = new CASBannerView(activity, manager);
        
        bannerView.setManager(manager);
        
        bannerView.setAdListener(new AdViewListener() {
            @Override
            public void onAdViewLoaded(@NonNull CASBannerView view) {
              // Invokes this callback when ad loaded and ready to present.
              onBannerAdLoadedThunkCpp();
            }
        
            @Override
            public void onAdViewFailed(@NonNull CASBannerView view, @NonNull AdError error) {
              // Invokes this callback when an error occurred with the ad.
              // To see the error code, see `AdError.getCode()`.
              // To see a description of the error, see `AdError.getMessage()`.
              onBannerAdShowFailedThunkCpp(error.getMessage());
            }
        
            @Override
            public void onAdViewPresented(@NonNull CASBannerView view, @NonNull AdStatusHandler impression) {
              // Invokes this callback when the ad did present for a user with info about the impression.
              onBannerAdShownThunkCpp(impression);
            }
        
            @Override
            public void onAdViewClicked(@NonNull CASBannerView view) {
              // Invokes this callback when a user clicks the ad.
              onBannerAdClickedThunkCpp();
            }
        });
        
        AdSize adSize = AdSize.BANNER; // Default is banner
        
        switch (bannerSize)
        {
            // Banner
            case 0: adSize = AdSize.BANNER; break; 
            
            // Leaderboard
            case 1: adSize = AdSize.LEADERBOARD; break;
            
            // Rectangle
            case 2: adSize = AdSize.MEDIUM_RECTANGLE; break;
            
            // Adaptive
            case 3: adSize = AdSize.getAdaptiveBannerInScreen(activity); break;
            
            // Smart
            case 4: adSize = AdSize.getSmartBanner(activity); break;
            
            default: break;
        }
        
        bannerView.setSize(adSize);
        
        // Activity > RelativeLayout > Banner (with Relative params)
        
        RelativeLayout.LayoutParams layout = new RelativeLayout.LayoutParams(RelativeLayout.LayoutParams.WRAP_CONTENT, RelativeLayout.LayoutParams.WRAP_CONTENT);
        layout.addRule(RelativeLayout.CENTER_HORIZONTAL);
        layout.addRule(RelativeLayout.ALIGN_PARENT_BOTTOM);

        RelativeLayout rl = new RelativeLayout(activity);

        rl.addView(bannerView, layout);
                
        activity.runOnUiThread(new Runnable() {
            @Override
            public void run() {
               activity.addContentView(rl, new RelativeLayout.LayoutParams(RelativeLayout.LayoutParams.MATCH_PARENT,RelativeLayout.LayoutParams.MATCH_PARENT));
            }
        });
    }
    
    public static boolean IsBannerReady(){
        if(bannerView == null) return false;
        
        return bannerView.isAdReady();
    }
    
    public static void LoadNextAd(){
        if(bannerView == null) return;
        
        bannerView.loadNextAd();
    }
    
    public static void ToggleBannerVisibility(boolean Visible){
        if(bannerView == null) return;
        
        activity.runOnUiThread(new Runnable() {
            @Override
            public void run() {
                bannerView.setVisibility(Visible ? View.VISIBLE : View.GONE);
            }
        });
    }  
    
    public static boolean IsBannerVisible(){
        if(bannerView == null) return false;
        
        return bannerView.getVisibility() == View.VISIBLE;
    } 
    
    public static void DestroyBanner(){
        if(bannerView == null) return;
        
        activity.runOnUiThread(new Runnable() {
            @Override
            public void run() {
                bannerView.destroy();
                
                bannerView = null;
                
                onBannerAdDestroyedThunkCpp();
            }
        });
    }
    
    public static void setBannerRefreshInterval(int interval){
        if(bannerView == null) return;
        
        bannerView.setRefreshInterval(interval);
    }
    
    public static void disableBannerRefresh(){
        if(bannerView == null) return;
        
        bannerView.disableAdRefresh();
    } 
}