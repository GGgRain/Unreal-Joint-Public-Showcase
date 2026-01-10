// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "FragmentableCharacterInterface.generated.h"

class UCharacterFragment;

UINTERFACE(BlueprintType, Blueprintable)
class JOINTPUBLICSHOWCASE_API UFragmentableCharacterInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * Interface for characters that can be composed of fragments.
 */
class JOINTPUBLICSHOWCASE_API IFragmentableCharacterInterface
{
	GENERATED_BODY()
public:
	
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Fragmentable Character")
	void GetFragments(TArray<UCharacterFragment*>& OutFragments);
	
	/** Get all character fragments associated with this character. */
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Fragmentable Character")
	void BeginPlayFragments();
	
	/** Get all character fragments associated with this character. */
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Fragmentable Character")
	void EndPlayFragments();
	
public:
	
	virtual void GetFragments_Implementation(TArray<UCharacterFragment*>& OutFragments);
	virtual void BeginPlayFragments_Implementation();
	virtual void EndPlayFragments_Implementation();
	
};