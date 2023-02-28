// Fill out your copyright notice in the Description page of Project Settings.

#include "Main.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/PlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Weapon.h"
#include "Components/SkeletalMeshComponent.h"
#include "Animation/AnimInstance.h"
#include "Sound/SoundCue.h"
#include "Kismet/KismetMathLibrary.h"
#include "Enemy.h"
#include "MainPlayerController.h"
#include "FirstSaveGame.h"
#include "Critter.h"
#include "WeaponContainerActor.h"

// Sets default values
AMain::AMain()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Create Camera Boom (pulls towards the player if there's a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));  //make a Camera Boom
	CameraBoom->SetupAttachment(GetRootComponent());
	CameraBoom->TargetArmLength = 600.f; // Camera follows at this distance
	CameraBoom->bUsePawnControlRotation = true; // Rotate arm based on controller

	// Set size for collision capsule
	GetCapsuleComponent()->SetCapsuleSize(48.f, 105.f);

	// Create Follow Camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	// Attach the camera to the end of the boom and let the boom adjust to match
	// the controller orientation
	FollowCamera->bUsePawnControlRotation = false;
	// Set our turn rates for input
	BaseTurnRate = 65.f;
	BaseLookUpRate = 65.f;

	// Don't rotate when the controller rotates.
	// Let that just affect the camera.
	bUseControllerRotationYaw = false;
	bUseControllerRotationPitch = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	//GetCharacterMovement() is method that contain Contoller data

	GetCharacterMovement()->bOrientRotationToMovement = true; // Character moves in the direction of input...
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 540.f, 0.0f); // ...at this rotation rate
	GetCharacterMovement()->JumpZVelocity = 650.f;
	GetCharacterMovement()->AirControl = 0.2f;

	MaxHealth = 100.f;
	Health = 65.f;
	MaxStamina = 150.f;
	Stamina = 120.f;
	Coins = 0;

	RunningSpeed = 650.f;
	SprintingSpeed = 950.f;

	bShiftKeyDown = false;
	bLMBDown = false;

	//Initialize Enums
	MovementStatus = EMovementStatus::EMS_Normal;
	StaminaStatus = EStaminaStatus::ESS_Normal;

	StaminaDrainRate = 25.f;
	MinSprintStamina = 50.f;

	InterpSpeed = 15.f;
	bInterpToEnemy = false;

	bHasCombatTarget = false;

	bMovingForward = false;
	bMovingRight = false;

	bESCDown = false;
}

// Called when the game starts or when spawned
void AMain::BeginPlay()
{
	Super::BeginPlay();
	
	MainPlayerController = Cast<AMainPlayerController>(GetController()); // in beagan we take ref of AmainPlayerController Script

	LoadGameNoSwitch();
	if (MainPlayerController)
	{
		/** Data structure used to setup an input mode that allows only the player input / player controller to respond to user input. */
		MainPlayerController->GameModeOnly();
	}
}

// Called every frame
void AMain::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (MovementStatus == EMovementStatus::EMS_Dead) return; // if playeer is dead then we show return all movement 

	float DeltaStamina = StaminaDrainRate * DeltaTime;   //decreaing stamina

	switch (StaminaStatus)
	{
	case EStaminaStatus::ESS_Normal:
		if (bShiftKeyDown)
		{
			if (Stamina - DeltaStamina <= MinSprintStamina)
			{
				SetStaminaStatus(EStaminaStatus::ESS_BelowMinimum);
				Stamina -= DeltaStamina;
			}
			else
			{
				Stamina -= DeltaStamina;
			}
			if (bMovingForward || bMovingRight)
			{
				SetMovementStatus(EMovementStatus::EMS_Sprinting);
			}
			else
			{
				SetMovementStatus(EMovementStatus::EMS_Normal);
			}
		}
		else // Shift key up
		{
			if (Stamina + DeltaStamina >= MaxStamina)
			{
				Stamina = MaxStamina;
			}
			else
			{
				Stamina += DeltaStamina;
			}
			SetMovementStatus(EMovementStatus::EMS_Normal);
		}
		break;
	case EStaminaStatus::ESS_BelowMinimum:
		if (bShiftKeyDown)
		{
			if (Stamina - DeltaStamina <= 0.f)
			{
				SetStaminaStatus(EStaminaStatus::ESS_Exhausted);
				Stamina = 0;
				SetMovementStatus(EMovementStatus::EMS_Normal);
			}
			else
			{
				Stamina -= DeltaStamina;
				if (bMovingForward || bMovingRight)
				{
					SetMovementStatus(EMovementStatus::EMS_Sprinting);
				}
				else
				{
					SetMovementStatus(EMovementStatus::EMS_Normal);
				}
			}
		}
		else // Shift key up
		{
			if (Stamina + DeltaStamina >= MinSprintStamina)
			{
				SetStaminaStatus(EStaminaStatus::ESS_Normal);
				Stamina += DeltaStamina;
			}
			else
			{
				Stamina += DeltaStamina;
			}
			SetMovementStatus(EMovementStatus::EMS_Normal);
		}
		break;
	case EStaminaStatus::ESS_Exhausted:
		if (bShiftKeyDown)
		{
			Stamina = 0.f;
		}
		else // Shift key up
		{
			SetStaminaStatus(EStaminaStatus::ESS_ExhaustedRecovering);
			Stamina += DeltaStamina;
		}
		SetMovementStatus(EMovementStatus::EMS_Normal);
		break;
	case EStaminaStatus::ESS_ExhaustedRecovering:
		if (Stamina + DeltaStamina >= MinSprintStamina)
		{
			SetStaminaStatus(EStaminaStatus::ESS_Normal);
			Stamina += DeltaStamina;
		}
		else
		{
			Stamina += DeltaStamina;
		}
		SetMovementStatus(EMovementStatus::EMS_Normal);
		break;
	default:
		;
	}   //Long Switch Stamin

	if (bInterpToEnemy && CombatTarget)  //if bInterpToEnemy = true and CombatTarget is valid in inspector than its become true
	{
		FRotator LookAtYaw = GetLookAtRotationYaw(CombatTarget->GetActorLocation());   // player lookAtYaw in game their is method that we want to make and witite in code //this meah 
		//we are if combatTarget is valid in over range our directiom directly to enemy
		//interp (getActorLocation()- Current ,  , to enmeyLocation  - target , deltatime and interpSpeed = 15.0f and use RInterpTo();
		
		FRotator InterpRotation = FMath::RInterpTo(GetActorRotation(), LookAtYaw, DeltaTime, InterpSpeed); // Interp(Lerp to player)
		
		SetActorRotation(InterpRotation); //and After setActorlocation using InterpLocatioj
	}

	if (CombatTarget) // CombatTargetlocation
	{
		CombatTargetLocation = CombatTarget->GetActorLocation(); // and get actorLocation if enemy
		if (MainPlayerController)
		{
			MainPlayerController->EnemyLocation = CombatTargetLocation; // tyhis show in mainPlayerController in EnemyLocation to FVector of CombatTargetLOcation
		}
	}
}

FRotator AMain::GetLookAtRotationYaw(FVector Target)
{
  //	UKismetMathLibrary::FindLookAtRotation(current or start  ,target) this is find the rotation in Y Axis
	FRotator LookAtRotation = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), Target);
	FRotator LookAtRotationYaw(0.f, LookAtRotation.Yaw, 0.f); // we can initial like FRotator LookAtRotationYaw = FRotator(0 , LookAtRotation.Yaw , 0.0);
	return LookAtRotationYaw; // return this and use in method return
}

// Called to bind functionality to input
void AMain::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	check(PlayerInputComponent);

	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &AMain::Jump); 
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);

	PlayerInputComponent->BindAction("ESC", IE_Pressed, this, &AMain::ESCDown);
	PlayerInputComponent->BindAction("ESC", IE_Released, this, &AMain::ESCUp);

	PlayerInputComponent->BindAction("Sprint", IE_Pressed, this, &AMain::ShiftKeyDown);
	PlayerInputComponent->BindAction("Sprint", IE_Released, this, &AMain::ShiftKeyUp);
	
	PlayerInputComponent->BindAction("LMB", IE_Pressed, this, &AMain::LMBDown); // For weapon
	PlayerInputComponent->BindAction("LMB", IE_Released, this, &AMain::LMBUp);

	PlayerInputComponent->BindAxis("MoveForward", this, &AMain::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &AMain::MoveRight);

	PlayerInputComponent->BindAxis("Turn", this, &AMain::Turn);  //Mouse
	PlayerInputComponent->BindAxis("LookUp", this, &AMain::LookUp);
	PlayerInputComponent->BindAxis("TurnRate", this, &AMain::TurnAtRate);
	PlayerInputComponent->BindAxis("LookUpRate", this, &AMain::LookUpAtRate);

}

bool AMain::AllowInput(float Value)  //Main Line
{
	return (Controller != nullptr) &&    //Controller is not nullptr , and value not equal to not zero , and his not attacking , movemnnet Staus is not stats to dead , pauseUi is not shown
		(Value != 0.0f) && 
		(!bAttacking) &&
		(MovementStatus != EMovementStatus::EMS_Dead) &&
		!MainPlayerController->bPauseMenuOpen;
}

void AMain::Turn(float Value)
{
	if (AllowInput(Value))  //bool of lIne 272
	{
		AddControllerYawInput(Value);   //Turn YawInout  //Right to left
	}
}

void AMain::LookUp(float Value)
{
	if (AllowInput(Value))
	{
		AddControllerPitchInput(Value);  //Look up pitch Input // up to down
	}
}

void AMain::MoveForward(float Value)
{
	bMovingForward = false;    //Moving forward is false
	if (AllowInput(Value))
	{
		// find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();  //movement rotatioin amnd GetControlRotaion
		const FRotator YawRotation(0.f, Rotation.Yaw, 0.f);   //Yaw rotation in right to left

		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X); //x ditection
		AddMovementInput(Direction, Value);

		bMovingForward = true;
	}
}


void AMain::MoveRight(float Value)
{
	bMovingRight = false;
	if (AllowInput(Value))
	{
		// find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0.f, Rotation.Yaw, 0.f);   //Mive Right

		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		AddMovementInput(Direction, Value);

		bMovingRight = true;
	}
}

void AMain::TurnAtRate(float Rate)
{
	if (AllowInput(Rate))
	{
		float a = Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds(); // this is Turn Rate of Mouse 
		float b = FMath::Clamp(a, -1.0f, 1.0f);
		AddControllerYawInput(Rate);
	}
}


void AMain::LookUpAtRate(float Rate)
{
	if (AllowInput(Rate))
	{
		AddControllerPitchInput(Rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds()); //Look At calculation
	}
}

void AMain::LMBDown()
{
	bLMBDown = true;   // If right Mouse Button down pressed The button lmd is true

	if (MovementStatus == EMovementStatus::EMS_Dead) return;  // if dead than return this methid and not work down 

	if (ActiveOverlappingItem)                  // if any item is overlap(Collide) with over player
	{
		AWeapon* Weapon = Cast<AWeapon>(ActiveOverlappingItem); //and the item is is cast with Weapon 
		if (Weapon)  // if weapon is overlap with player
		{
			Weapon->Equip(this);   //Equip the weapon in hand socket
			SetActiveOverlappingItem(nullptr);   //After setActiveOvverlappingIem = nullPtr beacause we not want after equped it
		}
	}
	else if (EquippedWeapon)   // and if equped weapon that we are Attack to enemy nad this is weapon class nad if we equipped theh weapon than up condition is not work
	{
		Attack();
	}
}

void AMain::LMBUp()
{
	bLMBDown = false;  //Mouse Up is false after released
}

void AMain::ESCDown()
{
	bESCDown = true;  // If we pressed Quit button down than is check the MianPLauerConttrolelr Bp

	if (MainPlayerController)
	{
		MainPlayerController->TogglePauseMenu(); // toggel the pause menu UI
	}
}

void AMain::ESCUp()
{
	bESCDown = false; //off the pause menu
}

void AMain::DecrementHealth(float Amount)
{
	if (Health - Amount <= 0.f) // decremnt health after collide with enemy or explosion this is used theri Script
	{
		Health -= Amount;
		Die();
	}
	else
	{
		Health -= Amount;
	}
	/*Health -= Amount;  //Use this also
	if (Health <= 0) {
		Die();
	}*/
}

void AMain::IncrementCoins(int32 Amount)
{
	Coins += Amount; // increment Coin
}

void AMain::IncrementHealth(float Amount)
{
	if (Health + Amount >= MaxHealth)
	{
		Health = MaxHealth;
	}
	else
	{
		Health += Amount;
	}
	//Health += Amount;  //use this also
	//if(Health >= MaxHealth)
	//{
	//	Health = MaxHealth;
	//}
}

void AMain::Die()
{
	if (MovementStatus == EMovementStatus::EMS_Dead) return;  // if we die than retuen this becasue we not want extra aniamtion stop Animatioon
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();  //get animInsTance for  over mesh that is show in Bp na d getMesh()-> get AnimInstance //#include "Animation/AnimInstance.h"
	if (AnimInstance && CombatMontage)  // if Animinstacne is valid and combatMonatge like have animation
	{
		AnimInstance->Montage_Play(CombatMontage, 1.0f);  //Monatge play
		AnimInstance->Montage_JumpToSection(FName("Death")); //And Direct differct section that we made in one montage and create many animation in
	}
	SetMovementStatus(EMovementStatus::EMS_Dead);  //SetMovementStatus = dead that after not play any thing that we use in many conditions
}

void AMain::Jump()
{
	if (MovementStatus != EMovementStatus::EMS_Dead)
	{
		Super::Jump();  // if not dead then Jump using and get Super::Jump override
	}
}

void AMain::DeathEnd()
{
	GetMesh()->bPauseAnims = true; //after death we want to pause anim and noSkelotonUpdate in game
	GetMesh()->bNoSkeletonUpdate = true;
}

void AMain::SetMovementStatus(EMovementStatus Status)
{
	MovementStatus = Status;    //SetMovemntStatis;
	if (MovementStatus == EMovementStatus::EMS_Sprinting) //If sprintif that increase speed
	{
		GetCharacterMovement()->MaxWalkSpeed = SprintingSpeed;
	}
	else
	{
		GetCharacterMovement()->MaxWalkSpeed = RunningSpeed; //i fnot sprinting than decrease Speed
	}
}


void AMain::ShiftKeyDown()
{
	bShiftKeyDown = true;  //Shift pressed when we want to dash
}


void AMain::ShiftKeyUp()
{
	bShiftKeyDown = false; //shift released when we want to not dash
}

void AMain::ShowPickupLocations()
{
	for (auto Location : PickupLocations)  //debug all location
	{
		UKismetSystemLibrary::DrawDebugSphere(this, Location, 25.f, 8, FLinearColor::Green, 10.f, 0.5f);
	}
}

void AMain::SetEquippedWeapon(AWeapon* WeaponToSet)
{
	if (EquippedWeapon)   //SetWquipedWeapon // check after
	{
		EquippedWeapon->Destroy();
	}

	EquippedWeapon = WeaponToSet;

}

void AMain::Attack()
{
	if (!bAttacking && MovementStatus != EMovementStatus::EMS_Dead) // If bAttacking is false and  stats is not dead
	{
		bAttacking = true;    //Atacking is true because we not want animation in loop 
		SetInterpToEnemy(true);   //SetINterpToenmey to rotate to enemy ist mean we are directly direction to enemy

		UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance(); //Get animInstacve from over getMesh Compononet
		if (AnimInstance && CombatMontage)
		{
			int32 Section = FMath::RandRange(0, 1);  //Randomlu play animation
			switch (Section)
			{
			case 0:
				AnimInstance->Montage_Play(CombatMontage, 2.2f);
				AnimInstance->Montage_JumpToSection(FName("Attack_1"), CombatMontage);
				break;
			case 1:
				AnimInstance->Montage_Play(CombatMontage, 1.8f);
				AnimInstance->Montage_JumpToSection(FName("Attack_2"), CombatMontage);
				break;

			default:
				;
			}
		}
	}
}

void AMain::AttackEnd()
{
	bAttacking = false;  // this is attack end that we are called in BkuePrint its maek nice transition 
	SetInterpToEnemy(false);
	if (bLMBDown)
	{
		Attack();
	}
}

void AMain::PlaySwingSound() //Inthis is PlaySwingSound end that we are called in BkuePrint its maek nice transition 
{
	if (EquippedWeapon->SwingSound)
	{
		UGameplayStatics::PlaySound2D(this, EquippedWeapon->SwingSound);
	}
}

void AMain::SetInterpToEnemy(bool Interp) //set using method
{
	bInterpToEnemy = Interp;
}

float AMain::TakeDamage(float DamageAmount, struct FDamageEvent const & DamageEvent, class AController * EventInstigator, AActor * DamageCauser)
{
	//take damage from enemy or explosion
	if (Health - DamageAmount <= 0.f)
	{
		Health -= DamageAmount;
		Die();
		if (DamageCauser) //the Actor that directly caused the damage 
		{
			AEnemy* Enemy = Cast<AEnemy>(DamageCauser);  //If enemy casued over damage
			if (Enemy)
			{
				Enemy->bHasValidTarget = false; // their no valid target
			}
		}
	}
	else
	{
		Health -= DamageAmount;
	}

	return DamageAmount;
}

void AMain::UpdateCombatTarget() // apdate target of enemy this is use in EnemyScript
{
	TArray<AActor*> OverlappingActors;   // overlapping actor that enemy overlap with player
	GetOverlappingActors(OverlappingActors, EnemyFilter);

	if (OverlappingActors.Num() == 0)
	{
		if (MainPlayerController)
		{
			MainPlayerController->RemoveEnemyHealthBar();  // if their is no Enmey Overlap with me tah remove EnmeyHealthbar
		}
		return;
	}

	AEnemy* ClosestEnemy = Cast<AEnemy>(OverlappingActors[0]);   //If array of Index one means one enmey is overlap with us than
	if (ClosestEnemy)  //if we find closetEnemy
	{
		FVector Location = GetActorLocation();  //GetThis actor location
		float MinDistance = (ClosestEnemy->GetActorLocation() - Location).Size(); //enemy location and overlocation and get magnitude of distance

		for (auto Actor : OverlappingActors) //All actor means Enemyy
		{
			AEnemy* Enemy = Cast<AEnemy>(Actor);  //cast with Enemy Actor
			if (Enemy)
			{
				float DistanceToActor = (Enemy->GetActorLocation() - Location).Size();
				if (DistanceToActor < MinDistance)
				{
					MinDistance = DistanceToActor;
					ClosestEnemy = Enemy;
				}
			}
		}
		if (MainPlayerController)
		{
			MainPlayerController->DisplayEnemyHealthBar(); //DisplayEnemyBar
		}
		SetCombatTarget(ClosestEnemy); //Set Comabat target to ref of closest Enemy
		bHasCombatTarget = true; //Have target
	}
}

void AMain::SwitchLevel(FName LevelName)
{
	UWorld* World = GetWorld(); //get word
	if (World)  // if world is valid
	{
		FString CurrentLevel = World->GetMapName();  //get this  world(Scene) name
		CurrentLevel.RemoveFromStart(GetWorld()->StreamingLevelsPrefix); // removing streaminglevlprefix removeuygSunTempla

		FName CurrentLevelName(*CurrentLevel); // current level name
		if (CurrentLevelName != LevelName)  // not equal to another level name
		{
			FString Level = LevelName.ToString();
			UE_LOG(LogTemp, Warning, TEXT("CurrentLevel: %s"), *CurrentLevel)
			UE_LOG(LogTemp, Warning, TEXT("LevelName: %s"), *Level)
			UGameplayStatics::OpenLevel(World, LevelName);  // open another level 
		}
	}
}

void AMain::SaveGame()
{
	/**
	 * Create a new, empty SaveGame object to set data on and then pass to SaveGameToSlot.
	 * @param	SaveGameClass	Class of SaveGame to create
	 * @return					New SaveGame object to write data to
	 */
	//Returns a UClass object representing this class at runtime
	UFirstSaveGame* SaveObject = Cast<UFirstSaveGame>(UGameplayStatics::CreateSaveGameObject(UFirstSaveGame::StaticClass()));

	//saveObjectClass to characterstats.heatlh to this variable to saave this
	SaveObject->CharacterStats.Health = Health;
	SaveObject->CharacterStats.MaxHealth = MaxHealth;
	SaveObject->CharacterStats.Stamina = Stamina;
	SaveObject->CharacterStats.MaxStamina = MaxStamina;
	SaveObject->CharacterStats.Coins = Coins;
	SaveObject->CharacterStats.Location = GetActorLocation();
	SaveObject->CharacterStats.Rotation = GetActorRotation();

	FString MapName = GetWorld()->GetMapName();
	UE_LOG(LogTemp, Warning, TEXT("MapName: %s"), *MapName)
	MapName.RemoveFromStart(GetWorld()->StreamingLevelsPrefix);
	UE_LOG(LogTemp, Warning, TEXT("MapName: %s"), *MapName)
	SaveObject->CharacterStats.LevelName = MapName;
	UE_LOG(LogTemp, Warning, TEXT("SaveObject->CharacterStats.LevelName: %s"), *SaveObject->CharacterStats.LevelName)
	if (EquippedWeapon) // if we wquipped weapon tah we want to save weapon name and particle
	{
		SaveObject->CharacterStats.WeaponName = EquippedWeapon->Name;   // equiped wepon nme and weapon name in weapon script
		SaveObject->CharacterStats.bWeaponParticles = EquippedWeapon->bWeaponParticles;
	}
	/**
	 * Save the contents of the SaveGameObject to a platform-specific save slot/file.
	 * @note This will write out all non-transient properties, the SaveGame property flag is not checked
	 *
	 * @param SaveGameObject	Object that contains data about the save game that we want to write out
	 * @param SlotName			Name of save game slot to save to.
	 * @param UserIndex			For some platforms, master user index to identify the user doing the saving.
	 * @return					Whether we successfully saved this information
	 */
	UGameplayStatics::SaveGameToSlot(SaveObject, SaveObject->SaveSlotName, SaveObject->UserIndex);
}

void AMain::LoadGame(bool LoadPosition)
{
	/**
 * Create a new, empty SaveGame object to set data on and then pass to SaveGameToSlot.
 * @param	SaveGameClass	Class of SaveGame to create
 * @return					New SaveGame object to write data to
 */
 //Returns a UClass object representing this class at runtime
	UFirstSaveGame* Load = Cast<UFirstSaveGame>(UGameplayStatics::CreateSaveGameObject(UFirstSaveGame::StaticClass()));
	/**
 * Load the contents from a given slot.
 * @param SlotName			Name of the save game slot to load from.
 * @param UserIndex			For some platforms, master user index to identify the user doing the loading.
 * @return					Object containing loaded game state (nullptr if load fails)
 */
	UFirstSaveGame* LoadObject = Cast<UFirstSaveGame>(UGameplayStatics::LoadGameFromSlot(Load->SaveSlotName, Load->UserIndex));

	if (LoadObject)  // if it is vaid
	{
		//UE_LOG(LogTemp, Warning, TEXT("Level: %s"), *LoadObject->CharacterStats.LevelName)
		if (LoadObject->CharacterStats.LevelName != "") // level name != blank
		{
			FName Map(*LoadObject->CharacterStats.LevelName);  // name load current level name
			SwitchLevel(Map);
		}

		Health = LoadObject->CharacterStats.Health; // saveobject in slotmeomory health to load health 
		MaxHealth = LoadObject->CharacterStats.MaxHealth; // saveobject in slotmeomory health to load MaxHealth 

		Stamina = LoadObject->CharacterStats.Stamina; //all this from saveobject
		MaxStamina = LoadObject->CharacterStats.MaxStamina;

		Coins = LoadObject->CharacterStats.Coins;


		if (WeaponContainer) // weaponContainer that contain weapon data in TMap<string , TSubclassOf<class AWeapon>>
		{
			//spawnAcr
			AWeaponContainerActor* Container = GetWorld()->SpawnActor<AWeaponContainerActor>(WeaponContainer);
			if (Container)
			{
				FString WeaponName = LoadObject->CharacterStats.WeaponName; //
				if (Container->WeaponMap.Num() > 0)
				{
					if (Container->WeaponMap.Contains(WeaponName)) // if weaponmap.contains string name eb bladdr or many taht is make true
					{
						AWeapon* Weapon = GetWorld()->SpawnActor<AWeapon>(Container->WeaponMap[WeaponName]); // spawn actror with name
						if (Weapon)
						{
							Weapon->bWeaponParticles = LoadObject->CharacterStats.bWeaponParticles;
							Weapon->Equip(this);  // equiped this

						}
					}
				}

			}
		}
		if (LoadPosition)
		{
			SetActorLocation(LoadObject->CharacterStats.Location);
			SetActorRotation(LoadObject->CharacterStats.Rotation);
		}
	}
}

void AMain::LoadGameNoSwitch()
{
	UFirstSaveGame* Load = Cast<UFirstSaveGame>(UGameplayStatics::CreateSaveGameObject(UFirstSaveGame::StaticClass()));
	UFirstSaveGame* LoadObject = Cast<UFirstSaveGame>(UGameplayStatics::LoadGameFromSlot(Load->SaveSlotName, Load->UserIndex));

	if (LoadObject)
	{
		Health = LoadObject->CharacterStats.Health;
		MaxHealth = LoadObject->CharacterStats.MaxHealth;

		Stamina = LoadObject->CharacterStats.Stamina;
		MaxStamina = LoadObject->CharacterStats.MaxStamina;

		Coins = LoadObject->CharacterStats.Coins;


		if (WeaponContainer)
		{
			AWeaponContainerActor* Container = GetWorld()->SpawnActor<AWeaponContainerActor>(WeaponContainer);
			if (Container)
			{
				FString WeaponName = LoadObject->CharacterStats.WeaponName;
				if (Container->WeaponMap.Num() > 0)
				{
					if (Container->WeaponMap.Contains(WeaponName))
					{
						AWeapon* Weapon = GetWorld()->SpawnActor<AWeapon>(Container->WeaponMap[WeaponName]);
						if (Weapon)
						{
							Weapon->bWeaponParticles = LoadObject->CharacterStats.bWeaponParticles;
							Weapon->Equip(this);

						}
					}
				}

			}
		}
	}
}