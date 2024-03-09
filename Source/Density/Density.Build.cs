// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class Density : ModuleRules
{
	public Density(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
       
        PublicIncludePaths.AddRange(
                new string[] {
                "Density"

                }
            );

        PrivateIncludePaths.AddRange(
            new string[] {
            }
        );

        PublicDependencyModuleNames.AddRange(
			new string[] 
			{	
				"Core", 
				"CoreUObject", 
				"Engine", 
				"InputCore",
                "GameplayAbilities", 
				"GameplayTags", 
				"GameplayTasks",
                "ModularGameplay",
                "PhysicsCore",
                "GameFeatures",
                "NetCore"
            });

		PrivateDependencyModuleNames.AddRange(
			new string[] 
			{
                "CoreUObject",
                "Engine",
                "Slate",
                "SlateCore",
                "UMG",
                "EnhancedInput",
                "GameplayAbilities",
                "GameplayTags",
                "GameplayTasks", 
                "AIModule"
			});

        // Uncomment if you are using Slate UI
        // PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });

        // Uncomment if you are using online features
        // PrivateDependencyModuleNames.Add("OnlineSubsystem");

        // To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true
    }
}
