// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "WeaponContainer.h"

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Main.generated.h"

USTRUCT(BlueprintType)
struct FCharacterStats
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "SavedData") //health that reduce in game
	float Health;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "SavedData")  //in game our max health
	float MaxHealth;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "SavedData")//stamin that reduce in game
	float Stamina;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "SavedData") //in game our max stamina
	float MaxStamina;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "SavedData") //for saving data of location in game
	FVector Location;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "SavedData")//for saving data of Rotation in game
	FRotator Rotation;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "SavedData") //Our  coin
	int32 Coins;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "SavedData") //Weapon name that is used
	FString WeaponName;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "SavedData") // weapon particle of bool if true is show else false is not Show
	bool bWeaponParticles;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "SavedData") //save level name
	FString LevelName;
};

UENUM(BlueprintType)    //Enum of movement stats
enum class EMovementStatus : uint8
{
	EMS_Normal UMETA(DisplayName = "Normal"),
	EMS_Sprinting UMETA(DisplayName = "Sprinting"),
	EMS_Dead UMETA(DisplayName = "Dead"),

	EMS_MAX UMETA(DisplayName = "DefaultMAX")
};

UENUM(BlueprintType)  //Stamina stats
enum class EStaminaStatus :uint8
{
	ESS_Normal UMETA(DisplayName = "Normal"),
	ESS_BelowMinimum UMETA(DisplayName = "BelowMinimum"),
	ESS_Exhausted UMETA(DisplayName = "Exhausted"),
	ESS_ExhaustedRecovering UMETA(DisplayName = "ExhaustedRecovering"),

	ESS_MAX UMETA(DisplayName = "DefaultMax")

};

UCLASS()
class FIRSTPROJECT_API AMain : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AMain();

	FCharacterStats CharacterStats;    //check after

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat")
	bool bHasCombatTarget;                     //use line line numbr 614     // there is enemy target in near by me

	FORCEINLINE void SetHasCombatTarget(bool HasTarget) { bHasCombatTarget = HasTarget; } // this is use in EnemyScript beacasue we want to false have has target line cPP 120

	UPROPERTY(BlueprintReadWrite, VisibleAnywhere, Category = "Combat")
	FVector CombatTargetLocation;     // the combat Target location of enemy , this is iniatial in 226 line

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Controller")
	class AMainPlayerController* MainPlayerController;        //userWidth _ Ui that show on Screen Module = "UMG"    

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	class UParticleSystem* HitParticles;               //hit particle to enemy

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	class USoundCue* HitSound;   // hit shound to enemy

	TArray<FVector> PickupLocations;
	
	UFUNCTION(BlueprintCallable)
	void ShowPickupLocations();    //Debug show pick_UP location in game and make sphere 

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Enums")
	EMovementStatus MovementStatus;   //enum of Movemnent status

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Enums")
	EStaminaStatus StaminaStatus;   //Enum of stamina status like low , medium , high

	FORCEINLINE void SetStaminaStatus(EStaminaStatus Status) { StaminaStatus = Status; }  //set stamina status on game

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float StaminaDrainRate;         // stamina down speed minus show stamina

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float MinSprintStamina;   //minimum stamina sprint in game

	float InterpSpeed;   //Interspeed that like lerp 

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Combat")
	bool bInterpToEnemy;    // interp to enemy usinh g lerp
	void SetInterpToEnemy(bool Interp);

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat")
	class AEnemy* CombatTarget;     //Enemy target for // use in enemy Script line number 116

	FORCEINLINE void SetCombatTarget(AEnemy* Target) { CombatTarget = Target; } //Set it to Combat Target and this is used in 613 line that show bar  // use in enemy Script line number 118

	FRotator GetLookAtRotationYaw(FVector Target);  //get look at enemy

	/** Set movement status and running speed */
	void SetMovementStatus(EMovementStatus Status);  

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Running")
	float RunningSpeed;   //Running speed of player

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Running")
	float SprintingSpeed;  //Sprinting speed of player

	bool bShiftKeyDown;   //Shift key down bool  is is true than we sprint else sprint false

	/** Pressed down to enable sprinting */
	void ShiftKeyDown();

	/** Released to stop sprinting */
	void ShiftKeyUp();

	/** Camera boom positioning the camera behind the player */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* CameraBoom;   //Spring arm in book defination

	/** Follow Camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* FollowCamera;

	/** Base turn rates to scale turning functions for the camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
	float BaseTurnRate;         
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
	float BaseLookUpRate;

	/**
	/*
	/* Player Stats
	/*
	*/

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Player Stats")
	float MaxHealth;   // for game maxHealth

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player Stats")
	float Health;  // for  reduce health 

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Player Stats")
	float MaxStamina;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player Stats")
	float Stamina;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player Stats")
	int32 Coins;

	void DecrementHealth(float Amount);    //Decremnt health after overlap with enemy and explosion

	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const & DamageEvent, class AController * EventInstigator, AActor * DamageCauser) override;
	//take damage is Apply damage to this actor. // this in unreal version
	//DamageAmount -How much damage to apply
	//DamageEvent - Data package that fully describes the damage received.
	//EventInstigator - The Controller responsible for the damage
	//DamageCauser - The Actor that directly caused the damage (e.g. the projectile that exploded, the rock that landed on you)

	UFUNCTION(BlueprintCallable)
	void IncrementCoins(int32 Amount);  //Increment coin

	UFUNCTION(BlueprintCallable)
	void IncrementHealth(float Amount); // incremnet health

	void Die();  //Dia a characte

	virtual void Jump() override;  //jump override by unreal engine

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	/** Called for forwards/backwards input */
	void MoveForward(float Value);

	/** Called for side to side input */
	void MoveRight(float Value);

	bool bMovingForward;
	bool bMovingRight;

	/** Called via input to turn at a given rate
	* @param Rate This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	*/
	void TurnAtRate(float Rate);

	/** Called via input to look up/down at a given rate
	* @param Rate This is a normalized rate, i.e. 1.0 means 100% of desired look up/down rate
	*/
	void LookUpAtRate(float Rate);

	/** Called for mouse input add yaw rotation */
	void Turn(float Value);

	/** Called for mouse input add pitch rotation */
	void LookUp(float Value);

	bool bLMBDown;
	void LMBDown();  //For taking weapon
	void LMBUp();

	bool bESCDown;   //For Quit
	void ESCDown();
	void ESCUp();

	bool AllowInput(float Value);

	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = Items)
	class AWeapon* EquippedWeapon;    //Weapon Equipped

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Items)
	class AItem* ActiveOverlappingItem;    //Current ActiveOverlappingItem this is use in 355

	void SetEquippedWeapon(AWeapon* WeaponToSet);  //SetEquippedWeapon

	UFUNCTION(BlueprintCallable)
	AWeapon* GetEquippedWeapon() { return EquippedWeapon; }

	FORCEINLINE void SetActiveOverlappingItem(AItem* Item) { ActiveOverlappingItem = Item; } //use in Weapon.cpp  script that SetActivePverlappintItem
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Anims")
	bool bAttacking;

	void Attack();

	UFUNCTION(BlueprintCallable)
	void AttackEnd();  //Called in blueprint

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Anims")
	class UAnimMontage* CombatMontage;   //Combat animation

	UFUNCTION(BlueprintCallable)
	void PlaySwingSound();

	UFUNCTION(BlueprintCallable)
	void DeathEnd();

	void UpdateCombatTarget();  //use in enemy Script Target becacase we want to show it line 124

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	TSubclassOf<AEnemy> EnemyFilter;    //EnemyFiltera use in 579 that enemy overlap with us

	void SwitchLevel(FName LevelName);

	UFUNCTION(BlueprintCallable)
	void SaveGame();  // Save game

	UFUNCTION(BlueprintCallable)
	void LoadGame(bool LoadPosition);

	UFUNCTION(BlueprintCallable)
	void LoadGameNoSwitch();   //Load game no switch

	//UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
	//TSubclassOf<UWeaponContainer> WeaponContainer;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
	TSubclassOf<class AWeaponContainerActor> WeaponContainer;  // WeaponContainer that save a data of current weapon and load after the same weapon
};
