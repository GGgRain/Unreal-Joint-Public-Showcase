// Fill out your copyright notice in the Description page of Project Settings.


#include "Misc/JointFacialAnimationComponent.h"
#include "Animation/AnimInstance.h"
#include "Curves/CurveFloat.h"
#include "Engine/World.h"
#include "TimerManager.h"
#include "Components/SkeletalMeshComponent.h"
#include "GameFramework/Character.h"

void IJointABPFacialAnimationInterface::SetMorphTargetWeight_Implementation(const FName& MorphTargetName, const float Weight)
{
	UE_LOG(LogTemp, Warning, TEXT("SetMorphTargetWeight called for Morph Target: %s with Weight: %f"), *MorphTargetName.ToString(), Weight);
	return;
}

float IJointABPFacialAnimationInterface::GetMorphTargetWeight_Implementation(const FName& MorphTargetName) const
{
	return 0.0f;
}

void IJointABPFacialAnimationInterface::SetEyeGazeRotation_Implementation(const FName& ID, const FRotator Rotation)
{
}

FRotator IJointABPFacialAnimationInterface::GetTargetEyeRotation_Implementation(const FName& ID) const
{
	return FRotator::ZeroRotator;
}





FJointFacialAnimationDefinition::FJointFacialAnimationDefinition()
{
}


//NullTrack
const FJointFacialAnimationTrack FJointFacialAnimationTrack::NullTrack = FJointFacialAnimationTrack();

FJointFacialAnimationTrack::FJointFacialAnimationTrack() : TrackGuid(FGuid::NewGuid())
{
	
}

void FJointFacialAnimationTrack::Start()
{
	CurrentElapsedTime = 0.0f;
	CurrentAlphaValue = 0.0f;
}

bool FJointFacialAnimationTrack::IsEnded() const
{
	return CurrentElapsedTime >= AnimationDefinition.Duration;
}


FJointEyeBlinkData::FJointEyeBlinkData()
{
}

FJointEyeGazeData::FJointEyeGazeData()
{
}







FJointEyeAnimationData::FJointEyeAnimationData()
{
}

void FJointEyeAnimationData::UpdateEyeBlink(float DeltaTime)
{
	EyeBlinkLeftTime = EyeBlinkLeftTime - DeltaTime;
}

bool FJointEyeAnimationData::CheckReadyToBlink() const
{
	return EyeBlinkLeftTime <= 0.0f;
}

void FJointEyeAnimationData::PickNextEyeBlinkTime()
{
	EyeBlinkLeftTime = FMath::FRandRange(
		EyeBlinkData.EyeBlinkIntervalMin,
		EyeBlinkData.EyeBlinkIntervalMax
	);
}

void FJointEyeAnimationData::UpdateEyeGaze(float DeltaTime)
{
	// Avoid prolonged eye contact (with timer, "idlebreaking")
	if (CanPerformIdleBreak()) StartEyeGazeIdleBreak();
	
	if (IsIdleBreakOnCooldown()) EyeGazeIdleBreakCooldownElapsedTime += DeltaTime;
	
	if (IsPerformingIdleBreak())
	{
		EyeGazeIdleBreakElapsedTime += DeltaTime;
		
		const int PreviousStep = EyeGazeIdleBreaksStep;
		
		UpdateEyeGazeIdleBreakStep();
		
		// Only update offset if we have progressed to a new step
		if (PreviousStep != EyeGazeIdleBreaksStep)
		{
			const float EyeGazeIdleBreaksStepProgress = EyeGazeIdleBreaksStep / FMath::Max(1, EyeGazeIdleBreaksStepCount);

			TargetEyeGazeRotationIdleBreakOffset = GetIdleBreakOffset(EyeGazeIdleBreaksStepProgress);
		}
		
	}else
	{
		TargetEyeGazeRotationIdleBreakOffset = FRotator::ZeroRotator;
	}
	
	
	// Subtle noise shake
	
	if (CanPerformSubtleShake())
	{
		PerformSubtleShake();
	}else
	{
		EyeGazeSubtleShakeCooldownElapsedTime += DeltaTime;
	}
	
	// log TargetEyeGazeRotationIdleBreakOffset
	//UE_LOG(LogTemp, Log, TEXT("Eye Animation Data ID: %s, TargetEyeGazeRotationIdleBreakOffset: Pitch=%f, Yaw=%f, Roll=%f"), *EyeAnimationDataID.ToString(), TargetEyeGazeRotationIdleBreakOffset.Pitch, TargetEyeGazeRotationIdleBreakOffset.Yaw, TargetEyeGazeRotationIdleBreakOffset.Roll);
	

	const FRotator FinalTargetEyeGazeRotation = GetFinalTargetEyeGazeRotation();


	// Eye jumping clipping threshold movement

	const float Distance = FinalTargetEyeGazeRotation.GetManhattanDistance(ActualEyeGazeRotation);

	if (!EyeGazeData.EyeGazeSaccadeCurve.IsNull())
	{
		if (const UCurveFloat* Curve = EyeGazeData.EyeGazeSaccadeCurve.LoadSynchronous())
		{
			const float Speed = Curve->GetFloatValue(Distance) * EyeGazeData.EyeGazeSaccadeSpeedMultiplier;

			ActualEyeGazeRotation = FMath::RInterpTo(
				ActualEyeGazeRotation,
				FinalTargetEyeGazeRotation,
				DeltaTime,
				Speed);
		}
	}
	else
	{
#if WITH_EDITOR
		UE_LOG(LogTemp, Warning, TEXT("EyeGazeSaccadeCurve is not set in EyeAnimationDataID: %s"), *EyeAnimationDataID.ToString());
#endif
	}
}

void FJointEyeAnimationData::PickIdleBreakDuration()
{
	EyeGazeIdleBreakDuration = FMath::RandRange(
		EyeGazeData.EyeGazeIdleBreakDurationMin,
		EyeGazeData.EyeGazeIdleBreakDurationMax
	);
}

FRotator FJointEyeAnimationData::GetFinalTargetEyeGazeRotation() const
{
	return TargetEyeGazeRotation + TargetEyeGazeRotationIdleBreakOffset + TargetEyeGazeRotationSubtleShakeOffset;
}

void FJointEyeAnimationData::StartEyeGazeIdleBreak()
{
	EyeGazeIdleBreakElapsedTime = 0.0f;
	
	EyeGazeIdleBreaksStep = 0;

	EyeGazeIdleBreaksStepCount = FMath::RandRange(
		EyeGazeData.EyeGazeIdleBreaksStepCountMin,
		EyeGazeData.EyeGazeIdleBreaksStepCountMax
	);
	
	PickIdleBreakDuration();
	
	PickNextEyeGazeIdleBreakTime();
}

bool FJointEyeAnimationData::CanPerformIdleBreak() const
{
	return !IsIdleBreakOnCooldown();
}

bool FJointEyeAnimationData::IsIdleBreakOnCooldown() const
{
	return EyeGazeIdleBreakCooldownDuration - EyeGazeIdleBreakCooldownElapsedTime >= 0.f;
}


bool FJointEyeAnimationData::IsPerformingIdleBreak() const
{
	return EyeGazeIdleBreakDuration - EyeGazeIdleBreakElapsedTime >= 0.f;
}

void FJointEyeAnimationData::PickNextEyeGazeIdleBreakTime()
{
	EyeGazeIdleBreakCooldownDuration = FMath::FRandRange(
		EyeGazeData.EyeGazeIdleBreakIntervalMin,
		EyeGazeData.EyeGazeIdleBreakIntervalMax);
	
	// Reset elapsed time and step count
	EyeGazeIdleBreakCooldownElapsedTime = 0.0f;
	EyeGazeIdleBreakElapsedTime = 0.0f;
}

FRotator FJointEyeAnimationData::GetIdleBreakOffset(const float EyeGazeIdleBreaksStepProgress)
{
	// Pick a random offset within the specified range for idle break.
	// human tend to look slightly left, down, right (but not upside) when their eyes are relaxing (looking straight),
	// but when they are looking at something, they tend to relax their eyes to make their eye look straight.
	
	if (TargetEyeGazeRotation.GetManhattanDistance(FRotator::ZeroRotator) > EyeGazeData.EyeGazeIdleBreakTensionThreshold)
	{
		//when they are looking at something while rolling their eye to the side, they tend to relax their eyes to make their eye look straight.
		//so, we have to set the offset to make the target rotation closer to zero.
		
		/*
		const FVector DirectionToStraight = TargetEyeGazeRotation.Vector().GetSafeNormal();

		const float Strength = FMath::FRandRange(
			EyeGazeData.EyeGazeIdleBreakSizeMin,
			EyeGazeData.EyeGazeIdleBreakSizeMax
		);

		const FVector OffsetDirection = DirectionToStraight;
		TargetEyeGazeRotationIdleBreakOffset = OffsetDirection.Rotation() * -Strength;
		*/
		
		// when they are relaxing their eyes, they tend to look slightly left, down, right (but not upside)
		const float YawOffset = FMath::FRandRange(EyeGazeData.EyeGazeIdleBreakSizeMin, EyeGazeData.EyeGazeIdleBreakSizeMax) * (FMath::RandBool() ? 1.0f : -1.0f);
		const float PitchOffset = FMath::FRandRange(0.0f, EyeGazeData.EyeGazeIdleBreakSizeMax); // only downward offset
		return FRotator(PitchOffset, YawOffset, 0.0f);
	}
	else
	{
		// when they are relaxing their eyes, they tend to look slightly left, down, right (but not upside)
		const float YawOffset = FMath::FRandRange(EyeGazeData.EyeGazeIdleBreakSizeMin, EyeGazeData.EyeGazeIdleBreakSizeMax) * (FMath::RandBool() ? 1.0f : -1.0f);
		const float PitchOffset = FMath::FRandRange(0.0f, EyeGazeData.EyeGazeIdleBreakSizeMax); // only downward offset
		return FRotator(PitchOffset, YawOffset, 0.0f);
	}
}

void FJointEyeAnimationData::UpdateEyeGazeIdleBreakStep()
{
	if (EyeGazeIdleBreaksStep < EyeGazeIdleBreaksStepCount)
	{
		UCurveFloat* StepCurve = EyeGazeData.EyeGazeStepCurve.LoadSynchronous();
		
		const float NormalizedElapsedTime = EyeGazeIdleBreakDuration > 0.0f ? (EyeGazeIdleBreakElapsedTime / EyeGazeIdleBreakDuration) : 1.0f;
		const float StepValue = StepCurve ? StepCurve->GetFloatValue(NormalizedElapsedTime) : 1.0f;
		
		EyeGazeIdleBreaksStep = FMath::Floor(StepValue);
	}
}

void FJointEyeAnimationData::PerformSubtleShake()
{
	
	if (FMath::FRand() > EyeGazeData.EyeGazeSubtleShakeChanceToReset)
	{
		TargetEyeGazeRotationSubtleShakeOffset = FRotator::ZeroRotator;
	}else
	{
		// Pick a random offset within the specified range for subtle shake.
		const float YawOffset = FMath::FRandRange(
			-EyeGazeData.EyeGazeSubtleShakeIntensity,
			EyeGazeData.EyeGazeSubtleShakeIntensity
		);
	
		const float PitchOffset = FMath::FRandRange(
			-EyeGazeData.EyeGazeSubtleShakeIntensity,
			EyeGazeData.EyeGazeSubtleShakeIntensity
		);
	
		TargetEyeGazeRotationSubtleShakeOffset = FRotator(PitchOffset, YawOffset, 0.0f);
	}
	
	// Reset cooldown timer
	EyeGazeSubtleShakeCooldownElapsedTime = 0.0f;
	
	PickNextSubtleShakeTime();
}

bool FJointEyeAnimationData::CanPerformSubtleShake()
{
	return EyeGazeSubtleShakeCooldownDuration - EyeGazeSubtleShakeCooldownElapsedTime <= 0.f;
}

void FJointEyeAnimationData::PickNextSubtleShakeTime()
{
	EyeGazeSubtleShakeCooldownDuration = FMath::FRandRange(
		EyeGazeData.EyeGazeSubtleShakeCooldownDurationMin,
		EyeGazeData.EyeGazeSubtleShakeCooldownDurationMax);
}


// Sets default values for this component's properties
UJointFacialAnimationComponent::UJointFacialAnimationComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;


	// ...
}


// Called when the game starts
void UJointFacialAnimationComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	GrabABPInterface();
	StartTickTimer();
}

void UJointFacialAnimationComponent::GrabABPInterface()
{
	if (AActor* Owner = GetOwner())
	{
		if (const ACharacter* OwnerAsCharacter = Cast<ACharacter>(Owner))
		{
			if (UAnimInstance* AnimInstance = OwnerAsCharacter->GetMesh()->GetAnimInstance())
			{
				if (AnimInstance->GetClass()->ImplementsInterface(UJointABPFacialAnimationInterface::StaticClass()))
				{
					ABPInterface.SetObject(AnimInstance);
					ABPInterface.SetInterface(Cast<IJointABPFacialAnimationInterface>(AnimInstance));
				}
			}
		}
	}
}

bool UJointFacialAnimationComponent::HasValidABPInterface() const
{
	return ABPInterface.GetObject() != nullptr;
}

void UJointFacialAnimationComponent::StartTickTimer()
{
	if (const UWorld* World = GetWorld())
	{
		World->GetTimerManager().SetTimer(
			TickTimerHandle,
			this,
			&UJointFacialAnimationComponent::TickTimerCallback,
			TickInterval,
			true);
	}
}

void UJointFacialAnimationComponent::StopTickTimer()
{
	if (const UWorld* World = GetWorld())
	{
		World->GetTimerManager().ClearTimer(TickTimerHandle);
	}
}


void UJointFacialAnimationComponent::TickTimerCallback()
{
	if (!GetWorld()) return;

	const float InTickInterval = GetWorld()->GetTimerManager().GetTimerElapsed(TickTimerHandle);

	UpdateFacialAnimationElapsedTimes(InTickInterval);
	UpdateFacialAnimationAlphaValues();
	UpdateEyeAnimations();

	ApplyFacialAnimationAnimation();
	ApplyEyeAnimations();
	
	
	// Expire ended tracks here because we want to make sure they are removed after applying their last frame
	ExpireEndedFacialAnimationTracks();

}


FJointFacialAnimationTrack UJointFacialAnimationComponent::StartFacialAnimation(const FJointFacialAnimationDefinition& AnimationDefinition)
{
	FJointFacialAnimationDefinition NewAnimationData = AnimationDefinition;

	FJointFacialAnimationTrack NewTrack;
	NewTrack.AnimationDefinition = NewAnimationData;
	NewTrack.Start();

	FacialAnimationTracks.Add(NewTrack);

	return *FacialAnimationTracks.Find(NewTrack);
}

FJointFacialAnimationTrack UJointFacialAnimationComponent::GetFacialAnimationTrackByID(const FGuid& TrackGuid)
{
	FJointFacialAnimationTrack NewTrack;
	NewTrack.TrackGuid = TrackGuid;
	
	return *FacialAnimationTracks.Find(NewTrack);
}

void UJointFacialAnimationComponent::UpdateFacialAnimationElapsedTimes(float DeltaTime)
{
	for (FJointFacialAnimationTrack& FacialAnimationTrack : FacialAnimationTracks)
	{
		if (FacialAnimationTrack.CurrentElapsedTime < FacialAnimationTrack.AnimationDefinition.Duration)
		{
			FacialAnimationTrack.CurrentElapsedTime += DeltaTime;

			//UE_LOG(LogTemp, Log, TEXT("Updating Elapsed Time for Animation: %s, CurrentElapsedTime: %f"), *AnimationData.AnimationName.ToString(), AnimationData.CurrentElapsedTime);

			if (FacialAnimationTrack.CurrentElapsedTime > FacialAnimationTrack.AnimationDefinition.Duration)
			{
				FacialAnimationTrack.CurrentElapsedTime = FacialAnimationTrack.AnimationDefinition.Duration;
			}
		}
	}
}

void UJointFacialAnimationComponent::UpdateFacialAnimationAlphaValues()
{
	for (FJointFacialAnimationTrack& FacialAnimationTrack : FacialAnimationTracks)
	{
		if (const UCurveFloat* Curve = FacialAnimationTrack.AnimationDefinition.StrengthCurve.LoadSynchronous())
		{
			const float NormalizedTime = FacialAnimationTrack.AnimationDefinition.Duration > 0.0f ? (FacialAnimationTrack.CurrentElapsedTime / FacialAnimationTrack.AnimationDefinition.Duration) : 1.0f;

			//UE_LOG(LogTemp, Log, TEXT("Updating Alpha Value for Animation: %s, NormalizedTime: %f"), *AnimationData.AnimationName.ToString(), NormalizedTime);

			FacialAnimationTrack.CurrentAlphaValue = FMath::Lerp(
				FacialAnimationTrack.CurrentAlphaValue,
				Curve->GetFloatValue(NormalizedTime) * FacialAnimationTrack.AnimationDefinition.StrengthMultiplier,
				FacialAnimationTrack.AnimationDefinition.InterpolationAlpha);
		}
	}
}

void UJointFacialAnimationComponent::ApplyFacialAnimationAnimation()
{
	if (!HasValidABPInterface()) return;

	TMap<FName, float> AnimationGroupToWeightSumMap;

	// First pass: calculate total weights per animation group
	// This allows us to normalize weights when multiple animations in the same group are active
	// and avoid exceeding a total weight of 1.0f

	for (FJointFacialAnimationTrack& FacialAnimationTrack : FacialAnimationTracks)
	{
		if (!AnimationGroupToWeightSumMap.Contains(FacialAnimationTrack.AnimationDefinition.AnimationGroupName))
		{
			AnimationGroupToWeightSumMap.Add(FacialAnimationTrack.AnimationDefinition.AnimationGroupName, 0.0f);
		}

		AnimationGroupToWeightSumMap[FacialAnimationTrack.AnimationDefinition.AnimationGroupName] += FacialAnimationTrack.AnimationDefinition.Weight * FacialAnimationTrack.CurrentAlphaValue;
	}

	// Second pass: apply normalized weights to morph targets

	for (FJointFacialAnimationTrack& FacialAnimationTrack : FacialAnimationTracks)
	{
		float* WeightSumPtr = AnimationGroupToWeightSumMap.Find(FacialAnimationTrack.AnimationDefinition.AnimationGroupName);

		if (!WeightSumPtr) continue;

		if (FacialAnimationTrack.AnimationDefinition.bUseMorphTarget && !FacialAnimationTrack.AnimationDefinition.MorphTargetName.IsNone())
		{
			const float Raw = FacialAnimationTrack.AnimationDefinition.Weight * FacialAnimationTrack.CurrentAlphaValue;

			float Final = (*WeightSumPtr > 1.0f) ? (Raw / *WeightSumPtr) : Raw;
			
			IJointABPFacialAnimationInterface::Execute_SetMorphTargetWeight(ABPInterface.GetObject(), FacialAnimationTrack.AnimationDefinition.MorphTargetName, Final);
		}
	}
}

void UJointFacialAnimationComponent::ExpireEndedFacialAnimationTracks()
{
	TSet<FJointFacialAnimationTrack> TracksToRemove;

	for (const FJointFacialAnimationTrack& FacialAnimationTrack : FacialAnimationTracks)
	{
		if (FacialAnimationTrack.IsEnded())
		{
			TracksToRemove.Add(FacialAnimationTrack);
		}
	}

	for (const FJointFacialAnimationTrack& TrackToRemove : TracksToRemove)
	{
		FacialAnimationTracks.Remove(TrackToRemove);
	}
}


void UJointFacialAnimationComponent::UpdateEyeAnimations()
{
	if (!HasValidABPInterface()) return;
	
	for (FJointEyeAnimationData& EyeAnimationData : EyeAnimationDataArray)
	{
		EyeAnimationData.UpdateEyeBlink(TickInterval);

		if (EyeAnimationData.EyeGazeData.bUseEyeGaze)
		{
			if (!EyeAnimationData.EyeGazeData.bUseManualTargetEyeGazeRotation)
			{
				EyeAnimationData.TargetEyeGazeRotation = IJointABPFacialAnimationInterface::Execute_GetTargetEyeRotation(ABPInterface.GetObject(), EyeAnimationData.EyeAnimationDataID);
			}

			EyeAnimationData.UpdateEyeGaze(TickInterval);

			IJointABPFacialAnimationInterface::Execute_SetEyeGazeRotation(ABPInterface.GetObject(), EyeAnimationData.EyeAnimationDataID, EyeAnimationData.ActualEyeGazeRotation);
		}
	}
}

void UJointFacialAnimationComponent::ApplyEyeAnimations()
{
	for (FJointEyeAnimationData& EyeAnimationData : EyeAnimationDataArray)
	{
		if (EyeAnimationData.CheckReadyToBlink())
		{
			PerformEyeBlink(EyeAnimationData);
		}
	}
}

void UJointFacialAnimationComponent::PerformEyeBlink(FJointEyeAnimationData& EyeAnimationData)
{
	
	StartFacialAnimation(EyeAnimationData.EyeBlinkData.EyeBlinkAnimationDefinition);

	EyeAnimationData.PickNextEyeBlinkTime();
}
