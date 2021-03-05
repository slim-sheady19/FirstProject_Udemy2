// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy.h"
#include "Components/SphereComponent.h"
#include "AIController.h"
#include "Main.h"
#include "Kismet/KismetSystemLibrary.h"

// Sets default values
AEnemy::AEnemy()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	AgroSphere = CreateDefaultSubobject<USphereComponent>(TEXT("AgroSphere"));
	AgroSphere->SetupAttachment(GetRootComponent());
	AgroSphere->InitSphereRadius(600.f);

	CombatSphere = CreateDefaultSubobject<USphereComponent>(TEXT("CombatSphere"));
	CombatSphere->SetupAttachment(GetRootComponent());
	CombatSphere->InitSphereRadius(75.f);
}

// Called when the game starts or when spawned
void AEnemy::BeginPlay()
{
	Super::BeginPlay();

	//Get reference to AIController on begin play and store in variable AIController
	AIController = Cast<AAIController>(GetController());

	//Bind Overlap event functions to Spheres
	AgroSphere->OnComponentBeginOverlap.AddDynamic(this, &AEnemy::AgroSphereOnOverlapBegin);
	AgroSphere->OnComponentEndOverlap.AddDynamic(this, &AEnemy::AgroSphereOnOverlapEnd);

	CombatSphere->OnComponentBeginOverlap.AddDynamic(this, &AEnemy::CombatSphereOnOverlapBegin);
	CombatSphere->OnComponentEndOverlap.AddDynamic(this, &AEnemy::CombatSphereOnOverlapEnd);
	
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


void AEnemy::AgroSphereOnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	//Check if it's an Other Actor
	if (OtherActor)
	{
		AMain* Main = Cast<AMain>(OtherActor);
		if (Main)
		{
			//Make enemy move to target Main when Main enters AgroSphere
			MoveToTarget(Main);
		}
	}
}

void AEnemy::AgroSphereOnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (OtherActor)
	{
		AMain* Main = Cast<AMain>(OtherActor);
		{
			if (Main)
			{
				//Stop enemy movement when Main exits AgroSphere
				SetEnemyMovementStatus(EEnemyMovementStatus::EMS_Idle);
				//Enemy is not stopping attacking with previous line of code, so we will manually stop it with AIController
				if (AIController)
				{
					AIController->StopMovement();
				}

			}
		}
	}
}

void AEnemy::CombatSphereOnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor)
	{
		AMain* Main = Cast<AMain>(OtherActor);
		{
			if (Main)
			{
				//Start enemy attack when Main enters Combat Sphere
				SetEnemyMovementStatus(EEnemyMovementStatus::EMS_Attacking);
			}
		}
	}
}

void AEnemy::CombatSphereOnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (OtherActor)
	{
		AMain* Main = Cast<AMain>(OtherActor);
		{
			if (Main)
			{
				//Set Enemy back to moving to target when Main exits combat sphere
				SetEnemyMovementStatus(EEnemyMovementStatus::EMS_MoveToTarget);
				MoveToTarget(Main);
			}
		}
	}
}

void AEnemy::MoveToTarget(AMain* Target)
{
	//Set Enemy movement status right away
	SetEnemyMovementStatus(EEnemyMovementStatus::EMS_MoveToTarget);

	if (AIController) //check is valid
	{
		//Create FAIMoveRequest called MoveRequest and Specify input parameters
		FAIMoveRequest MoveRequest;
		MoveRequest.SetGoalActor(Target);
		//Distance between collision volumes
		MoveRequest.SetAcceptanceRadius(5.0f);

		FNavPathSharedPtr NavPath;

		AIController->MoveTo(MoveRequest, &NavPath);

		//create array of type FNavPathPoints called PathPoints and call GetPathPoints from NavPath to store in the array
		TArray<FNavPathPoint> PathPoints = NavPath->GetPathPoints();
		//Loop through PathPoints to get locations and draw debug sphere
		for (auto Point : PathPoints)
		{
			//Create local variable of type FVector called Location, call Location on each Point
			FVector Location = Point.Location;
			//Draw debug sphere
			UKismetSystemLibrary::DrawDebugSphere(this, Location, 25.f, 8, FLinearColor::Blue, 10.f, 0.5f);
		}
	}
}
