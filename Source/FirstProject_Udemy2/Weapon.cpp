// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapon.h"
#include "Components/SkeletalMeshComponent.h"
#include "Main.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Sound/SoundCue.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystemComponent.h"

AWeapon::AWeapon()
{
	SkeletalMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("SkeletalMesh"));
	SkeletalMesh->SetupAttachment(GetRootComponent());

	bWeaponParticles = false;
}

void AWeapon::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	Super::OnOverlapBegin(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult); 
	
	//if Main Character collides with Weapon, cast to Main and Equip Weapon
	if (OtherActor) 
	{
		AMain* Main = Cast<AMain>(OtherActor); 
		if (Main)
		{
			//Stop overlapping Item if we haven't equipped weapon
			Main->SetActiveOverlappingItem(this);
		}
	}
}

void AWeapon::OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	Super::OnOverlapEnd(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex);
	if (OtherActor) 
	{
		AMain* Main = Cast<AMain>(OtherActor);
		if (Main)
		{
			Main->SetActiveOverlappingItem(nullptr);
		}
	}
}

void AWeapon::Equip(AMain* Char)
{
	if (Char) //check if Char is valid
	{
		SkeletalMesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore); //Make camera ignore collision
		SkeletalMesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Ignore);

		//Remove physics for Equipped Weapon
		SkeletalMesh->SetSimulatePhysics(false);

		//Create RightHandSocket variable for the weapon socket
		const USkeletalMeshSocket* RightHandSocket = Char->GetMesh()->GetSocketByName("RightHandSocket");
		if (RightHandSocket) //check RightHandSocket is valid
		{
			RightHandSocket->AttachActor(this, Char->GetMesh()); //Attach to RightHand Socket
			bRotate = false; //Stop rotating weapon (from BP) once equipped to Main
			Char->SetEquippedWeapon(this);
		}
		if (OnEquipSound) UGameplayStatics::PlaySound2D(this, OnEquipSound);
		if (!bWeaponParticles)
		{
			IdleParticlesComponent->Deactivate();
		}
	}
}