// Copyright Epic Games, Inc. All Rights Reserved.


#include "JPSCharacter.h"

#include "AbilitySystemComponent.h"
#include "AIController.h"
#include "Abilities/JPSAbilitySystemComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "GameFramework/SpringArmComponent.h"

//////////////////////////////////////////////////////////////////////////
// AJPSCharacter

AJPSCharacter::AJPSCharacter()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = true;
	bUseControllerRotationRoll = false;

	// Configure character movement
	UCharacterMovementComponent* MovementComp = CastChecked<UCharacterMovementComponent>(ACharacter::GetMovementComponent());
	
	// Note: For faster iteration times these variables, and many more, can be tweaked in the Character Blueprint
	// instead of recompiling to adjust them
	MovementComp->JumpZVelocity = 400.f;
	MovementComp->AirControl = 0.35f;
	MovementComp->MaxWalkSpeed = 500.f;
	MovementComp->MinAnalogWalkSpeed = 20.f;
	MovementComp->GroundFriction = 1.f;
	MovementComp->BrakingDecelerationWalking = 3.f;
	MovementComp->GravityScale = 1.2f;
	
	MovementComp->bUseControllerDesiredRotation = false;
	MovementComp->bOrientRotationToMovement = false;
	MovementComp->RotationRate = FRotator(0.0f, 720.0f, 0.0f);
	MovementComp->bAllowPhysicsRotationDuringAnimRootMotion = false;
	MovementComp->GetNavAgentPropertiesRef().bCanCrouch = true;
	MovementComp->bCanWalkOffLedgesWhenCrouching = true;
	MovementComp->SetCrouchedHalfHeight(65.0f);

	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 400.0f; // The camera follows at this distance behind the character	
	CameraBoom->bEnableCameraLag = true;
	CameraBoom->CameraLagSpeed = 500.0f;
	CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	// Note: The skeletal mesh and anim blueprint references on the Mesh component (inherited from Character) 
	// are set in the derived blueprint asset named ThirdPersonCharacter (to avoid direct content references in C++)
	
	AbilitySystemComponent = CreateDefaultSubobject<UJPSAbilitySystemComponent>(TEXT("AbilitySystemComponent"));
	AbilitySystemComponent->SetIsReplicated(true);
	
	
}

void AJPSCharacter::InitializeAbilitySet()
{
	if (AbilitySystemComponent && DefaultAbilitySet)
	{
		DefaultAbilitySet->GiveToAbilitySystem(AbilitySystemComponent, nullptr, this);
	}
}


UJPSAbilitySystemComponent* AJPSCharacter::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

void AJPSCharacter::BeginPlay()
{
	Super::BeginPlay();
}

void AJPSCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);
	
	if (AbilitySystemComponent)
	{
		AbilitySystemComponent->InitAbilityActorInfo(this, this);
		InitializeAbilitySet();
	}
	
	SetOwner(NewController);
}

void AJPSCharacter::MoveForward(float Value)
{
	if ((Controller != nullptr) && (Value != 0.0f))
	{
		// find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		AddMovementInput(Direction, Value);
	}
}

void AJPSCharacter::MoveRight(float Value)
{
	if ( (Controller != nullptr) && (Value != 0.0f) )
	{
		// find out which way is right
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);
	
		// get right vector 
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		// add movement in that direction
		AddMovementInput(Direction, Value);
	}
}

void AJPSCharacter::TurnAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerYawInput(Rate * 50 * GetWorld()->GetDeltaSeconds());
}

void AJPSCharacter::LookUpAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerPitchInput(Rate * 50 * GetWorld()->GetDeltaSeconds());
}

void AJPSCharacter::TurnInPlaceRate(float Rate)
{
	if (AAIController* AICon = Cast<AAIController>(Controller))
	{
		const float DeltaYaw = Rate * GetWorld()->GetDeltaSeconds();

		const FVector Forward = GetActorForwardVector();
		const FVector Rotated = Forward.RotateAngleAxis(DeltaYaw, FVector::UpVector);

		AICon->SetFocalPoint(GetActorLocation() + Rotated * 1000.f);
	}
}

void AJPSCharacter::AbilityInputPressed(FGameplayTag InputTag)
{
	GetAbilitySystemComponent()->AbilityInputTagPressed(InputTag);
}

void AJPSCharacter::AbilityInputReleased(FGameplayTag InputTag)
{
	GetAbilitySystemComponent()->AbilityInputTagReleased(InputTag);
}
