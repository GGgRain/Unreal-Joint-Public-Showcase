// Fill out your copyright notice in the Description page of Project Settings.


#include "Abilities/JPSGA_Run.h"
#include "Abilities/GameplayAbilityTypes.h"
#include "Abilities/AttributeSets/JPSAttributeSetBase.h"

#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/PawnMovementComponent.h"

UJPSGA_Run::UJPSGA_Run(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::LocalPredicted;
	InstancingPolicy = EGameplayAbilityInstancingPolicy::NonInstanced;
}

bool UJPSGA_Run::CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags, const FGameplayTagContainer* TargetTags,
	FGameplayTagContainer* OptionalRelevantTags) const
{
	if (!Super::CanActivateAbility(Handle, ActorInfo, SourceTags, TargetTags, OptionalRelevantTags))
	{
		return false;
	}

	const ACharacter* Character = CastChecked<ACharacter>(ActorInfo->AvatarActor.Get(), ECastCheckedType::NullAllowed);
	return Character != nullptr && Character->GetCharacterMovement() != nullptr && !Character->GetCharacterMovement()->IsCrouching();
}

void UJPSGA_Run::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	if (HasAuthorityOrPredictionKey(ActorInfo, &ActivationInfo))
	{
		if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
		{
			return;
		}

		ACharacter* Character = CastChecked<ACharacter>(ActorInfo->AvatarActor.Get());
		
		if (UCharacterMovementComponent* MovementComponent = Character->GetCharacterMovement())
		{
			const UAbilitySystemComponent* ASC = ActorInfo->AbilitySystemComponent.Get();
			if (!ASC) return;

			const UJPSAttributeSetBase* AttributeSet = ASC->GetSet<UJPSAttributeSetBase>();
			if (!AttributeSet) return;
			
			MovementComponent->MaxWalkSpeed = AttributeSet->GetJogSpeed();
		}
	}
}

void UJPSGA_Run::InputReleased(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo)
{
	if (ActorInfo != NULL && ActorInfo->AvatarActor != NULL)
	{
		CancelAbility(Handle, ActorInfo, ActivationInfo, true);
	}
}

void UJPSGA_Run::CancelAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateCancelAbility)
{
	if (ScopeLockCount > 0)
	{
		WaitingToExecute.Add(FPostLockDelegate::CreateUObject(this, &UJPSGA_Run::CancelAbility, Handle, ActorInfo, ActivationInfo, bReplicateCancelAbility));
		return;
	}

	Super::CancelAbility(Handle, ActorInfo, ActivationInfo, bReplicateCancelAbility);
	
	ACharacter* Character = CastChecked<ACharacter>(ActorInfo->AvatarActor.Get());
		
	if (UCharacterMovementComponent* MovementComponent = Character->GetCharacterMovement())
	{
		const UAbilitySystemComponent* ASC = ActorInfo->AbilitySystemComponent.Get();
		if (!ASC) return;

		const UJPSAttributeSetBase* AttributeSet = ASC->GetSet<UJPSAttributeSetBase>();
		if (!AttributeSet) return;
			
		MovementComponent->MaxWalkSpeed = AttributeSet->GetWalkSpeed();
	}
	
}
