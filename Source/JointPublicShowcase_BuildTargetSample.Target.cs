// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;
using System.Collections.Generic;

public class JointPublicShowcase_BuildTargetSampleTarget : TargetRules
{
	public JointPublicShowcase_BuildTargetSampleTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Game;
		DefaultBuildSettings = BuildSettingsVersion.Latest;
		ExtraModuleNames.Add("JointPublicShowcase");
	}
}
