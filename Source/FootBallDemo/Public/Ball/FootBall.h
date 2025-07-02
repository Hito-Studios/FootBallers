// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "FootBall.generated.h"

UCLASS()
class FOOTBALLDEMO_API AFootBall : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AFootBall();

	void KickBall(const FVector& Direction, float Force);

protected:
	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* Ball;




};
