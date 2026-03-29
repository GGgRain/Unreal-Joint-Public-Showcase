//Copyright 2022~2024 DevGrain. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Node/SubNode/JointEdGraphNode_Fragment.h"
#include "DialogueEdFragment_TextStyle.generated.h"

class SVerticalBox;
class UVoltAnimationManager;
class SScrollBox;
/**
 * 
 */
UCLASS()
class JOINTNATIVEEDITOR_API UDialogueEdFragment_TextStyle : public UJointEdGraphNode_Fragment
{
	GENERATED_BODY()

public:

	UDialogueEdFragment_TextStyle();

public:

	virtual TSubclassOf<UJointNodeBase> SupportedNodeClass() override;


public:

	virtual bool CanHaveSubNode() const override;


public:
	
	virtual void OnNodeInstancePropertyChanged(const FPropertyChangedEvent& PropertyChangedEvent, const FString& PropertyName) override;
	
	virtual bool CanHaveBreakpoint() const override;
	
public:
	
	virtual void ModifyGraphNodeSlate(const TSharedPtr<SJointGraphNodeBase>& InGraphNodeSlate) override;

	virtual void UpdateGraphNodeSlate(const TSharedPtr<SJointGraphNodeBase>& InGraphNodeSlate) override;

public:

	/**
	 * A box that holds the style representing slate.
	 */
	TWeakPtr<SVerticalBox> StyleBox;
	
};
