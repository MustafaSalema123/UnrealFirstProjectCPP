// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Main.h"

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "FirstSaveGame.generated.h"

/**
 * 
 */
UCLASS()
class FIRSTPROJECT_API UFirstSaveGame : public USaveGame
{
	GENERATED_BODY()
public:

public:

	UFirstSaveGame();

	UPROPERTY(VisibleAnywhere, Category = Basic)
	FCharacterStats CharacterStats;   // this structure in make in main_player


	UPROPERTY(VisibleAnywhere, Category = Basic)
	FString SaveSlotName;    //SaveSlotName

	UPROPERTY(VisibleAnywhere, Category = Basic)
	uint32 UserIndex;   //userIndex
	
};
