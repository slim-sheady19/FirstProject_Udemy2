// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Enemy.generated.h"

//create enum class EnemyMovementStatus of variable type uint8
UENUM(BlueprintType)
enum class EEnemyMovementStatus : uint8
{
	EMS_Idle			UMETA(DisplayName = "Idle"),
	EMS_MoveToTarget	UMETA(DisplayName = "MoveToTarget"),
	EMS_Attacking		UMETA(DisplayName = "Attacking"),

	EMS_MAX				UMETA(DisplayName = "DefaultMAX")
};

UCLASS()
class FIRSTPROJECT_UDEMY2_API AEnemy : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AEnemy();


	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Movement")
	EEnemyMovementStatus EnemyMovementStatus;

	//Inline Setter
	FORCEINLINE void SetEnemyMovementStatus(EEnemyMovementStatus Status) { EnemyMovementStatus = Status; }

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

};
