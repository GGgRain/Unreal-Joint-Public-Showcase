// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class JointPublicShowcaseEditor : ModuleRules
{
	public JointPublicShowcaseEditor(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PrivateDependencyModuleNames.AddRange(new string[] { "JointPublicShowcase" });

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
			"UnrealEd",
		});
	}
}
