// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "Kismet/BlueprintFunctionLibrary.h"
#include "Templates/SubclassOf.h"
#include "GameFramework/Actor.h" 
#include "UObject/SoftObjectPtr.h"

#include "BasicStuffBPFL.generated.h"



class USkeletalMeshComponent;

/**
 * 
 */
UCLASS()
class JOINTPUBLICSHOWCASE_API UBasicStuffBPFL : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	
public:
	
	UFUNCTION(BlueprintPure, Category = "BasicMath")
	static TArray<int32> GetRangeArray(const int32 Start, const int32 End);
	
	UFUNCTION(BlueprintPure, Category = "BasicMath")
	static TArray<int32> GetRangeArrayForSize(const int32 Num);
	
public:
	
	UFUNCTION(BlueprintCallable, Category = "Basic")
	static void CopyMorphTargetsToAnother(USkeletalMeshComponent* FromSkelComp, USkeletalMeshComponent* ToSkelComp);
	
public:
	
	UFUNCTION(BlueprintCallable, Category = "Basic")
	static AActor* SpawnActor(const UObject* WorldContextObject, TSubclassOf<AActor> ActorClass, FTransform const& Transform);
	
public:
	
	UFUNCTION(BlueprintCallable, Category = "Editor")
	static void OpenEditorForAsset(UObject* Asset);

	/**
	 * Load a level in the editor for showcase purposes.
	 * It works differently on Editor and Packaged builds - in Editor, it opens the level in the editor while halting PIE/Simulate if any is running, while in Packaged builds it just loads the level.
	 * @param LevelToLoad The level to load.
	 */
	UFUNCTION(BlueprintCallable, Category = "Editor")
	static void LoadLevelForShowcase(const TSoftObjectPtr<UWorld> LevelToLoad);
	
};
