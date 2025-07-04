// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Ball/FootBall.h"
#include "Components/SphereComponent.h"
#include "GameFramework/Character.h"
#include "Logging/LogMacros.h"
#include "FootBallDemoCharacter.generated.h"

class USpringArmComponent;
class UCameraComponent;
class UInputMappingContext;
class UInputAction;
struct FInputActionValue;

DECLARE_LOG_CATEGORY_EXTERN(LogTemplateCharacter, Log, All);

UCLASS(config=Game)
class AFootBallDemoCharacter : public ACharacter
{
	GENERATED_BODY()

	/** Camera boom positioning the camera behind the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	USpringArmComponent* CameraBoom;

	/** Follow camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	UCameraComponent* FollowCamera;
	
	/** MappingContext */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputMappingContext* DefaultMappingContext;

	/** Jump Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* JumpAction;

	/** Move Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* MoveAction;

	/** Look Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* LookAction;

	/** Sprint Input Action**/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* Sprint;

public:
	AFootBallDemoCharacter();
	
	void KickBall(float force);
protected:

	/** Called for movement input */
	void Move(const FInputActionValue& Value);

	/** Called for looking input */
	void Look(const FInputActionValue& Value);

	void StartSprint();
	void StopSprint();

	UPROPERTY(EditAnywhere, Category="Dribble")
	TSubclassOf<AFootBall> BallClass;

	UPROPERTY()
	AFootBall* ControlledBall = nullptr;

	UPROPERTY(VisibleAnywhere, Category="Dribble")
	USceneComponent* BallHoldPoint;

	UPROPERTY(EditAnywhere, Category="Dribble")
	float DribbleDistance = 60.f;

	FVector2D MovementInput;

	bool bIsSprinting = false;

	bool bDribbleMode = true;

	FVector Velocity;

	UPROPERTY(EditAnywhere)
	float MaxSpeed = 420.f;

	UPROPERTY(EditAnywhere)
	float Acceleration = 2500.f;

	UPROPERTY(EditAnywhere)
	float Braking = 2000.f;

	UPROPERTY(EditAnywhere)
	float RotationSpeed = 6.f;


	void UpdateBallFollow(float DeltaTime);

protected:
	// APawn interface
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	
	// To add mapping context
	virtual void BeginPlay();

	virtual void Tick(float DeltaTime) override;

public:
	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	/** Returns FollowCamera subobject **/
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }
	UFUNCTION(BlueprintCallable, BlueprintPure)
	FORCEINLINE float GetCurrentSpeed() const { return Velocity.Size(); }
};



