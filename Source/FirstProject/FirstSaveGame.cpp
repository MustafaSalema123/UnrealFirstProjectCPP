// Fill out your copyright notice in the Description page of Project Settings.

#include "FirstSaveGame.h"


UFirstSaveGame::UFirstSaveGame()
{
	SaveSlotName = TEXT("TestSaveSlot");   //SaveGameSlot bame
	UserIndex = 0;   //index

	CharacterStats.WeaponName = "";  //weapon Name
	CharacterStats.LevelName = ""; // Level name
}
