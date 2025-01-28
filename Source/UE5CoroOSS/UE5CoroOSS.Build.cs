// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class UE5CoroOSS : ModuleRules
{
	public UE5CoroOSS(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
		
		PublicIncludePaths.AddRange(
			new string[] {
				// ... add public include paths required here ...
			}
			);
				
		
		PrivateIncludePaths.AddRange(
			new string[] {
				// ... add other private include paths required here ...
			}
			);
			
		
		PublicDependencyModuleNames.AddRange(
			new []
			{
				"Core",
				"UE5Coro",
				"OnlineSubsystem",
				"OnlineSubsystemUtils", 
				"OnlineSubsystemBlueprints",
				"PlayFab",
				"PlayFabCommon",
				"PlayFabCpp"
			}
		);
			
		
		PrivateDependencyModuleNames.AddRange(
			new []
			{
				"CoreUObject",
				"Engine",
				"Slate",
				"SlateCore",
				"UE5Coro", 
				"OnlineSubsystem",
				"OnlineSubsystemUtils",
				"PlayFab",
				"PlayFabCommon",
				"PlayFabCpp"
			}
		);
		
		
		DynamicallyLoadedModuleNames.AddRange(
			new string[]
			{
				// ... add any modules that your module loads dynamically here ...
			}
			);
	}
}
