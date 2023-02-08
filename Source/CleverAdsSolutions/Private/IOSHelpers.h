#pragma once

#if PLATFORM_IOS

#include "CASDefines.h"

namespace CASIOSHelpers
{
	inline FCASImpressionInfo ParseImpressionInfo(id<CASStatusHandler> Impression)
	{
		FCASImpressionInfo ImpressionInfo;
		
		ImpressionInfo.RevenueCPM = (float)Impression.cpm;
		ImpressionInfo.Network = FString(Impression.network);
		ImpressionInfo.NetworkSDKVersion = FString(Impression.versionInfo);
		ImpressionInfo.UnitID = FString(Impression.identifier);
		ImpressionInfo.CreativeID = FString(Impression.creativeIdentifier);
		ImpressionInfo.TotalImpressions = (int)Impression.impressionDepth;
		ImpressionInfo.LifetimeRevenue = (float)Impression.lifetimeRevenue;
		
		switch (Impression.adType)
		{
		case CASTypeRewarded: ImpressionInfo.PlacementType = FString("RewardedAd"); break;
		case CASTypeInterstitial: ImpressionInfo.PlacementType = FString("Interstitial"); break;
		case CASTypeBanner: ImpressionInfo.PlacementType = FString("Banner"); break;
		default: break;
		}

		return ImpressionInfo;
	}
}
#endif
