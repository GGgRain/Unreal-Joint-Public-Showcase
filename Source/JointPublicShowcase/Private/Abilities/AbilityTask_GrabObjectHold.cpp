#include "Abilities/AbilityTask_GrabObjectHold.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/Character.h"
#include "AbilitySystemComponent.h"
#include "GameFramework/Actor.h"
#include "PhysicsEngine/BodyInstance.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/PrimitiveComponent.h"

UAbilityTask_GrabObjectHold* UAbilityTask_GrabObjectHold::GrabObjectHold(
	UGameplayAbility* OwningAbility,
	UPrimitiveComponent* InGrabbedComponent,
	FName InGrabbedComponentSocketName,
	float InDistance,
	float InForceScale)
{
	UAbilityTask_GrabObjectHold* Task =
		NewAbilityTask<UAbilityTask_GrabObjectHold>(OwningAbility);

	Task->GrabbedComponent = InGrabbedComponent;
	Task->GrabbedComponentSocketName = InGrabbedComponentSocketName;
	Task->Distance = InDistance;
	Task->ForceScale = InForceScale;
	Task->bTickingTask = true;

	return Task;
}

void UAbilityTask_GrabObjectHold::Activate()
{
	if (!GrabbedComponent.IsValid())
	{
		EndTask();
	}
}

void UAbilityTask_GrabObjectHold::TickTask(float DeltaTime)
{
	if (!GrabbedComponent.IsValid())
	{
		EndTask();
		return;
	}

	const FGameplayAbilityActorInfo* ActorInfo = Ability->GetCurrentActorInfo();
	ACharacter* Character = Cast<ACharacter>(ActorInfo->AvatarActor.Get());
	if (!Character) return;

	UCameraComponent* Camera = Character->FindComponentByClass<UCameraComponent>();
	if (!Camera) return;

	FVector TargetLocation = Camera->GetComponentLocation() + Camera->GetForwardVector() * Distance;

	/*
	if (FBodyInstance* BI = GetGrabbedBodyInstance())
	{
		//TODO: Not sure whether it will be replicated. -> probably not.
		FVector Dir = TargetLocation - BI->GetUnrealWorldTransform().GetLocation();
		
		BI->WakeInstance();
		
		BI->SetLinearVelocity(Dir * ForceScale, false);
		BI->SetAngularVelocityInRadians(FVector::ZeroVector, false);
	}
	*/
	
	if (USkeletalMeshComponent* SkeletalMeshComponent =  Cast<USkeletalMeshComponent>(GrabbedComponent.Get()))
	{
		FVector Dir = TargetLocation - SkeletalMeshComponent->GetSocketLocation(GrabbedComponentSocketName);
		Dir *= ForceScale;

		SkeletalMeshComponent->SetPhysicsLinearVelocity(Dir, false, GrabbedComponentSocketName);
		SkeletalMeshComponent->SetPhysicsAngularVelocityInDegrees(Dir, false, GrabbedComponentSocketName);
		SkeletalMeshComponent->WakeRigidBody(GrabbedComponentSocketName);
	}else
	{
		FVector Dir = TargetLocation - GrabbedComponent->GetComponentLocation();
		Dir *= ForceScale;

		GrabbedComponent->SetPhysicsLinearVelocity(Dir, false, GrabbedComponentSocketName);
		GrabbedComponent->SetPhysicsAngularVelocityInDegrees(Dir, false, GrabbedComponentSocketName);
		GrabbedComponent->WakeAllRigidBodies();
	}
}

void UAbilityTask_GrabObjectHold::OnDestroy(bool bInOwnerFinished)
{
	if (GrabbedComponent.IsValid())
	{
		GrabbedComponent->SetAllUseCCD(false);
	}
	
	Super::OnDestroy(bInOwnerFinished);
}

FBodyInstance* UAbilityTask_GrabObjectHold::GetGrabbedBodyInstance() const
{
	if (GrabbedComponent.IsValid()) return GrabbedComponent->GetBodyInstance(GrabbedComponentSocketName);
	
	return nullptr;
}
