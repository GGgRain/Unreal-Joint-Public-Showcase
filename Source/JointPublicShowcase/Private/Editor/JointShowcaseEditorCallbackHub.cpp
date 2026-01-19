#include "Editor/JointShowcaseEditorCallbackHub.h"

#include "JointPublicShowcase/JointPublicShowcase.h"

class FJointPublicShowcaseEditorModule;

FJointShowcaseEditorCallbackHub::FJointShowcaseEditorCallbackHub()
{
}

FJointShowcaseEditorCallbackHub::~FJointShowcaseEditorCallbackHub()
{
}

TWeakPtr<FJointShowcaseEditorCallbackHub> FJointShowcaseEditorCallbackHub::Get()
{
	// access module and get hub
	FJointPublicShowcaseModule& Module = FModuleManager::LoadModuleChecked<FJointPublicShowcaseModule>("JointPublicShowcase");
	return Module.EditorExtensionHub;
}
