#include "JointShowcaseEditorExtensionCallbackHandler.h"

#include "Editor/JointShowcaseEditorCallbackHub.h"

FJointShowcaseEditorExtensionCallbackHandler::FJointShowcaseEditorExtensionCallbackHandler()
{
}

FJointShowcaseEditorExtensionCallbackHandler::~FJointShowcaseEditorExtensionCallbackHandler()
{
}

void FJointShowcaseEditorExtensionCallbackHandler::RegisterExtensions()
{
	
	// get runtime module and access UEditorBasicStuffBPFL's delegate
	FJointShowcaseEditorCallbackHub::Get().Pin()->OnOpenEditorForAsset.AddRaw(this, &FJointShowcaseEditorExtensionCallbackHandler::CB_OpenEditorForAsset);
	
}

void FJointShowcaseEditorExtensionCallbackHandler::UnregisterExtensions()
{
	
	// get runtime module and access UEditorBasicStuffBPFL's delegate
	FJointShowcaseEditorCallbackHub::Get().Pin()->OnOpenEditorForAsset.RemoveAll(this);
	
}

void FJointShowcaseEditorExtensionCallbackHandler::CB_OpenEditorForAsset(UObject* Asset)
{
	// open editor for asset
	if (Asset)
	{
		if (UAssetEditorSubsystem* AssetEditorSubsystem = GEditor->GetEditorSubsystem<UAssetEditorSubsystem>())
		{
			AssetEditorSubsystem->OpenEditorForAsset(Asset);
		}
	}	
	
}
