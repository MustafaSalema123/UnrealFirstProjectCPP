// Fill out your copyright notice in the Description page of Project Settings.

#include "FloorSwitch.h"
#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "TimerManager.h"



// Sets default values
AFloorSwitch::AFloorSwitch()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	TriggerBox = CreateDefaultSubobject<UBoxComponent>(TEXT("TriggerBox"));  //#include "Components/BoxComponent.h"
	RootComponent = TriggerBox;

	TriggerBox->SetCollisionEnabled(ECollisionEnabled::QueryOnly); 
	//description of ECollisionEnabled::QueryOnly = allow only overlap event but not allow engine physics
	//description of ECollisionEnabled::PhysicsOnly= collision present that will check for overlap event it just detect if somwthing ciliders with it or something related
	//to physics set for optimization no lonher allow functionalty
	//description of ECollisionEnabled::Queryandphysics:- the allow physics or overlap event as well

	TriggerBox->SetCollisionObjectType(ECollisionChannel::ECC_WorldStatic);
	//Changes the collision channel that this object uses when it moves

	TriggerBox->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	//learn from https://www.youtube.com/watch?v=O8aV86Gdce8&ab_channel=MathewWadstein //
	
	TriggerBox->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);
	//Set Collision Response to ChannelSet Collision Response to Channel
	//Changes a member of the ResponseToChannels container for this PrimitiveComponent.

	TriggerBox->SetBoxExtent(FVector(62.f, 62.f, 32.f));   //

	FloorSwitch = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("FloorSwitch"));  //#include "Components/StaticMeshComponent.h
	FloorSwitch->SetupAttachment(GetRootComponent()); //set rootComponent

	Door = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Door"));  
	Door->SetupAttachment(GetRootComponent());

	SwitchTime = 2.f; //switch_Time where we controll usinf timeHandler //Headdr file we use is // #include "TimerManager.h"
	bCharacterOnSwitch = false;  //starting me player switch me player khada nhi hai
}

// Called when the game starts or when spawned
void AFloorSwitch::BeginPlay()
{
	Super::BeginPlay();

	TriggerBox->OnComponentBeginOverlap.AddDynamic(this, &AFloorSwitch::OnOverlapBegin); //use this for overlap event
	TriggerBox->OnComponentEndOverlap.AddDynamic(this, &AFloorSwitch::OnOverlapEnd);

	InitialDoorLocation = Door->GetComponentLocation();     //The current lcoation of door FVector using GetComponentLocation();
	InitialSwitchLocation = FloorSwitch->GetComponentLocation();  //The current lcoation of Switch FVector using GetComponentLocation();
	
}

// Called every frame
void AFloorSwitch::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AFloorSwitch::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult)
{
	if (!bCharacterOnSwitch) bCharacterOnSwitch = true;  //agar overlap hua fir player khada nhi hai to khada hoga fir bCharacterOnSwitch true hoga 
	//for door up hoga and floor down hoga
	RaiseDoor();
	LowerFloorSwitch();
}

void AFloorSwitch::OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (bCharacterOnSwitch) bCharacterOnSwitch = false;  //agar overlapEnd hua fir player  hai to khada nhi hoga fir bCharacterOnSwitch true se false hoga hoga 
	//fir GetWorldTimerManager() Settime se close door hoga after 2 second jo uper se varible de chuke hai
	GetWorldTimerManager().SetTimer(SwitchHandle, this, &AFloorSwitch::CloseDoor, SwitchTime);
}

void AFloorSwitch::UpdateDoorLocation(float Z)  //call in bluePrint
{
	FVector NewLocation = InitialDoorLocation;  //current door location of and after up the door location and setwordlLocation in z is use in blueprint
	NewLocation.Z += Z;
	Door->SetWorldLocation(NewLocation);
}

void AFloorSwitch::UpdateFloorSwitchLocation(float Z)
{
	FVector NewLocation = InitialSwitchLocation; //current FloorSwitch location of and after down the FloorSwitch location and setwordlLocation in z is use in blueprint
	NewLocation.Z += Z;
	FloorSwitch->SetWorldLocation(NewLocation);
}

void AFloorSwitch::CloseDoor()
{
	if (!bCharacterOnSwitch)
	{
		LowerDoor();    // if agar player is not stand in floorswitch than lowerdoor at oldposi and raiseFloorswitch
		RaiseFloorSwitch();  
	}
}