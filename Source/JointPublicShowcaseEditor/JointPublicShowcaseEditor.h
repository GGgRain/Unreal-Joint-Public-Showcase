// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Modules/ModuleInterface.h"
#include "Modules/ModuleManager.h"
 
class FJointPublicShowcaseEditorModule : public IModuleInterface
{
public:
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
	
public:
	
	TSharedPtr<class FJointShowcaseEditorExtensionCallbackHandler> EditorExtensionHandler;
};