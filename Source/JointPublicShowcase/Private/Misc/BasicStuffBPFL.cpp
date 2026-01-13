// Fill out your copyright notice in the Description page of Project Settings.


#include "Misc/BasicStuffBPFL.h"

#include "Editor/JointShowcaseEditorCallbackHub.h"

TArray<int32> UBasicStuffBPFL::GetRangeArray(const int32 Start, const int32 End)
{
	TArray<int32> RangeArray;
	
	// Preallocate array size
	RangeArray.Reserve(End - Start + 1);
	 
	for (int32 i = Start; i <= End; ++i) RangeArray.Add(i);
	
	return RangeArray;
}

TArray<int32> UBasicStuffBPFL::GetRangeArrayForSize(const int32 Num)
{
	return GetRangeArray(0, Num - 1);
}

void UBasicStuffBPFL::CopyMorphTargetsToAnother(USkeletalMeshComponent* FromSkelComp, USkeletalMeshComponent* ToSkelComp)
{
	if (!FromSkelComp || !ToSkelComp) return;
	
	ToSkelComp->ClearMorphTargets();
	
	for (const TPair<FName, float>& MorphTargetCurve : FromSkelComp->GetMorphTargetCurves())
	{
		ToSkelComp->SetMorphTarget(MorphTargetCurve.Key, MorphTargetCurve.Value);
	}
}


AActor* UBasicStuffBPFL::SpawnActor(const UObject* WorldContextObject, TSubclassOf<AActor> ActorClass, FTransform const& Transform)
{
	if (!WorldContextObject) return nullptr;
	
	UWorld* World = GEngine->GetWorldFromContextObjectChecked(WorldContextObject);
	
	if (!World) return nullptr;
	
	FActorSpawnParameters SpawnParams;
	
	return World->SpawnActor<AActor>(ActorClass, Transform, SpawnParams);
}

void UBasicStuffBPFL::OpenEditorForAsset(UObject* Asset)
{
	FJointShowcaseEditorCallbackHub::Get().Pin()->OnOpenEditorForAsset.Broadcast(Asset);
}

void UBasicStuffBPFL::LoadLevelForShowcase(const TSoftObjectPtr<UWorld> LevelToLoad)
{
	
#if WITH_EDITOR
	
	// load level in the editor
	FJointShowcaseEditorCallbackHub::Get().Pin()->OnChangeLevelOnEditor.Broadcast(LevelToLoad);
	
#else
	
	if (!LevelToLoad.IsValid()) return;
	
	UGameplayStatics::OpenLevel(GWorld, LevelToLoad.GetAssetName());
	
#endif
	
}
