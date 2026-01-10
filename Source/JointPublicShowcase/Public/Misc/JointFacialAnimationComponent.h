// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "JointFacialAnimationComponent.generated.h"

UINTERFACE(Blueprintable)
class UJointABPFacialAnimationInterface : public UInterface
{
	GENERATED_BODY()
	
};

class IJointABPFacialAnimationInterface
{
	GENERATED_BODY()

public:
	
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category="IJointABPFacialAnimationInterface")
	void SetMorphTargetWeight(const FName& MorphTargetName, const float Weight);
	
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category="IJointABPFacialAnimationInterface")
	float GetMorphTargetWeight(const FName& MorphTargetName) const;
	
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category="IJointABPFacialAnimationInterface")
	void SetEyeGazeRotation(const FName& ID, const FRotator Rotation);
	
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category="IJointABPFacialAnimationInterface")
	FRotator GetTargetEyeRotation(const FName& ID) const;

public:
	
	virtual void SetMorphTargetWeight_Implementation(const FName& MorphTargetName, const float Weight);

	virtual float GetMorphTargetWeight_Implementation(const FName& MorphTargetName) const;
	
	virtual void SetEyeGazeRotation_Implementation(const FName& ID, const FRotator Rotation);
	
	virtual FRotator GetTargetEyeRotation_Implementation(const FName& ID) const;
	
};

USTRUCT(BlueprintType)
struct JOINTPUBLICSHOWCASE_API FJointFacialAnimationDefinition
{
	GENERATED_BODY()
	
public:
	
	FJointFacialAnimationDefinition();
	
public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Facial Animation")
	FName AnimationGroupName = "DefaultGroup";
	
public:
	
	// we only support morph target for now - but it's quite easy to extend to other methods like poses
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Facial Animation")
	bool bUseMorphTarget = true;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Facial Animation")
	FName MorphTargetName;

public:
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Facial Animation")
	TSoftObjectPtr<UCurveFloat> StrengthCurve; // curve to control weight over time.
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Facial Animation")
	float StrengthMultiplier = 1.f;
	
	/**
	 * Weight multiplier for this animation data.
	 * Used to scale the final weight applied to the morph target - if multiple animations in the same group are active,
	 * their weights are summed up and then each animation's alpha is divided by the total weight sum to get the final weight.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Facial Animation")
	float Weight = 1.f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Facial Animation")
	float Duration = 0.2f;

	/**
	 * Interpolation alpha value for alpha value changes.
	 * 0.0 = no change, 1.0 = instant change.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Facial Animation")
	float InterpolationAlpha = 0.8f;
	
};


/**
 * A struct that represents the runtime data for a facial animation.
 */
USTRUCT(BlueprintType)
struct JOINTPUBLICSHOWCASE_API FJointFacialAnimationTrack
{
	GENERATED_BODY()
	
public:
	
	FJointFacialAnimationTrack();
	
public:
	
	void Start();
	bool IsEnded() const;
	bool IsNull() const { return *this == NullTrack; }
	bool IsValid() const { return !IsNull(); }

public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Facial Animation")
	FJointFacialAnimationDefinition AnimationDefinition;
	
public:
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="Facial Animation")
	float CurrentAlphaValue = 0.0f;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="Facial Animation")
	float CurrentElapsedTime = 0.0f;
	
public:
	
	/**
	 * Unique identifier for this track.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Facial Animation", meta=(AllowPrivateAccess="true"))
	FGuid TrackGuid;
	
	inline bool operator==(const FJointFacialAnimationTrack& Other) const
	{
		return TrackGuid == Other.TrackGuid;
	}
	
	static const FJointFacialAnimationTrack NullTrack;
	
};

FORCEINLINE uint32 GetTypeHash(const FJointFacialAnimationTrack& InTrack)
{
	return GetTypeHash(InTrack.TrackGuid);
}
	



	
USTRUCT(BlueprintType)
struct JOINTPUBLICSHOWCASE_API FJointEyeBlinkData
{
	GENERATED_BODY()
	
public:

	FJointEyeBlinkData();
	
public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Facial Animation|Eye Gaze")
	bool bUseEyeBlink = true;
	
public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Facial Animation|Eye Blink")
	float EyeBlinkIntervalMin = 4.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Facial Animation|Eye Blink")
	float EyeBlinkIntervalMax = 7.0f;
	
public:
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Facial Animation|Eye Blink")
	FJointFacialAnimationDefinition EyeBlinkAnimationDefinition;
	
};

USTRUCT(BlueprintType)
struct JOINTPUBLICSHOWCASE_API FJointEyeGazeData
{
	GENERATED_BODY()
	
	/**
	 * Eye gaze update logic must consider this:
	 * 1. humans' eye movements are typically quick and saccadic, with rapid shifts in focus - it works like a series of quick snapping motions rather than smooth transitions. (도약안구운동) (This can be simulated with clamping based on a threshold)
	 * 2. Eye movements are often coordinated with head movements and facial expressions, so they should be integrated into the overall animation system to ensure natural behavior.
	 * 3. When a human is speaking, their eye movements may be influenced by the rhythm and emphasis of their speech, leading to more dynamic and context-aware animations.
	 * 4. When a human is speaking, they change their eye focus more frequently, compare to when they are idle and either listening to someone.
	 * 5. Humans tend to avoid prolonged direct eye contact, so incorporating natural breaks in
	 */

public:
	
	FJointEyeGazeData();

public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Facial Animation|Eye Gaze")
	bool bUseEyeGaze = true;
	
	// Whether to use manual target eye gaze rotation (set via TargetEyeGazeRotation) or get it from ABP interface.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Facial Animation|Eye Gaze")
	bool bUseManualTargetEyeGazeRotation = false;
	
public:
	
	// curve to control saccade speed over time. X axis = Distance (Manhattan Distance), Y axis = speed multiplier.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Facial Animation|Eye Gaze")
	TSoftObjectPtr<UCurveFloat> EyeGazeSaccadeCurve; 
	
	// multiplier for saccade speed curve.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Facial Animation|Eye Gaze")
	float EyeGazeSaccadeSpeedMultiplier = 3.0f; 
	
public:
	
	// degrees. Intensity of subtle noise shake applied to eye gaze. (to simulate micro-saccades)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Facial Animation|Eye Gaze|Subtle Shake")
	float EyeGazeSubtleShakeIntensity = 7.5f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Facial Animation|Eye Gaze|Subtle Shake")
	float EyeGazeSubtleShakeCooldownDurationMin = 0.3f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Facial Animation|Eye Gaze|Subtle Shake")
	float EyeGazeSubtleShakeCooldownDurationMax = 1.2f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Facial Animation|Eye Gaze|Subtle Shake")
	float EyeGazeSubtleShakeChanceToReset = 0.66f; // chance to reset the offset to zero each time the cooldown ends.

	
public:
	
	// degrees. Minimum offset size for eye contact breaks when idle.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Facial Animation|Eye Gaze|Idle Break")
	float EyeGazeIdleBreakSizeMin = 12.0f; 
	
	// degrees. Maximum offset size for eye contact breaks when idle.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Facial Animation|Eye Gaze|Idle Break")
	float EyeGazeIdleBreakSizeMax = 22.0f; 
	
	// seconds. Minimum interval between eye contact breaks when idle.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Facial Animation|Eye Gaze|Idle Break")
	float EyeGazeIdleBreakIntervalMin = 2.7f;
	
	// seconds. Maximum interval between eye contact breaks when idle.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Facial Animation|Eye Gaze|Idle Break")
	float EyeGazeIdleBreakIntervalMax = 5.0f;
	
	// seconds. Duration of eye contact break when idle.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Facial Animation|Eye Gaze|Idle Break")
	float EyeGazeIdleBreakDurationMin = 1.2f;
	
	// seconds. Duration
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Facial Animation|Eye Gaze|Idle Break")
	float EyeGazeIdleBreakDurationMax = 2.f; 
	
	// degrees. If the target eye gaze rotation is beyond this threshold, the idle break offset will be adjusted to relax the eye gaze towards straight ahead.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Facial Animation|Eye Gaze|Idle Break")
	float EyeGazeIdleBreakTensionThreshold = 30.0f;
	
public:
	
	// step related
	
	// curve to control step size over progress. X axis = duration percentage (0.0 to 1.0), Y axis = step size multiplier (0.0 to 1.0, min to max).
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Facial Animation|Eye Gaze|Idle Break")
	TSoftObjectPtr<UCurveFloat> EyeGazeStepCurve; 
	
	// additional random breaks, min count
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Facial Animation|Eye Gaze|Idle Break")
	int EyeGazeIdleBreaksStepCountMin = 0;
	
	// additional random breaks, max count
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Facial Animation|Eye Gaze|Idle Break")
	int EyeGazeIdleBreaksStepCountMax = 1;
	
};

USTRUCT(BlueprintType)
struct JOINTPUBLICSHOWCASE_API FJointEyeAnimationData
{
	GENERATED_BODY()

public:

	FJointEyeAnimationData();

public:

	void UpdateEyeBlink(float DeltaTime);
	bool CheckReadyToBlink() const;
	void PickNextEyeBlinkTime();

public:
	void UpdateEyeGaze(float DeltaTime);
	void PickIdleBreakDuration();
	FRotator GetFinalTargetEyeGazeRotation() const;

public:
	
	void StartEyeGazeIdleBreak();
	
	bool CanPerformIdleBreak() const;
	bool IsIdleBreakOnCooldown() const;
	bool IsPerformingIdleBreak() const;
	
	void PickNextEyeGazeIdleBreakTime();
	FRotator GetIdleBreakOffset(const float EyeGazeIdleBreaksStepProgress);
	void UpdateEyeGazeIdleBreakStep();
	
public:

	void PerformSubtleShake();
	bool CanPerformSubtleShake();
	void PickNextSubtleShakeTime();
	
public:

	/**
	 * Unique ID for this eye gaze data.
	 * If your character has multiple eye gaze controllers, use different IDs to distinguish them.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Facial Animation|Eye Gaze")
	FName EyeAnimationDataID = NAME_None;

public:
	
	/** 
	 * Eye blink configuration data.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Facial Animation|Eye Gaze")
	FJointEyeBlinkData EyeBlinkData;
	
	/** 
	 * Eye gaze configuration data.
	 * You can adjust parameters like saccade threshold, speed, noise intensity, etc. here.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Facial Animation|Eye Gaze")
	FJointEyeGazeData EyeGazeData;

public:
	
	inline bool operator==(const FJointEyeAnimationData& Other) const
	{
		return EyeAnimationDataID == Other.EyeAnimationDataID;
	}

public:
	
	//Runtime parameters
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Facial Animation|Eye Blink")
	float EyeBlinkLeftTime = 0.0f;
	
public:
	
	// Actual eye gaze rotation applied to the character.
	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Category="Facial Animation|Eye Gaze")
	FRotator ActualEyeGazeRotation;
	
	// Target eye gaze rotation (where the character should be looking at).
	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Category="Facial Animation|Eye Gaze")
	FRotator TargetEyeGazeRotation;
	
	// Offset to be added to the target eye gaze rotation (for Idle breaks)
	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Category="Facial Animation|Eye Gaze")
	FRotator TargetEyeGazeRotationIdleBreakOffset;
	
	// Offset to be added to the target eye gaze rotation (for subtle noise shake)
	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Category="Facial Animation|Eye Gaze")
	FRotator TargetEyeGazeRotationSubtleShakeOffset;
	
public:

	UPROPERTY(VisibleAnywhere, Category="Facial Animation|Eye Gaze")
	float EyeGazeIdleBreakCooldownDuration = 0.0f;
	
	UPROPERTY(VisibleAnywhere, Category="Facial Animation|Eye Gaze")
	float EyeGazeIdleBreakCooldownElapsedTime = 0.0f;
	
	UPROPERTY(VisibleAnywhere, Category="Facial Animation|Eye Gaze")
	float EyeGazeIdleBreakDuration = 0.0f;
	
	UPROPERTY(VisibleAnywhere, Category="Facial Animation|Eye Gaze")
	float EyeGazeIdleBreakElapsedTime = 0.0f;
	
public:

	UPROPERTY(VisibleAnywhere, Category="Facial Animation|Eye Gaze")
	float EyeGazeSubtleShakeCooldownDuration = 0.0f;
	
	UPROPERTY(VisibleAnywhere, Category="Facial Animation|Eye Gaze")
	float EyeGazeSubtleShakeCooldownElapsedTime = 0.0f;
	
public:
	
	// idle break has multiple steps: initial break takes big jumps, then subsequent breaks are smaller adjustments that are related to the previous break.
	
	// For counting additional idle breaks
	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Category="Facial Animation|Eye Gaze")
	int EyeGazeIdleBreaksStepCount = 1;
	
	// For counting additional idle breaks
	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Category="Facial Animation|Eye Gaze")
	int EyeGazeIdleBreaksStep = 0;
	
	
};






UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class JOINTPUBLICSHOWCASE_API UJointFacialAnimationComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	
	// Sets default values for this component's properties
	UJointFacialAnimationComponent();
	
public:
	
	virtual void BeginPlay() override;

public:

	void GrabABPInterface();
	
	bool HasValidABPInterface() const;
	
public:
	
	UPROPERTY()
	TScriptInterface<IJointABPFacialAnimationInterface> ABPInterface;
	
public:
	
	UFUNCTION(BlueprintCallable, Category="Tick")
	void StartTickTimer();
	
	UFUNCTION(BlueprintCallable, Category="Tick")
 	void StopTickTimer();

	UFUNCTION()
	void TickTimerCallback();
	
public:
	
	/**
	 * Starts a facial animation based on the provided definition.
	 * @param AnimationDefinition The definition of the facial animation to start.
	 * @return A unique identifier for the started facial animation track.
	 */
	UFUNCTION(BlueprintCallable, Category="Facial Animation")
	FJointFacialAnimationTrack StartFacialAnimation(const FJointFacialAnimationDefinition& AnimationDefinition);
	
	UFUNCTION(BlueprintPure, Category="Facial Animation")
	FJointFacialAnimationTrack GetFacialAnimationTrackByID(const FGuid& TrackGuid);
	
public:
	
	void UpdateFacialAnimationElapsedTimes(float DeltaTime);
	void UpdateFacialAnimationAlphaValues();
	void ApplyFacialAnimationAnimation();
	
	void ExpireEndedFacialAnimationTracks();

	
public:

	void UpdateEyeAnimations();
	void ApplyEyeAnimations();
	
public:
	
	UFUNCTION(BlueprintCallable, Category="Facial Animation|Eye Blink")
	void PerformEyeBlink(FJointEyeAnimationData& EyeAnimationData);

public:
	
	/**
	 * Current active facial animation tracks.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Facial Animation")
	TSet<FJointFacialAnimationTrack> FacialAnimationTracks;

public:
	
	/**
	 * Facial animation data definitions.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Facial Animation")
	TArray<FJointEyeAnimationData> EyeAnimationDataArray;
	
public:
	
	UPROPERTY()
	FTimerHandle TickTimerHandle;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Facial Animation")
	float TickInterval = 0.01f;
	
};

