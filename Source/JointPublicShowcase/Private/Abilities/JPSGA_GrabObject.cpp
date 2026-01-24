// Fill out your copyright notice in the Description page of Project Settings.


#include "Abilities/JPSGA_GrabObject.h"

#include "JPSCharacter.h"
#include "Abilities/AbilityTask_GrabObjectHold.h"
#include "Abilities/AttributeSets/JPSAttributeSetBase.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Net/UnrealNetwork.h"

UE_DEFINE_GAMEPLAY_TAG(TAG_Cue_JPS_GrabObject, "GameplayCue.JPS.GrabObject");


UJPSGA_GrabObject::UJPSGA_GrabObject(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::LocalPredicted;
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	ReplicationPolicy = EGameplayAbilityReplicationPolicy::ReplicateYes;
}

FBodyInstance* UJPSGA_GrabObject::GetGrabbedBI() const
{
	if (GrabbedComponent.IsValid())
	{
		return GrabbedComponent->GetBodyInstance(GrabbedComponentSocketName);
	}
	
	return nullptr;
}

FVector UJPSGA_GrabObject::GetGrabbedBIWorldLocation() const
{
	if (FBodyInstance* BI = GetGrabbedBI())
	{
		return BI->GetUnrealWorldTransform().GetLocation();
	}
	
	// fallback to avatar hand location
	return GetAvatarHandWorldLocation();
}

FVector UJPSGA_GrabObject::GetAvatarHandWorldLocation(const FName HandBoneName) const
{
	const FGameplayAbilityActorInfo* ActorInfo = GetCurrentActorInfo();
	ACharacter* Character = Cast<ACharacter>(ActorInfo->AvatarActor.Get());
	if (!Character) return FVector::ZeroVector;

	if (USkeletalMeshComponent* MeshComp = Character->GetMesh())
	{
		return MeshComp->GetBoneLocation(HandBoneName);
	}

	return FVector::ZeroVector;
}

void UJPSGA_GrabObject::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	DOREPLIFETIME(UJPSGA_GrabObject, GrabbedComponent);
	DOREPLIFETIME(UJPSGA_GrabObject, GrabbedComponentSocketName);
}

bool UJPSGA_GrabObject::CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags, const FGameplayTagContainer* TargetTags,
                                           FGameplayTagContainer* OptionalRelevantTags) const
{
	if (!Super::CanActivateAbility(Handle, ActorInfo, SourceTags, TargetTags, OptionalRelevantTags))
	{
		return false;
	}

	const ACharacter* Character = CastChecked<ACharacter>(ActorInfo->AvatarActor.Get(), ECastCheckedType::NullAllowed);
	return Character != nullptr && !GrabbedComponent.IsValid(); // can only grab if we don't already have something grabbed
}

void UJPSGA_GrabObject::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	if (!HasAuthorityOrPredictionKey(ActorInfo, &ActivationInfo)) return;

	if (!CommitAbility(Handle, ActorInfo, ActivationInfo)) return;
	
	// trace from the camera to find a physics object to grab
	
	AJPSCharacter* Character = CastChecked<AJPSCharacter>(ActorInfo->AvatarActor.Get());
	
	if (UCameraComponent* Camera = Character->GetFollowCamera())
	{
		FVector Start = Camera->GetComponentLocation();
		FVector End = Start + (Camera->GetForwardVector() * 500.0f);
		
		// Perform a shape trace to find a physics object using "BlockAllDynamic",
		TArray<FHitResult> HitResults;
		FCollisionQueryParams Params;
		Params.AddIgnoredActor(Character);
		
		GetWorld()->SweepMultiByProfile(
			HitResults,
			Start,
			End,
			FQuat::Identity,
			"OverlapAllNoPawn",
			FCollisionShape::MakeSphere(50.0f),
			Params
		);
		

		for (const FHitResult& Hit : HitResults)
		{
			if (AActor* HitActor = Hit.GetActor())
			{
				if (USkeletalMeshComponent* SkelMesh = Cast<USkeletalMeshComponent>(Hit.Component))
				{
					if ( SkelMesh && Hit.BoneName == NAME_None)
					{
						//if we grabbed the skeletal mesh but no bone was hit, release the grab since we don't want to grab the whole mesh
						break;
					}
					
					// if the root component is a skeletal mesh
					GrabbedComponent = SkelMesh;
					GrabbedComponentSocketName = Hit.BoneName; // store the bone name to attach to
					
					break;
					
				}else if (UPrimitiveComponent* PrimComp = Cast<UPrimitiveComponent>(Hit.Component))
				{
					if (PrimComp->IsSimulatingPhysics())
					{
						// Found a physics object to grab
						GrabbedComponent = PrimComp;
						GrabbedComponentSocketName = NAME_None;
						break;
					}
				}
			}
		}
	}
	
	
	
	if (GrabbedComponent.Get())
	{
		// only do this on the authority side
		if (ActorInfo->IsNetAuthority())
		{
			if (AActor* InOwner = GrabbedComponent->GetOwner())
			{
				bSavedGrabbedObjectbReplicates = GrabbedComponent->GetIsReplicated(); // Cached
							
				// Make sure the actor is replicated so it can be seen by other clients
				InOwner->SetReplicates(true);
				InOwner->SetReplicateMovement(true);
							
				//GrabbedComponent->SetAllUseCCD(true); // Enable CCD for better physics interaction while being grabbed
			}
		}
		
		// Gameplay Cue
		if (UAbilitySystemComponent* ASC = ActorInfo->AbilitySystemComponent.Get())
		{
			FGameplayCueParameters Params;
			Params.SourceObject = this;
			Params.Instigator = Character;
			Params.TargetAttachComponent = Character->GetMesh();
			ASC->AddGameplayCue(TAG_Cue_JPS_GrabObject, Params);
		}

		UAbilityTask_GrabObjectHold* HoldTask =
			UAbilityTask_GrabObjectHold::GrabObjectHold(
				this,
				GrabbedComponent.Get(),
				GrabbedComponentSocketName
			);

		HoldTask->ReadyForActivation();
	}
	else
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
	}
}

void UJPSGA_GrabObject::InputReleased(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo)
{
	if (ActorInfo != NULL && ActorInfo->AvatarActor != NULL)
	{
		CancelAbility(Handle, ActorInfo, ActivationInfo, true);
	}
}

void UJPSGA_GrabObject::CancelAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateCancelAbility)
{
	if (ScopeLockCount > 0)
	{
		WaitingToExecute.Add(FPostLockDelegate::CreateUObject(this, &UJPSGA_GrabObject::CancelAbility, Handle, ActorInfo, ActivationInfo, bReplicateCancelAbility));
		return;
	}

	Super::CancelAbility(Handle, ActorInfo, ActivationInfo, bReplicateCancelAbility);
	
	//get ASC and trigger gameplay cue
	if (UAbilitySystemComponent* ASC = ActorInfo->AbilitySystemComponent.Get())
	{
		ASC->RemoveGameplayCue(TAG_Cue_JPS_GrabObject);
	}
	
	if (GrabbedComponent.IsValid())
	{
		//GrabbedComponent->SetAllUseCCD(false);
		
		if (AActor* InOwner = GrabbedComponent->GetOwner())
		{
			InOwner->SetReplicates(bSavedGrabbedObjectbReplicates);
		}
	}
	
	GrabbedComponent = nullptr;
	GrabbedComponentSocketName = NAME_None;
	
}
