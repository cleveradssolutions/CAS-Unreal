// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;
using System.IO;

public class CleverAdsSolutions : ModuleRules
{
	public CleverAdsSolutions(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;
		string PluginPath = Utils.MakePathRelativeTo(ModuleDirectory, Target.RelativeEnginePath);
		
		PublicDependencyModuleNames.AddRange(
			new string[]
			{
				"Core",
				// ... add other public dependencies that you statically link with here ...
			}
		);
			
		
		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
				"CoreUObject",
				"Engine",
				"Slate",
				"SlateCore",
				"Settings"
				// ... add private dependencies that you statically link with here ...	
			}
		);

		if (Target.Platform == UnrealTargetPlatform.Android)
		{
			AdditionalPropertiesForReceipt.Add("AndroidPlugin", Path.Combine(PluginPath, "CAS_UPL_Android.xml"));
			
			PublicDependencyModuleNames.AddRange(
			new string[]
			{
				"Launch"
			});
		}

		if (Target.Platform == UnrealTargetPlatform.IOS)
		{
			AdditionalPropertiesForReceipt.Add("IOSPlugin", Path.Combine(PluginPath, "CAS_UPL_IOS.xml"));
			
			PublicAdditionalFrameworks.Add(
				new Framework(
					"CleverAdsSolutions",
					"External/IOS/CleverAdsSolutions.embeddedframework.zip"
				)
			);
		}
	}
}
