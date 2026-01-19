// Fill out your copyright notice in the Description page of Project Settings.


#include "JPSPlayerController.h"

#include "AbilitySystemComponent.h"
#include "JPSCharacter.h"
#include "Abilities/JPSAbilitySystemComponent.h"
#include "GameFramework/PlayerState.h"


void AJPSPlayerController::AcknowledgePossession(APawn* P)
{
	Super::AcknowledgePossession(P);

	if (AJPSCharacter* CharacterBase = Cast<AJPSCharacter>(P))
	{
		CharacterBase->GetAbilitySystemComponent()->InitAbilityActorInfo(CharacterBase, CharacterBase);
	}

	//...
}

void AJPSPlayerController::PostProcessInput(const float DeltaTime, const bool bGamePaused)
{
	if (UJPSAbilitySystemComponent* LyraASC = GetJPSAbilitySystemComponent())
	{
		LyraASC->ProcessAbilityInput(DeltaTime, bGamePaused);
	}

	Super::PostProcessInput(DeltaTime, bGamePaused);
}

UJPSAbilitySystemComponent* AJPSPlayerController::GetJPSAbilitySystemComponent() const
{
	if (APawn* ControlledPawn = GetPawn())
	{
		if (AJPSCharacter* CharacterBase = Cast<AJPSCharacter>(ControlledPawn))
		{
			return CharacterBase->GetAbilitySystemComponent();
		}
	}

	return nullptr;
}