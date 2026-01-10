// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "JointEyeControlComponent.generated.h"


class UJointFacialAnimationComponent;

USTRUCT(BlueprintType)
struct JOINTPUBLICSHOWCASE_API FJointEyeControlLookAtTrackDefinition
{ 
	GENERATED_BODY()
	
public:
	
	FJointEyeControlLookAtTrackDefinition();
	
public:
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Eye Control")
	float MaxAge;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Eye Control")
	float Weight;
	
public:
	
	/**
	 * Target world location for the eye to look at.
	 * If TargetActor is set, this will be ignored.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Eye Control")
	FVector TargetLocation;
	
	/**
	 * Target actor for the eye to look at.
	 * If set, the eye will look at the actor's location.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Eye Control")
	AActor* TargetActor;
	
};

USTRUCT(BlueprintType)
struct JOINTPUBLICSHOWCASE_API FJointEyeControlLookAtTrack
{
	GENERATED_BODY()
	
public:
	
	FJointEyeControlLookAtTrack();
	
public:
	
	void Start();
	bool IsEnded() const;
	bool IsNull() const { return *this == NullTrack; }
	bool IsValid() const { return !IsNull(); }
	
public:

	const FGuid& GetTrackGuid() const { return TrackGuid; }
	const FName& GetTrackName() const { return TrackName; }

public:
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="Eye Control")
	float CurrentAge = 0;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="Eye Control")
	float CurrentWeight = 1.0f;
	
public:
	
	/**
	 * Optional name for this track. Used for additional identification or debugging purposes.
	 * (Can be NAME_None if not needed - the system uses GUID as the primary identifier)
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Eye Control")
	FName TrackName = NAME_None;
	
public:

	FVector GetCurrentTargetLookAtLocation() const;
	
public:
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="Eye Control")
	FJointEyeControlLookAtTrackDefinition Definition;
	
private:
	
	/**
	 * Unique identifier for this track.
	 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Eye Control", meta=(AllowPrivateAccess="true"))
	FGuid TrackGuid;
	
	inline bool operator==(const FJointEyeControlLookAtTrack& Other) const
	{
		return TrackGuid == Other.TrackGuid;
	}
	
public:
	
	static const FJointEyeControlLookAtTrack NullTrack;
	
	friend uint32 GetTypeHash(const FJointEyeControlLookAtTrack& InTrack);
	
};

FORCEINLINE uint32 GetTypeHash(const FJointEyeControlLookAtTrack& InTrack)
{
	return GetTypeHash(InTrack.TrackGuid);
}
	





UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class JOINTPUBLICSHOWCASE_API UJointEyeControlComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	
	// Sets default values for this component's properties
	UJointEyeControlComponent();
	
public:
	
	virtual void BeginPlay() override;

public:
	
	UFUNCTION(BlueprintCallable, Category="Tick")
	void StartTickTimer();
	
	UFUNCTION(BlueprintCallable, Category="Tick")
 	void StopTickTimer();
	
	UFUNCTION()
	void TickTimerCallback();
	
public:

	void UpdateTracks(float DeltaTime);
	
	void ApplyEyeControlLookAtTracks();
	
public:

	void OnTrackExpired(FJointEyeControlLookAtTrack& EndedTrack);
	
public:
	
	void StartCollectingLookAtNearbyActors();
	void StopCollectingLookAtNearbyActors();
	
	virtual bool CheckIfCanLookAtActor(AActor* OtherActor) const;

public:
	
	UFUNCTION(BlueprintPure, Category="Eye Control")
	UJointFacialAnimationComponent* GetFacialAnimationComponentFromOwner() const;
	
public:

	UFUNCTION(BlueprintCallable, Category="Eye Control")
	FJointEyeControlLookAtTrack FindEyeControlLookAtTrackForActor(AActor* TargetActor);
	
	UFUNCTION(BlueprintCallable, Category="Eye Control")
	FJointEyeControlLookAtTrack FindEyeControlLookAtTrackForGuid(const FGuid& Guid);
	
	UFUNCTION(BlueprintCallable, Category="Eye Control")
	FJointEyeControlLookAtTrack FindEyeControlLookAtTrackForName(const FName& TrackName);
	
	UFUNCTION(BlueprintCallable, Category="Eye Control")
	FGuid AddEyeControlLookAtTrackForDefinition(const FName& TrackName, const FJointEyeControlLookAtTrackDefinition& TrackDefinition);
	
public:

	/**
	 * Override an existing eye control look at track with new track struct data. (Match by GUID)
	 * @param NewTrack The new track data to override the existing track.
	 */
	UFUNCTION(BlueprintCallable, Category="Eye Control")
	void OverrideJointEyeControlLookAtTrack(const FJointEyeControlLookAtTrack& NewTrack);
	
public:
	
	FJointEyeControlLookAtTrack* FindEyeControlLookAtTrackForActor_Ptr(AActor* TargetActor);
	FJointEyeControlLookAtTrack* FindEyeControlLookAtTrackForGuid_Ptr(const FGuid& Guid);
	FJointEyeControlLookAtTrack* FindEyeControlLookAtTrackForName_Ptr(const FName& TrackName);

public:

	FVector GetTargetLookAtLocation() const;
	
	void ReserveForUpdatingTargetLookAt(float UpdateInterval, bool bForce = false);
	void CancelReservationForUpdatingTargetLookAt();
	
	UFUNCTION()
	void UpdateTargetLookAt();
	
public:
	
	UFUNCTION()
	void CollectLookAtNearbyActors();
	
	UPROPERTY()
	FTimerHandle NearbyActorsTraceTimerHandle;
	
	// (can be controlled via cinematic)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Eye Control", meta=(ExposeOnSpawn=true), Interp)
	bool bEnableLookAtNearbyActors = true;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Eye Control")
	float NearbyActorsTraceTickInterval = 0.66f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Eye Control")
	float NearbyActorsTraceRadius = 400.0f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Eye Control")
	float NearbyActorsBaseWeight = 20.f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Eye Control")
	float NearbyActorsVelocityWeightMultiplier = 0.5f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Eye Control")
	bool bUseLinearVelocityForWeighting = true;
	
public:
	
	UPROPERTY()
	FTimerHandle TargetLookAtUpdateTimerHandle;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Eye Control")
	FFloatRange TargetLookAtIntervalRange = FFloatRange(0.8f, 1.9f);
	
	/**
	 * Current active target look at tracks. They will be used to compute the final look at location.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Eye Control")
	TArray<int> TargetLookAtTracks;

public:
	
	/**
	 * Current active eye control look at tracks.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Eye Control")
	TArray<FJointEyeControlLookAtTrack> EyeControlLookAtTracks;

public:
	
	UPROPERTY()
	FTimerHandle TickTimerHandle;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Eye Control")
	float TickInterval = 0.01f;
	
public:
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Eye Control")
	bool bPrintDebugLogs = false;
	
public:
	
	UFUNCTION(BlueprintPure, Category="Eye Control")
	static bool IsNullEyeControlLookAtTrack(const FJointEyeControlLookAtTrack& Track)
	{
		return Track.IsNull();
	}
	
};

