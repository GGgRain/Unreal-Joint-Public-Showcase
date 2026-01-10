// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "CharacterFragment.generated.h"

class IFragmentableCharacterInterface;

/**
 * A fragment of character functionality that can be composed into a character.
 * (introduced mainly for the modular character cosmetics)
 */
UCLASS(EditInlineNew, Blueprintable, BlueprintType, DefaultToInstanced)
class JOINTPUBLICSHOWCASE_API UCharacterFragment : public UObject
{
	GENERATED_BODY()
	
public:
	
	UCharacterFragment();
	
public:
	
	UFUNCTION(BlueprintCallable, Category = "Character Fragment")
	void BeginPlayFragment();
	
	UFUNCTION(BlueprintCallable, Category = "Character Fragment")
	void EndPlayFragment();
	
public:
	
	UFUNCTION(BlueprintImplementableEvent, Category = "Character Fragment", meta = (BlueprintProtected = "true"))
	void OnBeginPlayFragment();
	virtual void OnBeginPlayFragment_Implementation();
	
	UFUNCTION(BlueprintImplementableEvent, Category = "Character Fragment", meta = (BlueprintProtected = "true"))
	void OnEndPlayFragment();
	virtual void OnEndPlayFragment_Implementation();
	
public:

	UFUNCTION(BlueprintPure, Category = "Character Fragment")
	TScriptInterface<IFragmentableCharacterInterface> GetOwningCharacter() const;
	
};

/**
 * An element that holds a character fragment instance.
 */
USTRUCT(BlueprintType)
struct JOINTPUBLICSHOWCASE_API FCharacterFragmentElement
{
	GENERATED_BODY()
	
public:
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Instanced, Category = "Character Fragment")
	UCharacterFragment* FragmentInstance;
	
};