// Fill out your copyright notice in the Description page of Project Settings.

#include "EnemyAnimInstance.h"
#include "Enemy.h"

void UEnemyAnimInstance::NativeInitializeAnimation()
{
	if (Pawn == nullptr) // check if pawn ==  nullptr
	{
		Pawn = TryGetPawnOwner(); // and tryget owner of pawn
		if (Pawn)  // if pawn then we want to cast in enemy pawn
		{
			Enemy = Cast<AEnemy>(Pawn);
		}
	}
}

void UEnemyAnimInstance::UpdateAnimationProperties() // called in bluePrint make update the animation
{
	if (Pawn == nullptr) // check if pawn ==  nullptr
	{
		Pawn = TryGetPawnOwner();  //// and tryget owner of pawn
		if (Pawn)
		{
			Enemy = Cast<AEnemy>(Pawn);// if pawn then we want to cast in enemy pawn
		}
	}

	if (Pawn) // if pawn valid
	{
		FVector Speed = Pawn->GetVelocity();  // check the pawn velocity from pawn class
		FVector LateralSpeed = FVector(Speed.X, Speed.Y, 0.f);  // then check in x , y position speed
		MovementSpeed = LateralSpeed.Size(); //get size / magnitude of movementspeed
	}
}
