// Fill out your copyright notice in the Description page of Project Settings.

#include "SpawnVolume.h"
#include "Components/BoxComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Engine/World.h"
#include "Enemy.h"
#include "AIController.h"



// Sets default values
ASpawnVolume::ASpawnVolume()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SpawningBox = CreateDefaultSubobject<UBoxComponent>(TEXT("SpawningBox"));  //creta spawnm 


}

// Called when the game starts or when spawned
void ASpawnVolume::BeginPlay()
{
	Super::BeginPlay();

	if (Actor_1 && Actor_2 && Actor_3 && Actor_4)  //SpawnArray that we add actor_1 , 2 , 3, 4 in TArray
	{
		SpawnArray.Add(Actor_1);
		SpawnArray.Add(Actor_2);
		SpawnArray.Add(Actor_3);
		SpawnArray.Add(Actor_4);
	}
	
}

// Called every frame
void ASpawnVolume::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

FVector ASpawnVolume::GetSpawnPoint()
{
	FVector Extent = SpawningBox->GetScaledBoxExtent();   //thi method is getScaledBoxExtent eample x , y ,z of boxCordinate
	FVector Origin = SpawningBox->GetComponentLocation(); // this method get  GetComponentLocation location of box

	FVector Point = UKismetMathLibrary::RandomPointInBoundingBox(Origin, Extent); //using UKismetMathLibrary:: get the randomPointinBoundingbox
	//#include "Kismet/KismetMathLibrary.h" use
	return Point;
}

void ASpawnVolume::SpawnOurActor_Implementation(UClass* ToSpawn, const FVector& Location)
{
	if (ToSpawn)
	{
		UWorld* World = GetWorld();   //#include "Engine/World.h" use this
		FActorSpawnParameters SpawnParams;  // this is spwanparameter 

		if (World)  // if world is valid
		{
			//ACritter* critter = World->SpawnActor<ACritter>(ToSpawn, Location, FRotator(0.f), SpawnParams); //for one object only  #include "Critter.h"

			AActor* Actor = World->SpawnActor<AActor>(ToSpawn, Location, FRotator(0.f), SpawnParams); //spwamn a actor uisng world

			AEnemy* Enemy = Cast<AEnemy>(Actor);   // if we not check this a Enenm= controller is not updated after spawing a enemy 0r any object
			if (Enemy)
			{
				Enemy->SpawnDefaultController();   //get Enemy after spawndefault controller 	/** Spawn default controller for this Pawn, and get possessed by it.

				AAIController* AICont = Cast<AAIController>(Enemy->GetController());   //cast the AI data Controller 
				if (AICont)
				{
					Enemy->AIController = AICont;  //this Ai data controller to nre Ai data		
				}
			}
		}
	}
}

TSubclassOf<AActor> ASpawnVolume::GetSpawnActor()
{
	if (SpawnArray.Num() > 0)  //if spwanAray is geater tha zero thas return selection
	{
		int32 Selection = FMath::RandRange(0, SpawnArray.Num() - 1);  //random.Range of start array to last array like array start with 0 that
		//i so 0 is 1 enmemy and and 3 is last enmey and return a enemy that we are spawn is called in blueprint check image of Enemyvolume.png

		return SpawnArray[Selection];
	}
	else
	{
		return nullptr;
	}
}