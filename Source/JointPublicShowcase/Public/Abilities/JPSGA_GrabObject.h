// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "JPSGameplayAbility.h"
#include "NativeGameplayTags.h"
#include "JPSGA_GrabObject.generated.h"

//GCN tag for grab object ability
JOINTPUBLICSHOWCASE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Cue_JPS_GrabObject);

/**
 * 
 */
UCLASS()
class JOINTPUBLICSHOWCASE_API UJPSGA_GrabObject : public UJPSGameplayAbility
{
	GENERATED_BODY()
	
public:
	
	UJPSGA_GrabObject(const FObjectInitializer& ObjectInitializer);
	
public:
	
	// we need to replicate the grabbed object so the gameplay cue can get it on clients
	// we need this because the gameplay cue doesn't provide a way to pass "GrabbedComponentSocketName", 
	// so we must send the ability object itself to the gameplay cue and let it extract the socket name from there
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category="Grab", Replicated)
	TWeakObjectPtr<UPrimitiveComponent> GrabbedComponent;
	
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category="Grab", Replicated)
	FName GrabbedComponentSocketName;
	
public:
	
	FBodyInstance* GetGrabbedBI() const;
	
public:
	
	UFUNCTION(BlueprintPure)
	FVector GetGrabbedBIWorldLocation() const;
	
	UFUNCTION(BlueprintPure)
	FVector GetAvatarHandWorldLocation(const FName HandBoneName = "Hand_R") const;
	
public:
	
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
public:
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Grab")
	bool bSavedGrabbedObjectbReplicates;

public:

	virtual bool CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags = nullptr, const FGameplayTagContainer* TargetTags = nullptr, OUT FGameplayTagContainer* OptionalRelevantTags = nullptr) const override;

	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

	virtual void InputReleased(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo) override;

	virtual void CancelAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateCancelAbility) override;
	
};
