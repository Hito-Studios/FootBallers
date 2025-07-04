// Copyright Epic Games, Inc. All Rights Reserved.

#include "FootBallDemoCharacter.h"
#include "Engine/LocalPlayer.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/Controller.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"
#include "Kismet/GameplayStatics.h"
#include "Physics/ImmediatePhysics/ImmediatePhysicsShared/ImmediatePhysicsCore.h"

DEFINE_LOG_CATEGORY(LogTemplateCharacter);

//////////////////////////////////////////////////////////////////////////
// AFootBallDemoCharacter

AFootBallDemoCharacter::AFootBallDemoCharacter()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);
		
	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Character moves in the direction of input...	
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f); // ...at this rotation rate

	// Note: For faster iteration times these variables, and many more, can be tweaked in the Character Blueprint
	// instead of recompiling to adjust them
	GetCharacterMovement()->JumpZVelocity = 700.f;
	GetCharacterMovement()->AirControl = 0.35f;
	GetCharacterMovement()->MaxWalkSpeed = 500.f;
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;
	GetCharacterMovement()->BrakingDecelerationFalling = 1500.0f;

	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 400.0f; // The camera follows at this distance behind the character	
	CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	// Note: The skeletal mesh and anim blueprint references on the Mesh component (inherited from Character) 
	// are set in the derived blueprint asset named ThirdPersonCharacter (to avoid direct content references in C++)

	BallHoldPoint = CreateDefaultSubobject<USceneComponent>("BallHoldPoint");
	BallHoldPoint->SetupAttachment(GetMesh());
	BallHoldPoint->SetRelativeLocation(FVector(-10, 0, -90)); 
	BallHoldPoint->SetRelativeRotation(FRotator::ZeroRotator);

	bUseControllerRotationYaw = false;
	GetCharacterMovement()->bOrientRotationToMovement = false;

}

void AFootBallDemoCharacter::KickBall(float force)
{
	if (!ControlledBall) return;

	bDribbleMode = false;

	UStaticMeshComponent* BallMesh = ControlledBall->BallMesh;
	BallMesh->SetSimulatePhysics(true);

	FVector KickDir = GetActorForwardVector();
	BallMesh->AddImpulse(KickDir * force, NAME_None, true);

	// Optionally, re-engage dribble after delay
	GetWorldTimerManager().SetTimerForNextTick([this]()
	{
		if (ControlledBall)
		{
			ControlledBall->BallMesh->SetSimulatePhysics(false);
			bDribbleMode = true;
		}
	});
}

void AFootBallDemoCharacter::BeginPlay()
{
	// Call the base class
	Super::BeginPlay();
	if (BallClass)
	{
		// Spawn the ball at the hold point
		FActorSpawnParameters P;
		P.Owner = this;
		ControlledBall = GetWorld()->SpawnActor<AFootBall>(BallClass, BallHoldPoint->GetComponentTransform(), P);

		UE_LOG(LogTemplateCharacter, Warning, TEXT("AFootBallDemoCharacter::BeginPlay"));
	}


	
	
}

void AFootBallDemoCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// INPUT → WORLD SPACE
	FVector InputDir = FVector(MovementInput.Y, MovementInput.X, 0.f);
	if (!InputDir.IsNearlyZero())
	{
		InputDir.Normalize();
	}

	// ACCELERATION/DECELERATION
	FVector TargetVel = InputDir * MaxSpeed;
	FVector DeltaVel = (TargetVel - Velocity).GetClampedToMaxSize(Acceleration * DeltaTime);
	Velocity += DeltaVel;

	if (InputDir.IsNearlyZero())
	{
		FVector BrakeVel = Velocity.GetClampedToMaxSize(Braking * DeltaTime);
		Velocity -= BrakeVel;
	}

	// MOVE CHARACTER
	AddActorWorldOffset(Velocity * DeltaTime, true);

	// ROTATE CHARACTER TO FACE MOVEMENT
	if (!Velocity.IsNearlyZero())
	{
		FRotator TargetRotation = Velocity.ToOrientationRotator();
		SetActorRotation(FMath::RInterpTo(GetActorRotation(), TargetRotation, DeltaTime, RotationSpeed));
	}

	UpdateBallFollow(DeltaTime);
	

	
}


void AFootBallDemoCharacter::UpdateBallFollow(float DeltaTime)
{
	if (!ControlledBall) return;

	UStaticMeshComponent* BallMesh = ControlledBall->BallMesh;
	if (!BallMesh) return;

	FVector BallLoc = ControlledBall->GetActorLocation();
	FVector TargetLoc = GetActorLocation() + GetActorForwardVector() * 60.f; //Change Multuplier to Change the how far the ball should be
	TargetLoc.Z = BallLoc.Z;

	FVector ToTarget = TargetLoc - BallLoc;
	float Distance = ToTarget.Size();

	if (Distance > 2.f)
	{
		FVector Direction = ToTarget.GetSafeNormal();
		float FollowStrength = FMath::Clamp(Distance * 20.f, 0.f, 900.f);
		FVector DesiredVel = Direction * FollowStrength;

		FVector CurrentVel = BallMesh->GetPhysicsLinearVelocity();
		FVector SmoothedVel = FMath::VInterpTo(CurrentVel, DesiredVel, DeltaTime, 25.f);
		BallMesh->SetPhysicsLinearVelocity(SmoothedVel);
	}
	else
	{
		BallMesh->SetPhysicsLinearVelocity(FVector::ZeroVector);
	}
	
}


//////////////////////////////////////////////////////////////////////////
// Input

void AFootBallDemoCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	// Add Input Mapping Context
	if (APlayerController* PlayerController = Cast<APlayerController>(GetController()))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
		}
	}
	
	// Set up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent)) {
		
		// Jumping
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &ACharacter::Jump);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);

		// Moving
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AFootBallDemoCharacter::Move);

		// Looking
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &AFootBallDemoCharacter::Look);

		EnhancedInputComponent->BindAction(Sprint, ETriggerEvent::Started, this, &AFootBallDemoCharacter::StartSprint);

		EnhancedInputComponent->BindAction(Sprint, ETriggerEvent::Completed, this, &AFootBallDemoCharacter::StopSprint);
	}
	else
	{
		UE_LOG(LogTemplateCharacter, Error, TEXT("'%s' Failed to find an Enhanced Input component! This template is built to use the Enhanced Input system. If you intend to use the legacy system, then you will need to update this C++ file."), *GetNameSafe(this));
	}
}

void AFootBallDemoCharacter::Move(const FInputActionValue& Value)
{
	FVector2D InputVec = Value.Get<FVector2D>();
	MovementInput = InputVec;
}

void AFootBallDemoCharacter::Look(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// add yaw and pitch input to controller
		AddControllerYawInput(LookAxisVector.X);
		AddControllerPitchInput(LookAxisVector.Y);
	}
}

void AFootBallDemoCharacter::StartSprint()
{
	bIsSprinting = true;

	GetCharacterMovement()->MaxWalkSpeed = 500;
}

void AFootBallDemoCharacter::StopSprint()
{
	bIsSprinting = false;
	
	GetCharacterMovement()->MaxWalkSpeed = 300;
}
