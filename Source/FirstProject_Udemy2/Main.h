// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Main.generated.h"

UENUM(BlueprintType)
enum class EMovementStatus : uint8
{
	EMS_Normal UMETA(DisplayName = "Normal"),
	EMS_Sprinting UMETA(DisplayName = "Sprinting"),
	EMS_Dead UMETA(DisplayName = "Dead"),

	EMS_MAX UMETA(DisplayName = "DefaultMAX")
};

UENUM(BlueprintType)
enum class EStaminaStatus : uint8
{
	ESS_Normal UMETA(DisplayName = "Normal"),
	ESS_BelowMinimum UMETA(DisplayName = "BelowMinimum"),
	ESS_Exhausted UMETA(DisplayName = "Exhausted"),
	ESS_ExhaustedRecovering UMETA(DisplayName = "ExhaustedRecovering"),

	ESS_MAX UMETA(DisplayName = "DefaultMax")
};

UCLASS()
class FIRSTPROJECT_UDEMY2_API AMain : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AMain();

	//Give Main_BP ability to choose from dropdown a WeaponStorage BP to store in this TSubclass of called WeaponStorage
	UPROPERTY(EditDefaultsOnly, Category = "SavedData")
	TSubclassOf<class AItemStorage> WeaponStorage;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat")
	bool bHasCombatTarget;

	//setter inline function called SetHasCombatTarget to set boolean for if it has a combat target takes argument boolean HasTarget
	FORCEINLINE void SetHasCombatTarget(bool HasTarget) { bHasCombatTarget = HasTarget; }

	//create variable to hold vector of combat target's location
	UPROPERTY(BlueprintReadWrite, VisibleAnywhere, Category = "Combat")
	FVector CombatTargetLocation;

	//add Controller variable so we have access to MainPlayerController
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Controller")
	class AMainPlayerController* MainPlayerController;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	class UParticleSystem* HitParticles;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	class USoundCue* HitSound;

	TArray<FVector> PickupLocations; // Create an array of type FVectors called PickupLocations

	UFUNCTION(BlueprintCallable)
	void ShowPickupLocations();

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Enums")
	EMovementStatus MovementStatus;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Enums")
	EStaminaStatus StaminaStatus;

	FORCEINLINE void SetStaminaStatus(EStaminaStatus Status) { StaminaStatus = Status; }

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float StaminaDrainRate;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float MinSprintStamina;

	//Interp to Enemy so that Main orients itself in direction of Enemy while attacking otherwise Main has to be aimed too precisely towards Enemy
	float InterpSpeed;
	bool bInterpToEnemy;
	void SetInterpToEnemy(bool Interp);

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat")
	class AEnemy* CombatTarget;

	FORCEINLINE void SetCombatTarget(AEnemy* Target) { CombatTarget = Target; }

	FRotator GetLookAtRotationYaw(FVector Target);

	//Set movement status and running speed
	void SetMovementStatus(EMovementStatus Status);

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Running")
	float RunningSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Running")
	float SprintingSpeed;

	bool bShiftKeyDown;

	//Pressed down to enable sprinting
	void ShiftKeyDown();

	//Released to stop sprinting
	void ShiftKeyUp();

	//Camera boom positioning the camera behind the player
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true")) //AllowPrivateAccess lets us access this suboject in BP
	class USpringArmComponent* CameraBoom;

	//Follow Camera
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* FollowCamera;

	//Base turn rates to scale turning functions for the camera
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
	float MaxHealth;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Player Stats")
	float Health;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Player Stats")
	float MaxStamina;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Player Stats")
	float Stamina;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Player Stats")
	int32 Coins;

	void DecrementHealth(float Amount);

	//Inherited (from Actor class) function TakeDamage that returns float with function signature copied from UE4 documentation
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;

	UFUNCTION(BlueprintCallable)
	void IncrementCoins(int32 Amount);

	UFUNCTION(BlueprintCallable)
	void IncrementHealth(float Amount);

	void Die();

	virtual void Jump() override;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	//Called for forwards/backwards input
	void MoveForward(float Value);

	//Called for side to side input
	void MoveRight(float Value);

	bool bMovingForward;
	bool bMovingRight;

	//Called via input to turn at a given rate
	//@param Rate This is a noramlized rate, i.e. 1.0 means 100% of desired turn rate
	void TurnAtRate(float Rate);

	//Called via input to turn at a given rate
	//@param Rate This is a noramlized rate, i.e. 1.0 means 100% of desired turn rate
	void LookUpAtRate(float Rate);

	bool bLMBDown;
	void LMBDown();
	void LMBUp();

	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Items)
	class AWeapon* EquippedWeapon;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Items)
	class AItem* ActiveOverlappingItem;

	void SetEquippedWeapon(AWeapon* WeaponToSet);

	UFUNCTION(BlueprintCallable)
	AWeapon* GetEquippedWeapon() { return EquippedWeapon; }

	FORCEINLINE void SetActiveOverlappingItem(AItem* Item) { ActiveOverlappingItem = Item; }

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Anims")
	bool bAttacking;

	void Attack();
	
	UFUNCTION(BlueprintCallable)
	void AttackEnd();

	//Create Anim Montage for this character
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Anims")
	class UAnimMontage* CombatMontage;

	UFUNCTION(BlueprintCallable)
	void PlaySwingSound();

	//function to stop player from moving after death anim
	UFUNCTION(BlueprintCallable)
	void DeathEnd();

	//function to update Main's combat target after killing an enemy (for fighting multiple enemies)
	void UpdateCombatTarget();

	//Array to hold all Combat Targets for function above. We filter for only Enemy actors
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	TSubclassOf<AEnemy> EnemyFilter;

	//function SwitchLevel with input paramater LevelNAme
	void SwitchLevel(FName LevelName);

	UFUNCTION(BlueprintCallable)
	void SaveGame();

	UFUNCTION(BlueprintCallable)
	void LoadGame(bool SetPosition); //bool SetPosition is for whether or not we want to load Main's position because sometimes we don't want to
};
