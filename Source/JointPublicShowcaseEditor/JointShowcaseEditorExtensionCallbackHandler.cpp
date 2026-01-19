#include "JointShowcaseEditorExtensionCallbackHandler.h"

#include "FileHelpers.h"
#include "Async/Async.h"
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
	FJointShowcaseEditorCallbackHub::Get().Pin()->OnChangeLevelOnEditor.AddRaw(this, &FJointShowcaseEditorExtensionCallbackHandler::CB_ChangeLevelOnEditor);
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

void FJointShowcaseEditorExtensionCallbackHandler::CB_ChangeLevelOnEditor(TSoftObjectPtr<UWorld> World)
{
	if (World.IsNull()) return;
	
	// stop PIE/Simulate if any is running
	GEditor->RequestEndPlayMap();
	
	// async task 
	
	AsyncTask(ENamedThreads::AnyBackgroundThreadNormalTask, [World]()
	{
		// wait to ensure PIE/Simulate is fully stopped. 
		
		// wait until PIE/Simulate is fully stopped or max wait time is reached
		while (GEditor->ShouldEndPlayMap())
		{
			FPlatformProcess::Sleep(0.1f);
		}
		
		AsyncTask(ENamedThreads::GameThread, [World]()
		{
			// load level in the editor
			FEditorFileUtils::LoadMap(World.GetLongPackageName(), false, true);
		});
	});
}
