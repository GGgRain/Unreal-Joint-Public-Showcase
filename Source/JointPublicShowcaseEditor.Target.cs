// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;
using System.Collections.Generic;

public class JointPublicShowcaseEditorTarget : TargetRules
{
	public JointPublicShowcaseEditorTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Editor;
		DefaultBuildSettings = BuildSettingsVersion.Latest;
		ExtraModuleNames.AddRange(new string[] { "JointPublicShowcase", "JointPublicShowcaseEditor" });
	}
}
