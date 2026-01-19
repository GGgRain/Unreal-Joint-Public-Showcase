// Fill out your copyright notice in the Description page of Project Settings.


#include "Misc/JointEyeControlComponent.h"

#include "Misc/JointFacialAnimationComponent.h"
#include "Component/DialogueParticipantComponent.h"
#include "GameFramework/Character.h"
#include "Kismet/KismetMathLibrary.h"

FJointEyeControlLookAtTrackDefinition::FJointEyeControlLookAtTrackDefinition() :
	MaxAge(10.0f),
	Weight(1.0f),
	TargetLocation(FVector::ZeroVector),
	TargetActor(nullptr)
{
}


//FJointEyeControlLookAtTrack::NullTrack

FJointEyeControlLookAtTrack::FJointEyeControlLookAtTrack() : TrackGuid(FGuid::NewGuid())
{
}


void FJointEyeControlLookAtTrack::Start()
{
	CurrentAge = 0.0f;
	CurrentWeight = Definition.Weight;
}

bool FJointEyeControlLookAtTrack::IsEnded() const
{
	return CurrentAge >= Definition.MaxAge;
}

FVector FJointEyeControlLookAtTrack::GetCurrentTargetLookAtLocation() const
{
	if (Definition.TargetActor)
	{
		return Definition.TargetActor->GetActorLocation();
	}
	return Definition.TargetLocation;
}

const FJointEyeControlLookAtTrack FJointEyeControlLookAtTrack::NullTrack = FJointEyeControlLookAtTrack();


// Sets default values for this component's properties
UJointEyeControlComponent::UJointEyeControlComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;


	// ...
}


// Called when the game starts
void UJointEyeControlComponent::BeginPlay()
{
	Super::BeginPlay();

	StartTickTimer();
	StartCollectingLookAtNearbyActors();

	ReserveForUpdatingTargetLookAt(FMath::FRandRange(
		TargetLookAtIntervalRange.GetLowerBoundValue(),
		TargetLookAtIntervalRange.GetUpperBoundValue()
	));
}

void UJointEyeControlComponent::StartTickTimer()
{
	if (const UWorld* World = GetWorld())
	{
		World->GetTimerManager().SetTimer(
			TickTimerHandle,
			this,
			&UJointEyeControlComponent::TickTimerCallback,
			TickInterval,
			true);
	}
}

void UJointEyeControlComponent::StopTickTimer()
{
	if (const UWorld* World = GetWorld())
	{
		World->GetTimerManager().ClearTimer(TickTimerHandle);
	}
}

void UJointEyeControlComponent::TickTimerCallback()
{
	if (!GetWorld()) return;

	const float InTickInterval = GetWorld()->GetTimerManager().GetTimerElapsed(TickTimerHandle);

	UpdateTracks(InTickInterval);

	ApplyEyeControlLookAtTracks();
}

void UJointEyeControlComponent::UpdateTracks(float DeltaTime)
{
	//iterate from the end to allow removal of ended tracks

	for (int32 i = EyeControlLookAtTracks.Num() - 1; i >= 0; --i)
	{
		FJointEyeControlLookAtTrack& EyeControlLookAtTrack = EyeControlLookAtTracks[i];

		EyeControlLookAtTrack.CurrentAge += DeltaTime;

		if (bPrintDebugLogs)
		{
			UE_LOG(LogTemp, Log, TEXT("UJointEyeControlComponent::UpdateTracks - Updating Track for Actor: %s, CurrentAge: %f, MaxAge: %f, CurrentWeight: %f"),
			       * (EyeControlLookAtTrack.Definition.TargetActor ? EyeControlLookAtTrack.Definition.TargetActor->GetName() : TEXT("None")),
			       EyeControlLookAtTrack.CurrentAge,
			       EyeControlLookAtTrack.Definition.MaxAge,
			       EyeControlLookAtTrack.CurrentWeight
			);
		}

		if (EyeControlLookAtTrack.IsEnded())
		{
			OnTrackExpired(EyeControlLookAtTrack);
			EyeControlLookAtTracks.RemoveAt(i);
		}
	}
}

void UJointEyeControlComponent::ApplyEyeControlLookAtTracks()
{
	if (UJointFacialAnimationComponent* FacialAnimationComponent = GetFacialAnimationComponentFromOwner())
	{
		if (FacialAnimationComponent->EyeAnimationDataArray.Num() == 0 || EyeControlLookAtTracks.Num() == 0 || TargetLookAtTracks.IsEmpty()) return;

		FVector TargetLookAtLocation = GetTargetLookAtLocation();

		for (FJointEyeAnimationData& EyeAnimationData : FacialAnimationComponent->EyeAnimationDataArray)
		{
			EyeAnimationData.EyeGazeData.bUseManualTargetEyeGazeRotation = true;
			EyeAnimationData.TargetEyeGazeRotation = UKismetMathLibrary::FindRelativeLookAtRotation(
				GetOwner()->GetActorTransform(),
				TargetLookAtLocation
			);
		}
	}
}

void UJointEyeControlComponent::OnTrackExpired(FJointEyeControlLookAtTrack& EndedTrack)
{
	if (EyeControlLookAtTracks.IsEmpty())
	{
		if (UJointFacialAnimationComponent* FacialAnimationComponent = GetFacialAnimationComponentFromOwner())
		{
			for (FJointEyeAnimationData& EyeAnimationData : FacialAnimationComponent->EyeAnimationDataArray)
			{
				EyeAnimationData.EyeGazeData.bUseManualTargetEyeGazeRotation = false;
			}
		}
	}

	// immediately update the target look at to remove the expired track
	UpdateTargetLookAt();
}

void UJointEyeControlComponent::StartCollectingLookAtNearbyActors()
{
	if (const UWorld* World = GetWorld())
	{
		World->GetTimerManager().SetTimer(
			NearbyActorsTraceTimerHandle,
			this,
			&UJointEyeControlComponent::CollectLookAtNearbyActors,
			NearbyActorsTraceTickInterval,
			true);
	}
}

void UJointEyeControlComponent::StopCollectingLookAtNearbyActors()
{
	if (const UWorld* World = GetWorld())
	{
		World->GetTimerManager().ClearTimer(NearbyActorsTraceTimerHandle);
	}
}

bool UJointEyeControlComponent::CheckIfCanLookAtActor(AActor* OtherActor) const
{
	// by default, can look at any actor

	if (OtherActor)
	{
		// don't look at self
		if (OtherActor == GetOwner())
		{
			return false;
		}

		// only look at actors with DialogueParticipantComponent
		if (!OtherActor->GetComponentByClass(UDialogueParticipantComponent::StaticClass()))
		{
			return false;
		}
	}

	return true;
}

void UJointEyeControlComponent::CollectLookAtNearbyActors()
{
	// do a sphere trace to find nearby actors to look at
	 
	if (!bEnableLookAtNearbyActors) return;

	if (!GetWorld()) return;
	TArray<FOverlapResult> OverlapResults;

	GetWorld()->OverlapMultiByChannel(
		OverlapResults,
		GetOwner()->GetActorLocation(),
		FQuat::Identity,
		ECC_Pawn,
		FCollisionShape::MakeSphere(NearbyActorsTraceRadius)
	);
	bool bAddedNewTrack = false;

	float DeltaTime = GetWorld()->GetTimerManager().GetTimerElapsed(NearbyActorsTraceTimerHandle);

	for (const FOverlapResult& OverlapResult : OverlapResults)
	{
		if (AActor* OverlappedActor = OverlapResult.GetActor())
		{
			// check if we can look at this actor
			if (!CheckIfCanLookAtActor(OverlappedActor)) continue;

			// see if we already have a look at track for this actor

			if (FJointEyeControlLookAtTrack* FoundTrack = FindEyeControlLookAtTrackForActor_Ptr(OverlappedActor); FoundTrack != nullptr)
			{
				// reset the age of the existing track

				//UE_LOG(LogTemp, Log, TEXT("UJointEyeControlComponent::CollectLookAtNearbyActors - Resetting age of existing track for actor: %s, deltatime: %f"), *OverlappedActor->GetName(), DeltaTime);
				FoundTrack->CurrentAge = 0.0f;

				if (bUseLinearVelocityForWeighting)
				{
					const float DistanceWeight = FMath::Clamp(
						1.0f - (FVector::Dist(GetOwner()->GetActorLocation(), OverlappedActor->GetActorLocation()) / NearbyActorsTraceRadius),
						0.0f,
						1.0f
					);
					
					const float InterpTarget =
						OverlappedActor->GetVelocity().Size() * NearbyActorsVelocityWeightMultiplier 
						+ NearbyActorsDistanceWeightMultiplier * DistanceWeight 
						+ NearbyActorsBaseWeight;

					// use interpolation when lowering weight
					if (FoundTrack->CurrentWeight < InterpTarget)
					{
						FoundTrack->CurrentWeight = FMath::FInterpTo(
							FoundTrack->CurrentWeight,
							InterpTarget,
							DeltaTime,
							18.0f // TODO: interpolation speed
						);
					}
					else
					{
						FoundTrack->CurrentWeight = InterpTarget;
					}
				}
			}
			else
			{
				bAddedNewTrack = true;
				// add a new look at track for this actor
				FJointEyeControlLookAtTrackDefinition NewTrackDefinition;
				NewTrackDefinition.TargetActor = OverlappedActor;
				NewTrackDefinition.MaxAge = 2.5f; // look at for 2 seconds

				AddEyeControlLookAtTrackForDefinition(NAME_None, NewTrackDefinition);
			}
		}
	}

	if (bAddedNewTrack)
	{
		// immediately update the target look at to include the new track
		UpdateTargetLookAt();
	}
}

UJointFacialAnimationComponent* UJointEyeControlComponent::GetFacialAnimationComponentFromOwner() const
{
	if (AActor* Owner = GetOwner())
	{
		if (ACharacter* CharacterOwner = Cast<ACharacter>(Owner))
		{
			return CharacterOwner->FindComponentByClass<UJointFacialAnimationComponent>();
		}
	}

	return nullptr;
}

FJointEyeControlLookAtTrack UJointEyeControlComponent::FindEyeControlLookAtTrackForActor(AActor* TargetActor)
{
	for (FJointEyeControlLookAtTrack& EyeControlLookAtTrack : EyeControlLookAtTracks)
	{
		if (EyeControlLookAtTrack.Definition.TargetActor == TargetActor)
		{
			return EyeControlLookAtTrack;
		}
	}

	return FJointEyeControlLookAtTrack::NullTrack;
}

FJointEyeControlLookAtTrack UJointEyeControlComponent::FindEyeControlLookAtTrackForGuid(const FGuid& Guid)
{
	for (FJointEyeControlLookAtTrack& EyeControlLookAtTrack : EyeControlLookAtTracks)
	{
		if (EyeControlLookAtTrack.GetTrackGuid() == Guid)
		{
			return EyeControlLookAtTrack;
		}
	}

	return FJointEyeControlLookAtTrack::NullTrack;
}

FJointEyeControlLookAtTrack UJointEyeControlComponent::FindEyeControlLookAtTrackForName(const FName& TrackName)
{
	for (FJointEyeControlLookAtTrack& EyeControlLookAtTrack : EyeControlLookAtTracks)
	{
		if (EyeControlLookAtTrack.GetTrackName() == TrackName)
		{
			return EyeControlLookAtTrack;
		}
	}

	return FJointEyeControlLookAtTrack::NullTrack;
}

FJointEyeControlLookAtTrack* UJointEyeControlComponent::FindEyeControlLookAtTrackForActor_Ptr(AActor* TargetActor)
{
	for (FJointEyeControlLookAtTrack& EyeControlLookAtTrack : EyeControlLookAtTracks)
	{
		if (EyeControlLookAtTrack.Definition.TargetActor == TargetActor)
		{
			return &EyeControlLookAtTrack;
		}
	}

	return nullptr;
}

FJointEyeControlLookAtTrack* UJointEyeControlComponent::FindEyeControlLookAtTrackForGuid_Ptr(const FGuid& Guid)
{
	for (FJointEyeControlLookAtTrack& EyeControlLookAtTrack : EyeControlLookAtTracks)
	{
		if (EyeControlLookAtTrack.GetTrackGuid() == Guid)
		{
			return &EyeControlLookAtTrack;
		}
	}

	return nullptr;
}

FJointEyeControlLookAtTrack* UJointEyeControlComponent::FindEyeControlLookAtTrackForName_Ptr(const FName& TrackName)
{
	for (FJointEyeControlLookAtTrack& EyeControlLookAtTrack : EyeControlLookAtTracks)
	{
		if (EyeControlLookAtTrack.GetTrackName() == TrackName)
		{
			return &EyeControlLookAtTrack;
		}
	}

	return nullptr;
}


FGuid UJointEyeControlComponent::AddEyeControlLookAtTrackForDefinition(const FName& TrackName, const FJointEyeControlLookAtTrackDefinition& TrackDefinition)
{
	FJointEyeControlLookAtTrack NewLookAtTrack;
	NewLookAtTrack.TrackName = TrackName;
	NewLookAtTrack.Definition = TrackDefinition;
	NewLookAtTrack.Start();
	
	EyeControlLookAtTracks.Add(NewLookAtTrack);

	// immediately update the target look at to include the new track
	UpdateTargetLookAt();
	
	return NewLookAtTrack.GetTrackGuid();
}

bool UJointEyeControlComponent::RemoveEyeControlLookAtTrackForGuid(const FGuid& Guid, bool bUpdate)
{
	// iterate from the end to allow removal
	for (int32 i = EyeControlLookAtTracks.Num() - 1; i >= 0; --i)
	{
		FJointEyeControlLookAtTrack& EyeControlLookAtTrack = EyeControlLookAtTracks[i];
		if (EyeControlLookAtTrack.GetTrackGuid() == Guid)
		{
			EyeControlLookAtTracks.RemoveAt(i);

			if (bUpdate)
			{
				// immediately update the target look at to remove the track
				UpdateTargetLookAt();
			}
			
			return true;
		}
	}
	return false;
}

bool UJointEyeControlComponent::RemoveEyeControlLookAtTrackForName(const FName& TrackName, bool bUpdate)
{
	// iterate from the end to allow removal
	for (int32 i = EyeControlLookAtTracks.Num() - 1; i >= 0; --i)
	{
		FJointEyeControlLookAtTrack& EyeControlLookAtTrack = EyeControlLookAtTracks[i];
		if (EyeControlLookAtTrack.GetTrackName() == TrackName)
		{
			EyeControlLookAtTracks.RemoveAt(i);

			if (bUpdate)
			{
				// immediately update the target look at to remove the track
				UpdateTargetLookAt();
			}
			
			return true;
		}
	}
	
	return false;
}

void UJointEyeControlComponent::OverrideJointEyeControlLookAtTrack(const FJointEyeControlLookAtTrack& NewTrack)
{
	if (FJointEyeControlLookAtTrack* FoundTrack = FindEyeControlLookAtTrackForGuid_Ptr(NewTrack.GetTrackGuid()))
	{
		*FoundTrack = NewTrack;
	}
}

FVector UJointEyeControlComponent::GetTargetLookAtLocation() const
{
	// iterate indices in TargetLookAtTracks to compute the final look at location
	if (TargetLookAtTracks.Num() > 0)
	{
		FVector AccumulatedLocation = FVector::ZeroVector;
		float TotalWeight = 0.0f;

		for (const int TrackIndex : TargetLookAtTracks)
		{
			// We must ensure the TrackIndex is valid before accessing the EyeControlLookAtTracks array since it can change dynamically.
			if (EyeControlLookAtTracks.IsValidIndex(TrackIndex))
			{
				const FJointEyeControlLookAtTrack& LookAtTrack = EyeControlLookAtTracks[TrackIndex];
				const FVector TrackLocation = LookAtTrack.GetCurrentTargetLookAtLocation();
				const float TrackWeight = LookAtTrack.CurrentWeight;

				AccumulatedLocation += TrackLocation * TrackWeight;
				TotalWeight += TrackWeight;
			}
		}

		if (TotalWeight > KINDA_SMALL_NUMBER)
		{
			//UE_LOG(LogTemp, Log, TEXT("UJointEyeControlComponent::GetTargetLookAtLocation - Computed Target Look At Location: %s"), *(AccumulatedLocation / TotalWeight).ToString());
			return AccumulatedLocation / TotalWeight;
		}
	}

	return FVector::ZeroVector;
}

void UJointEyeControlComponent::ReserveForUpdatingTargetLookAt(float UpdateInterval, bool bForce)
{
	if (bPrintDebugLogs)
	{
		UE_LOG(LogTemp, Warning, TEXT("UJointEyeControlComponent::ReserveForUpdatingTargetLookAt; - Reserving Update in %f seconds (bForce=%s)"),
		       UpdateInterval,
		       bForce ? TEXT("true") : TEXT("false")
		);
	}
	if (TargetLookAtUpdateTimerHandle.IsValid())
	{
		if (bForce)
		{
			CancelReservationForUpdatingTargetLookAt();
		}
		else
		{
			return;
		}
	}

	if (const UWorld* World = GetWorld())
	{
		World->GetTimerManager().SetTimer(
			TargetLookAtUpdateTimerHandle,
			this,
			&UJointEyeControlComponent::UpdateTargetLookAt,
			UpdateInterval,
			false);
	}
}

void UJointEyeControlComponent::CancelReservationForUpdatingTargetLookAt()
{
	if (const UWorld* World = GetWorld())
	{
		World->GetTimerManager().ClearTimer(TargetLookAtUpdateTimerHandle);
	}
}

void UJointEyeControlComponent::UpdateTargetLookAt()
{
	TargetLookAtTracks.Empty();

	const int32 TrackCount = EyeControlLookAtTracks.Num();
	
	if (TrackCount != 0)
	{
		int32 WeightSum = 0;
		int32 HighestWeightIndex = INDEX_NONE;

		// Calculate weight sum and find highest-weight track index
		for (int32 i = 0; i < TrackCount; ++i)
		{
			const int32 Weight = EyeControlLookAtTracks[i].CurrentWeight;
			WeightSum += Weight;

			if (HighestWeightIndex == INDEX_NONE ||
				Weight > EyeControlLookAtTracks[HighestWeightIndex].CurrentWeight)
			{
				HighestWeightIndex = i;
			}
		}

		// Weighted random selection (store indices)
		for (int32 i = 0; i < TrackCount; ++i)
		{
			const float NormalizedWeight =
				(WeightSum > 0)
					? static_cast<float>(EyeControlLookAtTracks[i].CurrentWeight) / WeightSum
					: 0.0f;

			if (FMath::FRand() <= NormalizedWeight)
			{
				TargetLookAtTracks.Add(i);
			}
		}

		// Fallback: ensure at least one track is selected
		if (TargetLookAtTracks.IsEmpty() && HighestWeightIndex != INDEX_NONE)
		{
			TargetLookAtTracks.Add(HighestWeightIndex);
		}

		if (bPrintDebugLogs)
		{
			UE_LOG(LogTemp, Warning, TEXT("UJointEyeControlComponent::UpdateTargetLookAt; - Updated Target Look At Tracks: %s"),
			       *FString::JoinBy(TargetLookAtTracks, TEXT(", "), [](int32 Index) {
				       return FString::FromInt(Index);
				       })
			);
		}
	}

	ReserveForUpdatingTargetLookAt(FMath::FRandRange(
		                               TargetLookAtIntervalRange.GetLowerBoundValue(),
		                               TargetLookAtIntervalRange.GetUpperBoundValue()
	                               ), true);
}
