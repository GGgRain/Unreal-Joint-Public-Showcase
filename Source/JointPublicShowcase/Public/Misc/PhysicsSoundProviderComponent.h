// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "UObject/WeakObjectPtr.h"
#include "Engine/NetSerialization.h"
#include "Sound/SoundBase.h"
#include "PhysicsSoundProviderComponent.generated.h"


class UPhysicalMaterial;
class USoundAttenuation;

USTRUCT(BlueprintType)
struct JOINTPUBLICSHOWCASE_API FPhysicsSoundProviderStructureIndex
{
	GENERATED_BODY()
	
public:
	
	FPhysicsSoundProviderStructureIndex() : SelfPhysicalMaterial(nullptr), HitPhysicalMaterial(nullptr)
	{
	}

	FPhysicsSoundProviderStructureIndex(
		TSoftObjectPtr<UPhysicalMaterial> InSelfPhysicalMaterial,
		TWeakObjectPtr<class UPhysicalMaterial> InHitPhysicalMaterial
	) : SelfPhysicalMaterial(InSelfPhysicalMaterial), HitPhysicalMaterial(InHitPhysicalMaterial)
	{
	}
	
	~FPhysicsSoundProviderStructureIndex() {}
	
public:
	
	// Physical material of the self object. Leave it empty to match any.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="PhysicsSound")
	TSoftObjectPtr<UPhysicalMaterial> SelfPhysicalMaterial;
	
	// Physical material of the hit object. Leave it empty to match any.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="PhysicsSound")
	TWeakObjectPtr<class UPhysicalMaterial> HitPhysicalMaterial;
	
public:
	
	FORCEINLINE bool operator==(const FPhysicsSoundProviderStructureIndex& Other) const
	{
		return (SelfPhysicalMaterial == Other.SelfPhysicalMaterial) && (HitPhysicalMaterial == Other.HitPhysicalMaterial);
	}
	
	FORCEINLINE bool operator!=(const FPhysicsSoundProviderStructureIndex& Other) const
	{
		return !(*this == Other);
	}
	

};

inline uint32 GetTypeHash( const FPhysicsSoundProviderStructureIndex A )
{
	uint32 Hash = 0;
	Hash = HashCombine(Hash, GetTypeHash(A.SelfPhysicalMaterial));
	Hash = HashCombine(Hash, GetTypeHash(A.HitPhysicalMaterial));
	return Hash;
}



USTRUCT(BlueprintType)
struct JOINTPUBLICSHOWCASE_API FPhysicsSoundProviderStructure
{
 	GENERATED_BODY()
public:
	
	FPhysicsSoundProviderStructure() : MasterSound(nullptr)
    {
    }

    ~FPhysicsSoundProviderStructure() {}
	
	// Minimum impulse required to trigger sound playback.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="PhysicsSound")
	float ImpulseThreshold = 100.0f;
	
	// Master sound to play if no PhysicalMaterial override is found
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="PhysicsSound")
	TSoftObjectPtr<USoundBase> MasterSound;
	
	// Map of PhysicalMaterial to SoundBase overrides
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="PhysicsSound")
	TMap<FPhysicsSoundProviderStructureIndex, TSoftObjectPtr<USoundBase>> PhysicalMaterialToSoundMap;
	
public:
	
	USoundBase* FindSoundForPhysicalMaterials(
		const FPhysicsSoundProviderStructureIndex& InIndexKey
	) const
	{
		
		// 1. Exact match - it's cheap, so try first
		if (PhysicalMaterialToSoundMap.Contains(InIndexKey))
		{
			return PhysicalMaterialToSoundMap[InIndexKey].LoadSynchronous();
		}
		
		// 2. check if we have a match for SelfPhysicalMaterial only
		FPhysicsSoundProviderStructureIndex SelfOnlyKey(
			InIndexKey.SelfPhysicalMaterial,
			nullptr
		);
		
		if (PhysicalMaterialToSoundMap.Contains(SelfOnlyKey))
		{
			return PhysicalMaterialToSoundMap[SelfOnlyKey].LoadSynchronous();
		}
		
		// 3. check if we have a match for HitPhysicalMaterial only
		
		FPhysicsSoundProviderStructureIndex HitOnlyKey(
			nullptr,
			InIndexKey.HitPhysicalMaterial
		);
		
		if (PhysicalMaterialToSoundMap.Contains(HitOnlyKey))
		{
			return PhysicalMaterialToSoundMap[HitOnlyKey].LoadSynchronous();
		}
		
		// 4. no match found - fallback to master sound
		return MasterSound.LoadSynchronous();
	}
};


/**
 * PrimitiveComponentPhysicsSoundProvider
 * Provide a simple collision based sound playback (This one by itself can be a product btw)
 * It's used on Pekorobo's meshes to provide physics based sound effects
 */
UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class JOINTPUBLICSHOWCASE_API UPhysicsSoundProviderComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	
	// Sets default values for this component's properties
	UPhysicsSoundProviderComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	
public:

	UFUNCTION(BlueprintCallable, Category="PhysicsSound")
	void ActivatePhysicsSoundProvider();
	
	UFUNCTION(BlueprintCallable, Category="PhysicsSound")
	void DeactivatePhysicsSoundProvider();
	
public:
	
	UFUNCTION(BlueprintPure, Category="PhysicsSound")
	UPrimitiveComponent* FindTargetPrimitiveComponent() const;
	
	void BindHitEventToPrimitiveComponent(UPrimitiveComponent* TargetPrimitiveComponent);
	void UnbindHitEventFromPrimitiveComponent(UPrimitiveComponent* TargetPrimitiveComponent);
	
public:
	
	UFUNCTION()
	void OnTargetComponentHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);
	
	UFUNCTION(BlueprintNativeEvent, Category="PhysicsSound")
	void HandlePhysicsHitEvent(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);
	virtual void HandlePhysicsHitEvent_Implementation(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

public:
	
	void PlayPhysicsSoundFromStructureArray(
		const TArray<FPhysicsSoundProviderStructure>& InArray, 
		const FVector_NetQuantize& Location,
		float BodyMass,
		float ImpactStrength,
		float SoundMaxVolumeImpulseThreshold,
		float SoundMaxVolumeMultiplier,
		UPhysicalMaterial* SelfPhysicalMaterial,
		TWeakObjectPtr<class UPhysicalMaterial> HitPhysicalMaterial);

public:
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="PhysicsSound|Basic")
	FName TargetPrimitiveComponentName;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="PhysicsSound|Basic")
	bool bChangeBodySetupToGenerateHitEventOnActivate = true;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="PhysicsSound|Basic")
	bool bRestoreBodySetupOnDeactivate = true;
	
public:
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="PhysicsSound|Sound")
	TArray<FPhysicsSoundProviderStructure> DirectHitSounds;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="PhysicsSound|Sound")
	TArray<FPhysicsSoundProviderStructure> SlidingHitSound;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="PhysicsSound|Sound")
	TArray<FPhysicsSoundProviderStructure> RollingHitSound;
	
	// Master attenuation settings applied to all sounds played by this component - we didn't expose per-sound attenuation for simplicity (who wants that anyway?? + It's just a sample project for Joint anyway...)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="PhysicsSound|Sound")
	TSoftObjectPtr<USoundAttenuation> MasterAttenuation;
	
public:
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="PhysicsSound|Parameter")
	float DirectHitImpulseThreshold = 25.0f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="PhysicsSound|Parameter")
	float SlidingSpeedThreshold = 150.0f;
	
	// rotational velocity threshold (in cm/s) below which rolling sound hits will not be played
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="PhysicsSound|Parameter")
	float RollingAngularVelocityThreshold = 50.0f;
	
public:
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="PhysicsSound|Parameter")
	float DirectHitVolumeMaxImpulseThreshold = 450.0f;
	
	// 0 ~ this value linearly maps to 0.0 ~ 1.0 volume multiplier
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="PhysicsSound|Parameter")
	float DirectHitVolumeMultiplier = 1.2;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="PhysicsSound|Parameter")
	float SlidingVolumeMaxImpulseThreshold = 1000.0f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="PhysicsSound|Parameter")
	float SlidingVolumeMultiplier = 1.0;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="PhysicsSound|Parameter")
	float RollingVolumeMaxImpulseThreshold = 1000.0f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="PhysicsSound|Parameter")
	float RollingVolumeMultiplier = 0.6;
	
	// Body mass (in kg) above which the sound will play at full volume, linearly scales from 0 to this value
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="PhysicsSound|Parameter")
	float MassVolumeMaxImpulseThreshold = 50.0f;
	
	// Body mass (in kg) above which the sound will play at full volume, linearly scales from 0 to this value
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="PhysicsSound|Parameter")
	FFloatRange MassVolumeBound = FFloatRange(0.7f, 1.2f);
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="PhysicsSound|Parameter")
	FFloatRange MassPitchBound = FFloatRange(1.3f, 0.7f);
	
	
public:
	
	// cooldown time (in seconds) between playing hit sounds - to avoid sound spamming
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="PhysicsSound|Parameter")
	float DirectHitSoundCooldownTime = 0.1f;
	
	UPROPERTY()
	FTimerHandle DirectHitSoundCooldownTimerHandle;

private:
	
	UPROPERTY()
	bool bSavedNotifyRigidBodyCollision = false;

public:
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="PhysicsSound")
	bool bActivateOnBeginPlay = true;

public:
	
#if WITH_EDITOR
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
	
	// Sort the sound provider structures by impulse threshold in ascending order
	void SortSoundProviderStructuresByImpulseThreshold(TArray<FPhysicsSoundProviderStructure>& Array);
#endif
};
