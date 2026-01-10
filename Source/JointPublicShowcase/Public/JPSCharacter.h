// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/AbilitySets/JPSAbilitySet.h"
#include "GameFramework/Character.h"
#include "JPSCharacter.generated.h"

class UJPSAbilitySystemComponent;

UCLASS(config=Game, Blueprintable, BlueprintType)
class JOINTPUBLICSHOWCASE_API AJPSCharacter : public ACharacter
{
	GENERATED_BODY()

	/** Camera boom positioning the camera behind the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* CameraBoom;

	/** Follow camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* FollowCamera;
	
public:
	
	AJPSCharacter();

public:
	
	virtual void InitializeAbilitySet();
	
	UJPSAbilitySystemComponent* GetAbilitySystemComponent() const;
	
public:
	
	// GAS Implementation
	
	// Default ability set to grant to this character.
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "GASDocumentation|Abilities")
	UJPSAbilitySet* DefaultAbilitySet;
	
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "GASDocumentation|Abilities")
	class UJPSAbilitySystemComponent* AbilitySystemComponent;

public:
	
	virtual void BeginPlay() override;
	
	virtual void PossessedBy(AController* NewController) override;

protected:

	/** Called for forwards/backward input */
	UFUNCTION(BlueprintCallable, Category="Input")
	void MoveForward(float Value);

	/** Called for side to side input */
	UFUNCTION(BlueprintCallable, Category="Input")
	void MoveRight(float Value);

	/** 
	 * Called via input to turn at a given rate. 
	 * @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	 */
	UFUNCTION(BlueprintCallable, Category="Input")
	void TurnAtRate(float Rate);

	/**
	 * Called via input to turn look up/down at a given rate. 
	 * @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	 */
	UFUNCTION(BlueprintCallable, Category="Input")
	void LookUpAtRate(float Rate);
	
	/**
	 * Called via input to turn in place at a given rate for AI controller.
	 * @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	 */
	UFUNCTION(BlueprintCallable, Category="Input")
	void TurnInPlaceRate(float Rate);

public:
	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	/** Returns FollowCamera subobject **/
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }
	
public:

	UFUNCTION(BlueprintCallable, Category="GASDocumentation|Abilities")
	void AbilityInputPressed(FGameplayTag InputTag);
	
	UFUNCTION(BlueprintCallable, Category="GASDocumentation|Abilities")
	void AbilityInputReleased(FGameplayTag InputTag);
	
};

