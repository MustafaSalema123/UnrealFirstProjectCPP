// Fill out your copyright notice in the Description page of Project Settings.

#include "MainPlayerController.h"
#include "Blueprint/UserWidget.h"

AMainPlayerController::AMainPlayerController()
{
	bPauseMenuOpen = false;  //start bPasusemenu show true
}

void AMainPlayerController::BeginPlay()
{
	Super::BeginPlay();

	if (HUDOverlayAsset)  //subclass if HudOvrlayAssetis valid in inspector and not nullptr
	{
		//createWidget = (ownerObejct this  , UUserWidget)
		HUDOverlay = CreateWidget<UUserWidget>(this, HUDOverlayAsset); // creatr 
	}
	HUDOverlay->AddToViewport(); // add to scene or veiwport in game
	HUDOverlay->SetVisibility(ESlateVisibility::Visible); //  setvisiblity and make state visible shown in viewport

	if (WEnemyHealthBar) // subclass if WEnemyHealthBar valid in inspector and not nullptr
	{
		//createWidget = (ownerObejct this  , UUserWidget)
		EnemyHealthBar = CreateWidget<UUserWidget>(this, WEnemyHealthBar);
		if (EnemyHealthBar) // enemhyHealthbar create
		{
			EnemyHealthBar->AddToViewport();// add to scene or veiwport in game
			EnemyHealthBar->SetVisibility(ESlateVisibility::Hidden);//  setvisiblity and make state hidden not shown in scene in start
		}
		FVector2D Alignment(0.f, 0.f); //alinment in 0.0 ,0.0
		EnemyHealthBar->SetAlignmentInViewport(Alignment);  //set aligmentinViewport
	}

	if (WPauseMenu)// subclass if WPauseMenu valid in inspector and not nullptr
	{
		//createWidget = (ownerObejct this  , UUserWidget)
		PauseMenu = CreateWidget<UUserWidget>(this, WPauseMenu);
		if (PauseMenu) // PauseMenu create
		{
			PauseMenu->AddToViewport(); // add to scene or veiwport in game
			PauseMenu->SetVisibility(ESlateVisibility::Hidden);//  setvisiblity and make state hidden not shown in scene in start
		}
	}
}

void AMainPlayerController::DisplayEnemyHealthBar()
{
	if (EnemyHealthBar) // if EnemyHealthBar bar is valid
	{
		bEnemyHealthBarVisible = true; // healthbarvisible is true
		EnemyHealthBar->SetVisibility(ESlateVisibility::Visible);//  setvisiblity and make state visible shown in viewport
	}
}

void AMainPlayerController::RemoveEnemyHealthBar()
{
	if (EnemyHealthBar)
	{
		bEnemyHealthBarVisible = false;// healthbarvisible is false not shown
		EnemyHealthBar->SetVisibility(ESlateVisibility::Hidden);//  setvisiblity and make state hidden not shown in scene in start
	}
}

void AMainPlayerController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (EnemyHealthBar)
	{
		FVector2D PositionInViewport; // position in viewport
	 //	Convert a World Space 3D position into a 2D Screen Space position.
		//	* @return true if the world coordinate was successfully projected to the screen.

		ProjectWorldLocationToScreen(EnemyLocation, PositionInViewport);
		PositionInViewport.Y -= 85.f; //set position

		FVector2D SizeInViewport(300.f, 25.f); // size in veiwport

		/**
 * Sets the widgets position in the viewport.
 * @param Position The 2D position to set the widget to in the viewport.
 * @param bRemoveDPIScale If you've already calculated inverse DPI, set this to false.
 * Otherwise inverse DPI is applied to the position so that when the location is scaled
 * by DPI, it ends up in the expected position.
 */
		EnemyHealthBar->SetPositionInViewport(PositionInViewport);
		EnemyHealthBar->SetDesiredSizeInViewport(SizeInViewport);
	}
}

void AMainPlayerController::DisplayPauseMenu_Implementation()
{
	bPauseMenuOpen = true;
	bShowMouseCursor = true;
	//Data structure used to setup an input mode that allows the UI to respond to user input, and if the UI doesn't handle it player input / player controller gets a chance
	FInputModeGameAndUI InputModeGameAndUI;
	SetInputMode(InputModeGameAndUI);/** Setup an input mode. */ // show cursor mouse
}


void AMainPlayerController::RemovePauseMenu_Implementation()
{
	bPauseMenuOpen = false;
	bShowMouseCursor = false;

	GameModeOnly();  //not show cursor mouse
}

void AMainPlayerController::TogglePauseMenu()
{
	if (!bPauseMenuOpen) // toggel it
	{
		DisplayPauseMenu();
	}
	else
	{
		RemovePauseMenu();
	}
}

void AMainPlayerController::GameModeOnly()
{
	/** Whether the mouse down that causes capture should be consumed, and not passed to player input processing */
	FInputModeGameOnly InputModeGameOnly;  
	SetInputMode(InputModeGameOnly);
}