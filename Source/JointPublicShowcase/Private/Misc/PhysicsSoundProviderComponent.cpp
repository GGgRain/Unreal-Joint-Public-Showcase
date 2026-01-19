// Fill out your copyright notice in the Description page of Project Settings.


#include "Misc/PhysicsSoundProviderComponent.h"
#include "PhysicalMaterials/PhysicalMaterial.h"
#include "Sound/SoundBase.h"
#include "Kismet/GameplayStatics.h"

UPhysicsSoundProviderComponent::UPhysicsSoundProviderComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UPhysicsSoundProviderComponent::BeginPlay()
{
	Super::BeginPlay();
	
	if (bActivateOnBeginPlay)
	{
		ActivatePhysicsSoundProvider();
	}
}

void UPhysicsSoundProviderComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	// Ensure we clean up things
	DeactivatePhysicsSoundProvider();
	
	Super::EndPlay(EndPlayReason);
}

void UPhysicsSoundProviderComponent::ActivatePhysicsSoundProvider()
{
	if (UPrimitiveComponent* TargetPrimitiveComponent = FindTargetPrimitiveComponent())
	{
		if (bChangeBodySetupToGenerateHitEventOnActivate)
		{
			bSavedNotifyRigidBodyCollision = TargetPrimitiveComponent->BodyInstance.bNotifyRigidBodyCollision;
			TargetPrimitiveComponent->SetNotifyRigidBodyCollision(true);
		}
			
		BindHitEventToPrimitiveComponent(TargetPrimitiveComponent);
	}
}

void UPhysicsSoundProviderComponent::DeactivatePhysicsSoundProvider()
{
	if (UPrimitiveComponent* TargetPrimitiveComponent = FindTargetPrimitiveComponent())
	{
		UnbindHitEventFromPrimitiveComponent(TargetPrimitiveComponent);
		
		if (bRestoreBodySetupOnDeactivate)
		{
			TargetPrimitiveComponent->SetNotifyRigidBodyCollision(bSavedNotifyRigidBodyCollision);
		}
	}
}

UPrimitiveComponent* UPhysicsSoundProviderComponent::FindTargetPrimitiveComponent() const
{
	AActor* Owner = GetOwner();
	if (!Owner) return nullptr;

	TArray<UPrimitiveComponent*> PrimitiveComponents;
	Owner->GetComponents<UPrimitiveComponent>(PrimitiveComponents);
	
	if (TargetPrimitiveComponentName.IsNone())
	{
		// if no name specified, use the root component if it's a primitive component
		if (UPrimitiveComponent* RootPrimComp = Cast<UPrimitiveComponent>(Owner->GetRootComponent()))
		{
			return RootPrimComp;
		}
	}

	for (UPrimitiveComponent* PrimComp : PrimitiveComponents)
	{
		if (PrimComp && PrimComp->GetFName() == TargetPrimitiveComponentName)
		{
			return PrimComp;
		}
	}

	return nullptr;
}

void UPhysicsSoundProviderComponent::BindHitEventToPrimitiveComponent(UPrimitiveComponent* TargetPrimitiveComponent)
{
	if (!TargetPrimitiveComponent) return;
	
	TargetPrimitiveComponent->OnComponentHit.AddDynamic(this, &UPhysicsSoundProviderComponent::OnTargetComponentHit);
	
}

void UPhysicsSoundProviderComponent::UnbindHitEventFromPrimitiveComponent(UPrimitiveComponent* TargetPrimitiveComponent)
{
	if (!TargetPrimitiveComponent) return;
	
	TargetPrimitiveComponent->OnComponentHit.RemoveDynamic(this, &UPhysicsSoundProviderComponent::OnTargetComponentHit);
}

void UPhysicsSoundProviderComponent::OnTargetComponentHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	//UE_LOG(LogTemp, Log, TEXT("PhysicsSoundProviderComponent: OnTargetComponentHit called, NormalImpulse = %s"), *NormalImpulse.ToString());
	HandlePhysicsHitEvent(HitComponent, OtherActor, OtherComp, NormalImpulse, Hit);
}

void UPhysicsSoundProviderComponent::HandlePhysicsHitEvent_Implementation(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	// Get BodyInstance of the HitComponent (at the hit bone if skeletal mesh)
	
	FBodyInstance* MyBodyInstance = nullptr;
	
	if (Cast<USkeletalMeshComponent>(HitComponent))
	{
		if (USkeletalMeshComponent* SkeletalMeshComp = Cast<USkeletalMeshComponent>(HitComponent))
		{
			if (SkeletalMeshComp->GetBodyInstance(Hit.MyBoneName))
			{
				MyBodyInstance = SkeletalMeshComp->GetBodyInstance(Hit.MyBoneName);
			}
		}
	}else
	{
		MyBodyInstance = HitComponent->GetBodyInstance();
	}
	
	// Get the BodyInstance of the OtherComp (at the hit bone if skeletal mesh)
	FBodyInstance* OtherBodyInstance = nullptr;
	
	if (OtherComp)
	{
		if (Cast<USkeletalMeshComponent>(OtherComp))
		{
			if (USkeletalMeshComponent* OtherSkeletalMeshComp = Cast<USkeletalMeshComponent>(OtherComp))
			{
				if (OtherSkeletalMeshComp->GetBodyInstance(Hit.BoneName))
				{
					OtherBodyInstance = OtherSkeletalMeshComp->GetBodyInstance(Hit.BoneName);
				}
			}
		}else
		{
			OtherBodyInstance = OtherComp->GetBodyInstance();
		}
	}

	// Early out if no valid BodyInstances
	if (!MyBodyInstance || !OtherBodyInstance) return;

	// 1. Direct hit sound - 상대속도를 구한 뒤, 임펙트 노말 방향으로 프로젝션하여 임팩트 강도를 산출

	const FVector MyVelocity = MyBodyInstance->GetUnrealWorldVelocity();
	const FVector OtherVelocity = OtherBodyInstance->GetUnrealWorldVelocity();
	const FVector RelativeVelocity = MyVelocity - OtherVelocity;
		
	float ImpactStrength = FVector::DotProduct(RelativeVelocity, Hit.ImpactNormal);
	ImpactStrength = FMath::Abs(ImpactStrength);
	
	if ( ImpactStrength >= DirectHitImpulseThreshold)
	{
		PlayPhysicsSoundFromStructureArray(
			DirectHitSounds,
			Hit.ImpactPoint,
			MyBodyInstance->GetBodyMass(),
			ImpactStrength,
			DirectHitVolumeMaxImpulseThreshold,
			DirectHitVolumeMultiplier,
			MyBodyInstance->GetSimplePhysicalMaterial(),
			Hit.PhysMaterial);	
		
		return; // we only play one by one.
	}
	
	const FVector MyAngularVelocity = MyBodyInstance->GetUnrealWorldAngularVelocityInRadians();
	const FVector OtherAngularVelocity = OtherBodyInstance->GetUnrealWorldAngularVelocityInRadians();
	const FVector RelativeAngularVelocity = MyAngularVelocity - OtherAngularVelocity;

	//각속도 백터는 Pitch, Yaw, Roll 축에 대한 회전 속도를 나타내므로 바로 사용할 수 없음.
	//구르는 속도를 구하려면 접촉면에 수직인 축을 기준으로 한 각속도 성분(스핀 성분)을 제거해야 함.
	const FVector ContactPointRadiusVector = MyBodyInstance->GetUnrealWorldTransform().GetLocation() - Hit.ImpactPoint;
	const FVector RollingVelocity = FVector::CrossProduct(RelativeAngularVelocity, ContactPointRadiusVector);
	const float RollingSpeed = RollingVelocity.Size(); 
	

	if (RollingSpeed >= RollingAngularVelocityThreshold)
	{
		PlayPhysicsSoundFromStructureArray(
			RollingHitSound,
			Hit.ImpactPoint,
			MyBodyInstance->GetBodyMass(),
			RollingSpeed,
			RollingVolumeMaxImpulseThreshold,
			RollingVolumeMultiplier,
			MyBodyInstance->GetSimplePhysicalMaterial(),
			Hit.PhysMaterial);
	}
	
	// 2. Sliding hit sound - 접촉면을 따라 움직이는 속도를 구하여 슬라이딩 속도를 산출
	const FVector SlidingVelocity = RelativeVelocity - (FVector::DotProduct(RelativeVelocity, Hit.ImpactNormal));
	const float SlidingSpeed = SlidingVelocity.Size();
	if (SlidingSpeed >= SlidingSpeedThreshold)
	{
		PlayPhysicsSoundFromStructureArray(
			SlidingHitSound,
			Hit.ImpactPoint,
			MyBodyInstance->GetBodyMass(),
			SlidingSpeed,
			SlidingVolumeMaxImpulseThreshold,
			SlidingVolumeMultiplier,
			MyBodyInstance->GetSimplePhysicalMaterial(),
			Hit.PhysMaterial);
		
	}
	
	
}

void UPhysicsSoundProviderComponent::PlayPhysicsSoundFromStructureArray(
	const TArray<FPhysicsSoundProviderStructure>& InArray,
	const FVector_NetQuantize& Location,
	float BodyMass,
	float ImpactStrength,
	float SoundMaxVolumeImpulseThreshold,
	float SoundMaxVolumeMultiplier,
	UPhysicalMaterial* SelfPhysicalMaterial,
	TWeakObjectPtr<class UPhysicalMaterial> HitPhysicalMaterial)
{
	if (InArray.Num() == 0) return;
	
	// find appropriate sound struct based on ImpulseThreshold.
	const FPhysicsSoundProviderStructure* SoundStruct = nullptr;
	
	// Assuming InArray is sorted by ImpulseThreshold ascendingly. (We ensure this in editor via PostEditChangeProperty)
	for (int32 i = 0; i < InArray.Num(); ++i)
	{
		// if no sound struct selected yet, select the first one as default (smallest ImpulseThreshold)
		if ( !SoundStruct )
		{
			SoundStruct = &InArray[i];
		}
		
		if (ImpactStrength >= InArray[i].ImpulseThreshold)
		{
			SoundStruct = &InArray[i];
			break;
		}
	}
	
	// return if no sound struct found
	if (!SoundStruct) return;
	
	float VolumeMultiplier = 1.0f;
	// calculate volume multiplier based on ImpactStrength and SoundMaxVolumeImpulseThreshold
	VolumeMultiplier = FMath::Clamp(ImpactStrength / SoundMaxVolumeImpulseThreshold, 0.0f, 1.0f) * SoundMaxVolumeMultiplier;
	VolumeMultiplier = VolumeMultiplier * FMath::Clamp<float>(BodyMass / MassVolumeMaxImpulseThreshold, MassVolumeBound.GetLowerBound().GetValue(), MassVolumeBound.GetUpperBound().GetValue());
	
	float PitchMultiplier = 1.0f;
	PitchMultiplier = FMath::Clamp<float>(BodyMass / MassVolumeMaxImpulseThreshold, MassPitchBound.GetLowerBound().GetValue(), MassPitchBound.GetUpperBound().GetValue());
	
	const FPhysicsSoundProviderStructureIndex IndexKey(
		SelfPhysicalMaterial,
		HitPhysicalMaterial.Get()
	);
	
	UGameplayStatics::PlaySoundAtLocation(
				this,
				SoundStruct->FindSoundForPhysicalMaterials(IndexKey),
				Location, 
				VolumeMultiplier,
				PitchMultiplier,
				0.0f,
				MasterAttenuation.LoadSynchronous()
			);
	
	
}

#if WITH_EDITOR

void UPhysicsSoundProviderComponent::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);
	
	SortSoundProviderStructuresByImpulseThreshold(DirectHitSounds);
	SortSoundProviderStructuresByImpulseThreshold(SlidingHitSound);
	SortSoundProviderStructuresByImpulseThreshold(RollingHitSound);
}


void UPhysicsSoundProviderComponent::SortSoundProviderStructuresByImpulseThreshold(TArray<FPhysicsSoundProviderStructure>& Array)
{
	Algo::SortBy(Array, &FPhysicsSoundProviderStructure::ImpulseThreshold, [](float A, float B) { return A < B; });
}

#endif
