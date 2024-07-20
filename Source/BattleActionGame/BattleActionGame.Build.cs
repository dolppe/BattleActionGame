// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class BattleActionGame : ModuleRules
{
	public BattleActionGame(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
	
		PublicDependencyModuleNames.AddRange(new string[]
		{
			"Core", 
			"CoreUObject", 
			"Engine", 
			"InputCore",
			// GameFeatures
			"ModularGameplay",
			"GameFeatures",
			"ModularGameplayActors",
			// GAS
			"GameplayTags",
			"GameplayTasks",
			"GameplayAbilities",
			// CommonUser
			"CommonUser",
			// Input
			"InputCore",
			"EnhancedInput",
			// CommonGame
			"CommonGame",
			// UI
			"CommonUI",
			"UMG",
			"UIExtension",
			// Slate
			"Slate",
			"SlateCore"
		});

		PrivateDependencyModuleNames.AddRange(new string[] {  });

		// Uncomment if you are using Slate UI
		// PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });
		
		// Uncomment if you are using online features
		// PrivateDependencyModuleNames.Add("OnlineSubsystem");

		// To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true
	}
}
