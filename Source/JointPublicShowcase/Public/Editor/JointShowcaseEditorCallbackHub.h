#pragma once

#include "CoreMinimal.h"
#include "UObject/SoftObjectPtr.h"

class UWorld;
class UObject;

/**
 * Editor extension hub for Joint Showcase.
 * It holds delegates that can be bound to by editor modules so that BPFLs in runtime modules can call back into editor code.
 */

class JOINTPUBLICSHOWCASE_API FJointShowcaseEditorCallbackHub : public TSharedFromThis<FJointShowcaseEditorCallbackHub>
{
public:
	
	FJointShowcaseEditorCallbackHub();
	~FJointShowcaseEditorCallbackHub();

public:
	
	// DELEGATES
	
	DECLARE_MULTICAST_DELEGATE_OneParam(FOnOpenEditorForAsset, UObject*);
	DECLARE_MULTICAST_DELEGATE_OneParam(FOnChangeLevelOnEditor, TSoftObjectPtr<UWorld>);

public:
	
	//Delegate for UBasicStuffBPFL::OpenEditorForAsset
	FOnOpenEditorForAsset OnOpenEditorForAsset;
	
	//Delegate for UBasicStuffBPFL::LoadLevelForShowcase
	FOnChangeLevelOnEditor OnChangeLevelOnEditor;

public:
	
	static TWeakPtr<FJointShowcaseEditorCallbackHub> Get();
	
};
