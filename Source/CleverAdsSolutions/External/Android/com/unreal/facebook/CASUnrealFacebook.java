package com.unreal.facebook;

import com.facebook.FacebookSdk;

public class CASUnrealFacebook {
    public static void disableLDU(){
        FacebookSdk.setDataProcessingOptions(new String[] {}); 
    }
    public static void enableLDU(){
        FacebookSdk.setDataProcessingOptions(new String[] {"LDU"}, 0, 0);
    }
    public static void enableLDULocation(){
        FacebookSdk.setDataProcessingOptions(new String[] {"LDU"}, 1, 1000);
    }
}