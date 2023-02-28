// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "MainPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class FIRSTPROJECT_API AMainPlayerController : public APlayerController
{
	GENERATED_BODY()
public:

	AMainPlayerController();

	/** Reference to the UMG asset in the editor */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Widgets")
	TSubclassOf<class UUserWidget> HUDOverlayAsset;
	
	/** Variable to hold the widget after creating it */ // its hold HUDOverlayAsset
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Widgets")
	UUserWidget* HUDOverlay;

	/** Reference to the UMG asset in the editor */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Widgets")
	TSubclassOf<UUserWidget> WEnemyHealthBar;

	/** Variable to hold the widget after creating it */ // its hold WEnemyHealthBar
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Widgets")
	UUserWidget* EnemyHealthBar;   
	/** Reference to the UMG asset in the editor */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Widgets")
	TSubclassOf<UUserWidget> WPauseMenu;

	/** Variable to hold the widget after creating it */ // its hold WPauseMenu
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Widgets")
	UUserWidget* PauseMenu;

	bool bEnemyHealthBarVisible;

	void DisplayEnemyHealthBar();// in mainScript this is use
	void RemoveEnemyHealthBar();

	UFUNCTION(BlueprintNativeEvent) // display pausemeny bar in screen
	void DisplayPauseMenu();

	UFUNCTION(BlueprintNativeEvent)// hide pausemeny bar in screen
	void RemovePauseMenu();

	UFUNCTION(BlueprintCallable)
	void TogglePauseMenu();   //togglePauseMenu like on and off

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Widgets")
	bool bPauseMenuOpen;   // bool show pauseMenu

	FVector EnemyLocation;     //Get Enemy location

	void GameModeOnly();     //GameModeonly no cursor

protected:
	virtual void BeginPlay() override;

	virtual void Tick(float DeltaTime) override;
	
};
