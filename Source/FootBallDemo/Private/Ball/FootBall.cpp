// Fill out your copyright notice in the Description page of Project Settings.


#include "Ball/FootBall.h"

// Sets default values
AFootBall::AFootBall()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;


	Ball = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Ball"));
	SetRootComponent(Ball);
	Ball->SetSimulatePhysics(true);
	Ball->SetLinearDamping(0.2f);
	Ball->SetAngularDamping(0.5f);
	Ball->SetCollisionProfileName(TEXT("PhysicsActor"));

}

void AFootBall::KickBall(const FVector& Direction, float Force)
{
	if (Ball)
	{
		Ball->AddImpulse(Direction * Force, NAME_None, true);
	}
}

// Called when the game starts or when spawned
