// Fill out your copyright notice in the Description page of Project Settings.

#include "Enemy.h"
#include "Components/SphereComponent.h"
#include "AIController.h"
#include "Main.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Components/BoxComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Main.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Sound/SoundCue.h"
#include "Animation/AnimInstance.h"
#include "TimerManager.h"
#include "Components/CapsuleComponent.h"
#include "MainPlayerController.h"



// Sets default values
AEnemy::AEnemy()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	AgroSphere = CreateDefaultSubobject<USphereComponent>(TEXT("AgroSphere"));
	AgroSphere->SetupAttachment(GetRootComponent());  //Root atactment to parent
	AgroSphere->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldDynamic, ECollisionResponse::ECR_Ignore);  //Ignore Coll
	AgroSphere->InitSphereRadius(600.f);  //radius of sphere

	CombatCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("CombatCollision"));
	CombatCollision->SetupAttachment(GetMesh(), FName("EnemySocket"));   //Combat Collisoin that attack to player is setUpatatckment to EnemySocket

	CombatSphere = CreateDefaultSubobject<USphereComponent>(TEXT("CombatSphere"));
	CombatSphere->SetupAttachment(GetRootComponent()); //Root atactment to parent
	CombatSphere->InitSphereRadius(75.f);

	bOverlappingCombatSphere = false; //Overlapping with player is false

	Health = 75.f;
	MaxHealth = 100.f;
	Damage = 10.f;

	AttackMinTime = 0.5f;  //Attack timer is random range
	AttackMaxTime = 3.5f;

	EnemyMovementStatus = EEnemyMovementStatus::EMS_Idle;  //Starting enemy state is idel

	DeathDelay = 3.f; 

	bHasValidTarget = false;  //Player is valid in ref is false

}

// Called when the game starts or when spawned
void AEnemy::BeginPlay()
{
	Super::BeginPlay();

	AIController = Cast<AAIController>(GetController()); //Get Ai controller

	AgroSphere->OnComponentBeginOverlap.AddDynamic(this, &AEnemy::AgroSphereOnOverlapBegin);  //main
	AgroSphere->OnComponentEndOverlap.AddDynamic(this, &AEnemy::AgroSphereOnOverlapEnd);

	CombatSphere->OnComponentBeginOverlap.AddDynamic(this, &AEnemy::CombatSphereOnOverlapBegin);
	CombatSphere->OnComponentEndOverlap.AddDynamic(this, &AEnemy::CombatSphereOnOverlapEnd);

	CombatCollision->OnComponentBeginOverlap.AddDynamic(this, &AEnemy::CombatOnOverlapBegin);
	CombatCollision->OnComponentEndOverlap.AddDynamic(this, &AEnemy::CombatOnOverlapEnd);

	CombatCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);  // no collison
	CombatCollision->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
	CombatCollision->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	CombatCollision->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);

	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);  //Get mesh ECC_Camera 
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);  //Learn
}

// Called every frame
void AEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void AEnemy::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void AEnemy::AgroSphereOnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult)
{
	if (OtherActor && Alive())   // if other actor is Availble and enemy is not dead
	{
		AMain* Main = Cast<AMain>(OtherActor);  //Cast to mainPlayer to otherActor 
		if (Main) //if is Valid
		{
			MoveToTarget(Main);  //move to target
		}
	}
}

void AEnemy::AgroSphereOnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (OtherActor)    // if other actor
	{
		AMain* Main = Cast<AMain>(OtherActor); //Cast to mainPlayer to otherActor 
		{
			if (Main) // if mian Player is valid
			{
				bHasValidTarget = false;  // Valid  player is false now
				if (Main->CombatTarget == this)  // if main comat target is set to this enemy after endAgroOverlap than we set to nullptr becasue their is no enemy in player range
				{
					Main->SetCombatTarget(nullptr); // we set to nullptr becasue their is no enemy in player range
				}
				Main->SetHasCombatTarget(false); // we set to false becasue their is no enemy in player range 


				Main->UpdateCombatTarget(); // set this to null 

				SetEnemyMovementStatus(EEnemyMovementStatus::EMS_Idle); //set to idel after end overlap
				if (AIController)  //than AiControoler to stopMovement of Ai enemy
				{
					AIController->StopMovement();
				}
			}
		}
	}
}

void AEnemy::CombatSphereOnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult)
{
	if (OtherActor && Alive())   // if other actor is Availble and enemy is not dead
	{
		AMain* Main = Cast<AMain>(OtherActor);  //Cast to mainPlayer to otherActor 
		{
			if (Main)
			{
				bHasValidTarget = true;  //Theri is valid target in combat sphere

				Main->SetCombatTarget(this);  //tha we want to set CombatTarget enemy class to this beacause we want to upadate
				Main->SetHasCombatTarget(true); //set to true has target

				Main->UpdateCombatTarget(); // set this to to tArray  Actor  

				CombatTarget = Main;  //this mainPLayerCombat target to ref to main cast
				bOverlappingCombatSphere = true;  //OverlappingCombatSphere is true

				//Contiuse Attack than use 
				//Attack();
			//else
				float AttackTime = FMath::FRandRange(AttackMinTime, AttackMaxTime);  //and then we want to random range a Attacktime
				GetWorldTimerManager().SetTimer(AttackTimer, this, &AEnemy::Attack, AttackTime);// than setTimer to AttackTimer
			}
		}
	}
}

void AEnemy::CombatSphereOnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (OtherActor && OtherComp)  // // if other actor is Availblle and otherComponent 
	{
		AMain* Main = Cast<AMain>(OtherActor);  ////Cast to mainPlayer to otherActor  and get therir contains data
		{
			if (Main)
			{
				bOverlappingCombatSphere = false;  //Overleping fasle
				MoveToTarget(Main);  // move to target till player is not endoverlap agro spherer end tak
				CombatTarget = nullptr; //Mian* playerto nullptr because their is not enemy in near by me

				if (Main->CombatTarget == this)  //
				{
					Main->SetCombatTarget(nullptr);//tha we want to set CombatTarget enemy class to this beacause we want to upadate
					Main->bHasCombatTarget = false;//set to true has target
					Main->UpdateCombatTarget();// set this to to tArray  Actor  
				}
				if (Main->MainPlayerController)  //Miin- mainplayer ui
				{
					USkeletalMeshComponent* MainMesh = Cast<USkeletalMeshComponent>(OtherComp); // get otherCompnet of SkelotonMesh of player and not endOvrlap // #include "Components/SkeletalMeshComponent.h"
					if (MainMesh) Main->MainPlayerController->RemoveEnemyHealthBar(); //remove EnemyHeatl bar
				}

				GetWorldTimerManager().ClearTimer(AttackTimer);  //after endOverlap we want to clearTimer //imp
			}
		}
	}
}

void AEnemy::MoveToTarget(AMain* Target)
{
	SetEnemyMovementStatus(EEnemyMovementStatus::EMS_MoveToTarget);  //stats MoveTpTarget

	if (AIController)  //AiController in begin and get his controlelr
	{
		FAIMoveRequest MoveRequest;   //MoveRequest of this Enemy
		MoveRequest.SetGoalActor(Target);  // set target is enmey
		MoveRequest.SetAcceptanceRadius(10.0f);

		FNavPathSharedPtr NavPath;  //Nav mesh path we want to add nevmeshPath bound Compoent in scene

		AIController->MoveTo(MoveRequest, &NavPath);  //Move to target and get ref of nevpath from scene

		/**
		auto PathPoints = NavPath->GetPathPoints();
		for (auto Point : PathPoints)
		{
			FVector Location = Point.Location;

			UKismetSystemLibrary::DrawDebugSphere(this, Location, 25.f, 8, FLinearColor::Red, 10.f, 1.5f);
		}
		*/
	}
}

void AEnemy::CombatOnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult)
{
	if (OtherActor)  //  if other actor 
	{
		AMain* Main = Cast<AMain>(OtherActor); //cast to otherActor
		if (Main)  //valid main
		{
			if (Main->HitParticles)  //hit particel 
			{
				const USkeletalMeshSocket* TipSocket = GetMesh()->GetSocketByName("TipSocket"); //get the meshSocjet becasue we want to hit theier only  //#include "Engine/SkeletalMeshSocket.h"
				if (TipSocket)
				{
					FVector SocketLocation = TipSocket->GetSocketLocation(GetMesh());  //their tipSocket of socketLoaction that we pay a emmiter
					UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), Main->HitParticles, SocketLocation, FRotator(0.f), false); //Spwan particles  //#include "Kismet/GameplayStatics.h"
				}
			}
			if (Main->HitSound)   //Play hit Sound
			{
				UGameplayStatics::PlaySound2D(this, Main->HitSound);
			}
			if (DamageTypeClass)  //Miin take a DamaegTypeClass and Apply damage to player mian
			{
	//apply daamge (damageactor taht we want to damage , damageamoun that we gave using damagecasued , eventInstgaot  , damageCauser is this that we gave damage , TsubTypeClass<DamageTypeClass>
            
				UGameplayStatics::ApplyDamage(Main, Damage, AIController, this, DamageTypeClass);
			}
		}
	}
}


void AEnemy::CombatOnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
}

void AEnemy::ActivateCollision()
{
	CombatCollision->SetCollisionEnabled(ECollisionEnabled::QueryOnly); //collisoion enabled
	if (SwingSound)
	{
		UGameplayStatics::PlaySound2D(this, SwingSound);
	}
}


void AEnemy::DeactivateCollision()
{
	CombatCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision); //Collision No
}

void AEnemy::Attack()
{
	if (Alive() && bHasValidTarget) //If is not valid and has valid target
	{
		if (AIController)  // if we are closed attack tha stop attack and attacking to player
		{
			AIController->StopMovement();
			SetEnemyMovementStatus(EEnemyMovementStatus::EMS_Attacking);
		}
		if (!bAttacking)  // if atatcking is false then is attack is true
		{
			bAttacking = true;
			UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();  //get uAnimINstance getMesh Compo and inside get animInstance
			if (AnimInstance) //After valid this we want to play attack anim
			{
				AnimInstance->Montage_Play(CombatMontage, 1.35f);
				AnimInstance->Montage_JumpToSection(FName("Attack"), CombatMontage);
			}
		}
	}
}

void AEnemy::AttackEnd()
{
	bAttacking = false;  //attack is end
	if (bOverlappingCombatSphere)  //IsCombatSpher is valid
	{
		float AttackTime = FMath::FRandRange(AttackMinTime, AttackMaxTime);  //rand
		GetWorldTimerManager().SetTimer(AttackTimer, this, &AEnemy::Attack, AttackTime); //atacktiemr  , TimeHandle use class
	}
}

float AEnemy::TakeDamage(float DamageAmount, struct FDamageEvent const & DamageEvent, class AController * EventInstigator, AActor * DamageCauser)
{
	if (Health - DamageAmount <= 0.f) //take damage
	{
		Health = 0.f;
		Die(DamageCauser);  //make die usinf damageCauser of other actor 
	}
	else
	{
		Health -= DamageAmount;
	}

	return DamageAmount;
}

void AEnemy::Die(AActor* Causer)
{
	SetEnemyMovementStatus(EEnemyMovementStatus::EMS_Dead);  // enemy is dead
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();  //get animINstacne = getmesh->getanimInstance
	if (AnimInstance) // if valid tha play montage play
	{
		AnimInstance->Montage_Play(CombatMontage, 1.0f);
		AnimInstance->Montage_JumpToSection(FName("Death"), CombatMontage);
	}

	CombatCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision); //then disable all collision in CombatCollision ,  AgroSphere , CombatSphere ,GetCapsuleComponent , attack is false
	AgroSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	CombatSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	bAttacking = false;

	AMain* Main = Cast<AMain>(Causer);  //takeing damage from causer other Actor
	if (Main)
	{
		Main->UpdateCombatTarget(); //taking damafe
	}
}

void AEnemy::DeathEnd()
{
	GetMesh()->bPauseAnims = true;   //after death end pauseAnim , NoSketonupdate = true , disapper ther enemuy or destroy it after 3 second
	GetMesh()->bNoSkeletonUpdate = true;

	GetWorldTimerManager().SetTimer(DeathTimer, this, &AEnemy::Disappear, DeathDelay);
}

bool AEnemy::Alive()
{
	return GetEnemyMovementStatus() != EEnemyMovementStatus::EMS_Dead;
}

void AEnemy::Disappear()
{
	Destroy();
}