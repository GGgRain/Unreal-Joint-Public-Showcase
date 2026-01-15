// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;
using System.Collections.Generic;

public class JointPublicShowcaseTarget : TargetRules
{
	public JointPublicShowcaseTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Game;
		DefaultBuildSettings = BuildSettingsVersion.V2;
		ExtraModuleNames.Add("JointPublicShowcase");
		
		if (Target.Version.MajorVersion == 5 && Target.Version.MinorVersion <= 2)
		{
			WindowsPlatform.CompilerVersion = "14.29.30159";
		}
		else if (Target.Version.MajorVersion == 5 && Target.Version.MinorVersion <= 6)
		{
			WindowsPlatform.CompilerVersion = "14.38.33144";
		}
		else
		{
			WindowsPlatform.CompilerVersion = "14.38.33145";
		}
		
	}
}
