// Fill out your copyright notice in the Description page of Project Settings.

#include "MainAnimInstance.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Main.h"


void UMainAnimInstance::NativeInitializeAnimation()
{
	if (Pawn == nullptr) // check if pawn ==  nullptr
	{
		Pawn = TryGetPawnOwner();// and tryget owner of pawn
		if (Pawn) // if pawn then we want to cast in main_Player pawn
		{
			Main = Cast<AMain>(Pawn);
		}
	}
}

void UMainAnimInstance::UpdateAnimationProperties() // called in animINstacne event graph
{
	if (Pawn == nullptr) // check if pawn ==  nullptr
	{
		Pawn = TryGetPawnOwner();   //// and tryget owner of pawn
	}

	if (Pawn) // if pawn valid
	{
		FVector Speed = Pawn->GetVelocity(); // check the pawn velocity from pawn class
		FVector LateralSpeed = FVector(Speed.X, Speed.Y, 0.f); // then check in x , y position speed
		MovementSpeed = LateralSpeed.Size(); //get size / magnitude of movementspeed

		bIsInAir = Pawn->GetMovementComponent()->IsFalling();  // isfalling 

		if (Main == nullptr) // main in called blueprint
		{
			Main = Cast<AMain>(Pawn);
		}
	}
}

