// Copyright Epic Games, Inc. All Rights Reserved.

#include "JointPublicShowcase.h"

#include "Editor/JointShowcaseEditorCallbackHub.h"
#include "Modules/ModuleManager.h"
#include "AbilitySystemGlobals.h"

IMPLEMENT_MODULE(FJointPublicShowcaseModule, JointPublicShowcase)

void FJointPublicShowcaseModule::StartupModule()
{
	UAbilitySystemGlobals::Get().InitGlobalData();
	
	IModuleInterface::StartupModule();
	
	EditorExtensionHub = MakeShared<FJointShowcaseEditorCallbackHub>();
	
}

void FJointPublicShowcaseModule::ShutdownModule()
{
	IModuleInterface::ShutdownModule();
	
	EditorExtensionHub.Reset();
}
