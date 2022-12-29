package com.unreal.cas;

import android.app.NativeActivity;

import androidx.annotation.Nullable;
import androidx.annotation.NonNull;
import android.view.View;
import androidx.constraintlayout.widget.ConstraintLayout;
import androidx.constraintlayout.widget.ConstraintLayout.LayoutParams;
import android.view.Gravity;
import android.app.Activity;

import com.cleversolutions.ads.*;
import com.cleversolutions.ads.android.CAS;
import com.cleversolutions.ads.android.CASBannerView;

public class CASUnrealBanner {
    private static MediationManager manager;
    private static NativeActivity activity;
    private static CASBannerView bannerView;
    
    // CPP callbacks
    private native static void onBannerAdLoadedThunkCpp();
    private native static void onBannerAdShownThunkCpp();
    private native static void onBannerAdClickedThunkCpp();
    private native static void onBannerAdDestroyedThunkCpp();
    private native static void onBannerAdShowFailedThunkCpp(String errorMessage);
    
    public static void Init(MediationManager mediationManager, NativeActivity appActivity){
        manager = mediationManager;
        activity = appActivity;
    }
    
    public static void CreateBanner(){
        if(bannerView != null) return;
        
        bannerView = new CASBannerView(activity, manager);
        
// TODO: Load layout or move programmically a banner
//        CASBannerView bannerView = (CASBannerView) activity.findViewById(R.id.bannerView);
//        bannerView.setManager(manager);
        
            
//        
//        ConstraintLayout.LayoutParams layout = new ConstraintLayout.LayoutParams(
//            ConstraintLayout.LayoutParams.WRAP_CONTENT, 
//            ConstraintLayout.LayoutParams.WRAP_CONTENT);
//        
//        layout.addRule(ConstraintLayout.LayoutParams.BOTTOM);
//        layout.bottomMargin = 4;
//        
//        ConstraintLayout layout = new ConstraintLayout();
//        ConstraintSet set = new ConstraintSet();
//        
//        bannerView.setId(View.generateViewId());  // cannot set id after add
//        layout.addView(bannerView,0);
//        set.clone(layout);
//        set.connect(bannerView.getId(), ConstraintSet.TOP, layout.getId(), ConstraintSet.TOP, 60);
//        set.applyTo(layout);

//        ConstraintLayout = findViewById<ConstraintLayout>(R.id.banner_layout);
        
        //
        
        // WRAP_CONTENT - Start
        // MATCH_PARENT - Center
        LayoutParams layout = new LayoutParams(LayoutParams.MATCH_PARENT, LayoutParams.WRAP_CONTENT);
        
        //bannerView = findViewById<CASBannerView>(R.id.bannerView);
        
        //layout.addRule(RelativeLayout.ALIGN_PARENT_END);
        
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
            public void onAdViewPresented(@NonNull CASBannerView view, @NonNull AdStatusHandler info) {
              // Invokes this callback when the ad did present for a user with info about the impression.
              onBannerAdShownThunkCpp();
            }
        
            @Override
            public void onAdViewClicked(@NonNull CASBannerView view) {
              // Invokes this callback when a user clicks the ad.
              onBannerAdClickedThunkCpp();
            }
        });
                
        activity.runOnUiThread(new Runnable() {
            @Override
            public void run() {
                activity.addContentView(bannerView, layout);
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
}