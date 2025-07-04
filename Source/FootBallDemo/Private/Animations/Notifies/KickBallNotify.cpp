// Fill out your copyright notice in the Description page of Project Settings.


#include "Animations/Notifies/KickBallNotify.h"

#include "FootBallDemo/FootBallDemoCharacter.h"


void UKickBallNotify::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
                             const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);

	AFootBallDemoCharacter* FootBallDemoCharacter = Cast<AFootBallDemoCharacter>(MeshComp->GetOwner());

	if (FootBallDemoCharacter)
	{
		FootBallDemoCharacter->KickBall(700);
	}
}
