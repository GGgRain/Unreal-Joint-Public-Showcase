#pragma once

/**
 * Editor extension callback handler for Joint Showcase.
 * 
 */
class JOINTPUBLICSHOWCASEEDITOR_API FJointShowcaseEditorExtensionCallbackHandler : public TSharedFromThis<FJointShowcaseEditorExtensionCallbackHandler>
{
	
public:
	
	FJointShowcaseEditorExtensionCallbackHandler();
	~FJointShowcaseEditorExtensionCallbackHandler();
	
public:

	void RegisterExtensions();
	void UnregisterExtensions();
	
public:
	
	void CB_OpenEditorForAsset(UObject* Asset);
	void CB_ChangeLevelOnEditor(TSoftObjectPtr<UWorld> World);

};
