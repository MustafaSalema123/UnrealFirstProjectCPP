// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "FloorSwitch.generated.h"

UCLASS()
class FIRSTPROJECT_API AFloorSwitch : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AFloorSwitch();

	/** Overlap volume for functionality to be triggered */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Floor Switch")
	class UBoxComponent* TriggerBox; // this is make boxComponent that we trigger in box and we play a door up animation

	/** Switch for the character to step on */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Floor Switch")
	class UStaticMeshComponent* FloorSwitch;

	/** Door to move when the floor switch is stepped on */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Floor Switch")
	UStaticMeshComponent* Door;

	/** Initial location for the door */
	UPROPERTY(BlueprintReadWrite, Category = "Floor Switch")
	FVector InitialDoorLocation; //current location of door ! this use because we want to make door up at +z axis

	/** Initial location for the floor switch */
	UPROPERTY(BlueprintReadWrite, Category = "Floor Switch")
	FVector InitialSwitchLocation; //Iniatiol locatoin of swich this use because we want to make switch down at -z axis

	FTimerHandle SwitchHandle;  // this is time handle like courotine in unity 

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Floor Switch")
	float SwitchTime; // there is switch time use in delay

	bool bCharacterOnSwitch;

	void CloseDoor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult);
	//like on collisionEnter in unity
	UFUNCTION()
	void OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
	
	UFUNCTION(BlueprintImplementableEvent, Category = "Floor Switch")
	void RaiseDoor();   //this is implement is floorSwitchbp  the image is floorSwitch.png screenshot 

	UFUNCTION(BlueprintImplementableEvent, Category = "Floor Switch")
	void LowerDoor(); //this is implement is floorSwitchbp  the image is floorSwitch.png screenshot 
	// this method is uuse using timeline and we make animation the down door graph
	UFUNCTION(BlueprintImplementableEvent, Category = "Floor Switch")
	void RaiseFloorSwitch(); //this is implement is floorSwitchbp  the image is floorSwitch.png screenshot 

	UFUNCTION(BlueprintImplementableEvent, Category = "Floor Switch")
	void LowerFloorSwitch(); //this is implement is floorSwitchbp  the image is floorSwitch.png screenshot 

	UFUNCTION(BlueprintCallable, Category = "Floor Switch")//this is implement is floorSwitchbp  the image is floorSwitch.png screenshot 
	void UpdateDoorLocation(float Z); 
	 
	UFUNCTION(BlueprintCallable, Category = "Floor Switch")//this is implement is floorSwitchbp  the image is floorSwitch.png screenshot 
	void UpdateFloorSwitchLocation(float Z);
	
};
