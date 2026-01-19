// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/JPSAbilitySystemComponent.h"
#include "GameFramework/PlayerController.h"
#include "JPSPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class JOINTPUBLICSHOWCASE_API AJPSPlayerController : public APlayerController
{
	GENERATED_BODY()
	
public:

	virtual void AcknowledgePossession(APawn* P) override;
	
	virtual void PostProcessInput(float DeltaTime, bool bGamePaused) override;
	
public:

	UJPSAbilitySystemComponent* GetJPSAbilitySystemComponent() const;
};
