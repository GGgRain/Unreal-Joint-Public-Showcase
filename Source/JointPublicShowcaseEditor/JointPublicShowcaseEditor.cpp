// Copyright Epic Games, Inc. All Rights Reserved.

#include "JointPublicShowcaseEditor.h"

#include "JointShowcaseEditorExtensionCallbackHandler.h"
#include "Modules/ModuleManager.h"

IMPLEMENT_MODULE(FJointPublicShowcaseEditorModule, JointPublicShowcaseEditor);

void FJointPublicShowcaseEditorModule::StartupModule()
{
	IModuleInterface::StartupModule();
	
	EditorExtensionHandler = MakeShared<FJointShowcaseEditorExtensionCallbackHandler>();
	EditorExtensionHandler->RegisterExtensions();
	
}

void FJointPublicShowcaseEditorModule::ShutdownModule()
{
	IModuleInterface::ShutdownModule();
	
	if (EditorExtensionHandler.IsValid()) EditorExtensionHandler->UnregisterExtensions();
}
