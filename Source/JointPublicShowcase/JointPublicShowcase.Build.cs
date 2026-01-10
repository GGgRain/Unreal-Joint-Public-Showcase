// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class JointPublicShowcase : ModuleRules
{
	public JointPublicShowcase(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicIncludePaths.AddRange(
			new string[] {
				"JointPublicShowcaseEditor"
			}
		);
		
		PublicDependencyModuleNames.AddRange(new string[]
		{
			"Core", 
			"CoreUObject", 
			"Engine", 
			"InputCore", 
			"HeadMountedDisplay",
			"PhysicsCore",
			"Joint",
			"JointNative",
			
			"GameplayAbilities", 
			"GameplayTags",
			"GameplayTasks",
			
			"AIModule",
			"GameFeatures"
		});
	}
}
