// Copyright Epic Games, Inc. All Rights Reserved.

using System.IO;
using UnrealBuildTool;

public class SurvivalProject : ModuleRules
{
	public SurvivalProject(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicIncludePaths.AddRange(new string[]
			{ "SurvivalProject" });
		PrivateIncludePaths.AddRange(new string[] { "SurvivalProject" });
		
		PublicDependencyModuleNames.AddRange(new string[]
		{
			"Core", 
			"CoreUObject", 
			"Engine", 
			"InputCore",
			"HeadMountedDisplay",
			"UMG",
			"Slate",
			"SlateCore",
			"EnhancedInput",
			"NetCore",
			"InventorySystemPlugin",
			"CraftSystemPlugin",
			"PlayerStatsSystemPlugin",
			"ModularGameplay",
			"ModularGameplayActors",
			"GameFeatures",
			"GameplayTasks",            
			"GameplayAbilities",          
			"GameplayTags",                
			"EnhancedInput",            
			"PhysicsCore",  
		});
		
		PrivateDependencyModuleNames.AddRange(new string[]
		{
			"Projects", 
			"OnlineSubsystem", 
			"OnlineSubsystemUtils"
		});
		
		// To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true
	}
}