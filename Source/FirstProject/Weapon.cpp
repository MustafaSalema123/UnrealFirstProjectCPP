// Fill out your copyright notice in the Description page of Project Settings.

#include "Weapon.h"


#include "Components/SkeletalMeshComponent.h"
#include "Main.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Sound/SoundCue.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystemComponent.h"
#include "Components/BoxComponent.h"
#include "Enemy.h"
#include "Engine/SkeletalMeshSocket.h"


AWeapon::AWeapon()
{
	SkeletalMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("SkeletalMesh"));
	SkeletalMesh->SetupAttachment(GetRootComponent());  //Weapon skeloten mesh

	CombatCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("CombatCollision"));
	CombatCollision->SetupAttachment(GetRootComponent()); //Combat sphere that collide with enemy for damage
	CombatCollision->SetVisibility(false);

	bWeaponParticles = false;

	WeaponState = EWeaponState::EWS_Pickup;  // pickabel state in start

	Damage = 25.f;
}

void AWeapon::BeginPlay()
{
	Super::BeginPlay();

	CombatCollision->OnComponentBeginOverlap.AddDynamic(this, &AWeapon::CombatOnOverlapBegin);
	CombatCollision->OnComponentEndOverlap.AddDynamic(this, &AWeapon::CombatOnOverlapEnd);

	CombatCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision); // start mai no colllision
	CombatCollision->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
	CombatCollision->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	CombatCollision->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);
}


void AWeapon::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult)
{
	Super::OnOverlapBegin(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult); //inherit from Item class 
	if ((WeaponState == EWeaponState::EWS_Pickup) && OtherActor) // check id weaponstaet == pickup and otherActor 
	{
		AMain* Main = Cast<AMain>(OtherActor);  //cast otheractor from Main
		if (Main)
		{
			Main->SetActiveOverlappingItem(this);  //set form main_pllayer script of ref
		}
	}
}

void AWeapon::OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	Super::OnOverlapEnd(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex);
	if (OtherActor) // if otherActor 
	{
		AMain* Main = Cast<AMain>(OtherActor);
		if (Main)
		{
			Main->SetActiveOverlappingItem(nullptr);  //set form nullptr script
		}
	}
}

void AWeapon::Equip(AMain* Char)
{
	if (Char) //Equiped char
	{
		SetInstigator(Char->GetController());   

		SkeletalMesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore); //set collisionresponnce
		SkeletalMesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Ignore);

		SkeletalMesh->SetSimulatePhysics(false); //set physics to false

		//skelotan mesh righthandsocket get char-getmesh-getsocketbyname and name in right hand
		const USkeletalMeshSocket* RightHandSocket = Char->GetMesh()->GetSocketByName("RightHandSocket");
		if (RightHandSocket)  // if its valid 
		{
		
				//	RightHandSocket->AttachActor(actor and USkelonmeshComopnent) thwt we are connect with player char mesh
			RightHandSocket->AttachActor(this, Char->GetMesh()); //righthandsockte- attachActor
			bRotate = false;  //rotate of item is false

			Char->SetEquippedWeapon(this);    //player setEquippedWeapon to this class
			Char->SetActiveOverlappingItem(nullptr);  //item to nullptr
		}
		if (OnEquipSound) UGameplayStatics::PlaySound2D(this, OnEquipSound);
		if (!bWeaponParticles) // weapon particle is fasle tha deactivate it
		{
			IdleParticlesComponent->Deactivate();
		}
		else
		{
			IdleParticlesComponent->Activate();
		}
	}
}


void AWeapon::CombatOnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult)
{
	if (OtherActor)   // see other actor
	{
		AEnemy* Enemy = Cast<AEnemy>(OtherActor);  //cast it
		if (Enemy)
		{
			if (Enemy->HitParticles)
			{
				const USkeletalMeshSocket* WeaponSocket = SkeletalMesh->GetSocketByName("WeaponSocket"); // get the weaponSocket naem to get it point to spawan particel
				if (WeaponSocket)
				{
					FVector SocketLocation = WeaponSocket->GetSocketLocation(SkeletalMesh);
					UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), Enemy->HitParticles, SocketLocation, FRotator(0.f), false);
				}
			}
			if (Enemy->HitSound) //sound of ennemy
			{
				UGameplayStatics::PlaySound2D(this, Enemy->HitSound);
			}
			if (DamageTypeClass)  // damage type class
			{
				//apply daamge (damageactor taht we want to damage , damageamount that we gave using damagecasued , eventInstgaot  , damageCauser is this that we gave damage , TsubTypeClass<DamageTypeClass>
				UGameplayStatics::ApplyDamage(Enemy, Damage, WeaponInstigator, this, DamageTypeClass);
			}
		}
	}
}


void AWeapon::CombatOnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{

}

void AWeapon::ActivateCollision()
{
	CombatCollision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);  // see in blueprint
}


void AWeapon::DeactivateCollision() // see in blueprint
{
	CombatCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}