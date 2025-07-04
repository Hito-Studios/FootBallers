// Fill out your copyright notice in the Description page of Project Settings.


#include "Ball/FootBall.h"

// Sets default values
AFootBall::AFootBall()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;


	BallMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Ball"));
	SetRootComponent(BallMesh);
	BallMesh->SetSimulatePhysics(true);
	BallMesh->SetLinearDamping(0.2f);
	BallMesh->SetAngularDamping(0.5f);
	BallMesh->SetCollisionProfileName(TEXT("PhysicsActor"));

}

void AFootBall::KickBall(const FVector& Direction, float Force)
{
	if (BallMesh && !Direction.IsNearlyZero())
	{
		BallMesh->AddImpulse(Direction.GetSafeNormal() * Force, NAME_None, true);
	}
}

// Called when the game starts or when spawned
