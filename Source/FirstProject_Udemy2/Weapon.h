// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Item.h"
#include "Weapon.generated.h"

UENUM(BlueprintType)
enum class EWeaponState :uint8
{
	EWS_Pickup UMETA(DisplayName = "Pickup"),
	EWS_Equipped UMETA(DisplayName = "Equipped"),

	EWS_Max UMETA(DisplayName = "DefaultMax")
};

/**
 * 
 */
UCLASS()
class FIRSTPROJECT_UDEMY2_API AWeapon : public AItem
{
	GENERATED_BODY()
public:

	AWeapon();

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Item")
	EWeaponState WeaponState;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item | Particles")
	bool bWeaponParticles;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item | Sound")
	class USoundCue* OnEquipSound;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "SkeletalMesh")
	class USkeletalMeshComponent* SkeletalMesh;

	//Inherited function cannot be marked UFUNCTION.  We use "override" to overwrite the properties of the parent function
	virtual void OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) override;

	virtual void OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex) override;

	void Equip(class AMain* Char);

	//create a function called SetWeaponState that uses the enum class WeaponState, set that to a variable called State
	FORCEINLINE void SetWeaponState(EWeaponState State) { WeaponState = State; } 
	FORCEINLINE EWeaponState GetWeaponState() { return WeaponState; } //Create function called GetWeaponState (takes no paramters) and returns WeaponState
	
};
