// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

class FJointPublicShowcaseModule : public IModuleInterface
{
public:
	
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
	
public:
	
	TSharedPtr<class FJointShowcaseEditorCallbackHub> EditorExtensionHub;
	
};