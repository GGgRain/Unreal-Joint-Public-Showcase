// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AttributeSet.h"
#include "AbilitySystemComponent.h"
#include "JPSAttributeSetBase.generated.h"

// Uses macros from AttributeSet.h
#define ATTRIBUTE_ACCESSORS(ClassName, PropertyName) \
	GAMEPLAYATTRIBUTE_PROPERTY_GETTER(ClassName, PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_GETTER(PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_SETTER(PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_INITTER(PropertyName)

/**
 * 
 */
UCLASS()
class JOINTPUBLICSHOWCASE_API UJPSAttributeSetBase : public UAttributeSet
{
	GENERATED_BODY()
	
public:
	
	UJPSAttributeSetBase();

	// AttributeSet Overrides
	virtual void PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue) override;
	virtual void PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data) override;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

public:
	
	UPROPERTY(BlueprintReadOnly, Category = "WalkSpeed", ReplicatedUsing = OnRep_WalkSpeed)
	FGameplayAttributeData WalkSpeed;
	ATTRIBUTE_ACCESSORS(UJPSAttributeSetBase, WalkSpeed)
	
	
	UPROPERTY(BlueprintReadOnly, Category = "JogSpeed", ReplicatedUsing = OnRep_JogSpeed)
	FGameplayAttributeData JogSpeed;
	ATTRIBUTE_ACCESSORS(UJPSAttributeSetBase, JogSpeed)
	
protected:
	
	// Helper function to proportionally adjust the value of an attribute when it's associated max attribute changes.
	// (i.e. When MaxHealth increases, Health increases by an amount that maintains the same percentage as before)
	void AdjustAttributeForMaxChange(FGameplayAttributeData& AffectedAttribute, const FGameplayAttributeData& MaxAttribute, float NewMaxValue, const FGameplayAttribute& AffectedAttributeProperty);

	/**
	* These OnRep functions exist to make sure that the ability system internal representations are synchronized properly during replication
	**/
	UFUNCTION()
	virtual void OnRep_WalkSpeed(const FGameplayAttributeData& OldWalkSpeed);
	
	UFUNCTION()
	virtual void OnRep_JogSpeed(const FGameplayAttributeData& OldJogSpeed);
	
};

#undef ATTRIBUTE_ACCESSORS