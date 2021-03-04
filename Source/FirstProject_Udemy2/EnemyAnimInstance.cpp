// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyAnimInstance.h"
#include "Enemy.h"

void UEnemyAnimInstance::NativeInitializeAnimation()
{
	if (Pawn == nullptr)
	{
		Pawn = TryGetPawnOwner(); //get owner of this pawn instance
		if (Pawn)
		{
			Enemy = Cast<AEnemy>(Pawn);
		}
	}
}


void UEnemyAnimInstance::UpdateAnimationProperties()
{
	if (Pawn == nullptr)
	{
		Pawn = TryGetPawnOwner(); //get owner of this pawn instance
		if (Pawn)
		{
			FVector Speed = Pawn->GetVelocity();
			FVector LateralSpeed = FVector(Speed.X, Speed.Y, 0.f);
			MovementSpeed = LateralSpeed.Size();  //set movement speed to (Lateral Speed Vector converted to float)
			Enemy = Cast<AEnemy>(Pawn);
		}
	}
}
