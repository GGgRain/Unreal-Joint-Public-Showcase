//Copyright 2022~2024 DevGrain. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Node/SubNode/JointEdGraphNode_Fragment.h"
#include "DialogueEdFragment_SpeakerAndListener.generated.h"

class SScrollBox;
/**
 * 
 */
UCLASS()
class JOINTNATIVEEDITOR_API UDialogueEdFragment_SpeakerAndListener : public UJointEdGraphNode_Fragment
{
	GENERATED_BODY()

public:

	virtual TSubclassOf<UJointNodeBase> SupportedNodeClass() override;
	
	virtual void OnNodeInstancePropertyChanged(const FPropertyChangedEvent& PropertyChangedEvent, const FString& PropertyName) override;

public:
	
	virtual void ModifyGraphNodeSlate(const TSharedPtr<SJointGraphNodeBase>& InGraphNodeSlate) override;
	virtual void UpdateGraphNodeSlate(const TSharedPtr<SJointGraphNodeBase>& InGraphNodeSlate) override;

public:

	TWeakPtr<SScrollBox> SpeakersBox;
	TWeakPtr<SScrollBox> ListenersBox;

public:
	
	static FText GetParticipantTextFor(const FJointNodePointer& Pointer);
};
