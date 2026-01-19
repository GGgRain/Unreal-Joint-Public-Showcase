// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "JPSGameplayAbility.generated.h"

/**
 * From Lyra Project by Epic Games
 *Defines how an ability is meant to activate.
 */
UENUM(BlueprintType)
enum class EJPSAbilityActivationPolicy : uint8
{
	// Try to activate the ability when the input is triggered.
	OnInputTriggered,

	// Continually try to activate the ability while the input is active.
	WhileInputActive,

	// Try to activate the ability when an avatar is assigned.
	OnSpawn
};


/**
 * From Lyra Project by Epic Games
 * Defines how an ability activates in relation to other abilities.
 */
UENUM(BlueprintType)
enum class EJPSAbilityActivationGroup : uint8
{
	// Ability runs independently of all other abilities.
	Independent,

	// Ability is canceled and replaced by other exclusive abilities.
	Exclusive_Replaceable,

	// Ability blocks all other exclusive abilities from activating.
	Exclusive_Blocking,

	MAX	UMETA(Hidden)
};


/**
 * 
 */
UCLASS()
class JOINTPUBLICSHOWCASE_API UJPSGameplayAbility : public UGameplayAbility
{
	GENERATED_BODY()
	
public:
	
	// Defines how this ability is meant to activate.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Lyra|Ability Activation")
	EJPSAbilityActivationPolicy ActivationPolicy;

	// Defines the relationship between this ability activating and other abilities activating.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Lyra|Ability Activation")
	EJPSAbilityActivationGroup ActivationGroup;

public:
	
	EJPSAbilityActivationPolicy GetActivationPolicy() const { return ActivationPolicy; }
	
	EJPSAbilityActivationGroup GetActivationGroup() const { return ActivationGroup; }
	
};
