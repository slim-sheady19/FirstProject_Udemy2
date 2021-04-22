// Fill out your copyright notice in the Description page of Project Settings.


#include "Main.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "Engine/World.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Weapon.h"
#include "Components/SkeletalMeshComponent.h"
#include "Animation/AnimInstance.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundCue.h"
#include "Kismet/KismetMathLibrary.h"
#include "Enemy.h"
#include "MainPlayerController.h"
#include "FirstSaveGame.h"
#include "ItemStorage.h"

// Sets default values
AMain::AMain()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	//Create Camera Boom; pulls towards player if there's A COLLISION
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(GetRootComponent());
	CameraBoom->TargetArmLength = 600.f; //Camera follows at this distance
	CameraBoom->bUsePawnControlRotation = true; //Rotate arm based on controller

	//Set size for collision capsule
	GetCapsuleComponent()->SetCapsuleSize(48.f, 105.f);

	//Create Follow Camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	//Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->bUsePawnControlRotation = false;

	//Hard coding turn rates for input
	BaseTurnRate = 65.f;
	BaseLookUpRate = 65.f;

	// don't rotate the character when the controller rotates
	// let that just affect the camera
	bUseControllerRotationYaw = false;
	bUseControllerRotationPitch = false;
	bUseControllerRotationRoll = false;

	//configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; //character moves in the direction of input...
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 540.f, 0.0f); //...at this rotation rate
	GetCharacterMovement()->JumpZVelocity = 650.f;
	GetCharacterMovement()->AirControl = 0.2f;

	MaxHealth = 100.f;
	Health = 65.f;
	MaxStamina = 150.f;
	Stamina = 120.f;
	Coins = 0;

	RunningSpeed = 650.f;
	SprintingSpeed = 950.f;

	bShiftKeyDown = false;
	bLMBDown = false;

	bESCDown = false;

	//Initialize enums
	MovementStatus = EMovementStatus::EMS_Normal;
	StaminaStatus = EStaminaStatus::ESS_Normal;

	StaminaDrainRate = 25.f;
	MinSprintStamina = 50.f;

	InterpSpeed = 15.f;
	bInterpToEnemy = false;

	bHasCombatTarget = false;

	bMovingForward = false;
	bMovingRight = false;
}

// Called when the game starts or when spawned
void AMain::BeginPlay()
{
	Super::BeginPlay();

	MainPlayerController = Cast<AMainPlayerController>(GetController());
}

// Called every frame
void AMain::Tick(float DeltaTime)
{
	//Super is the keyword for inheriting parent class function, in this case Actor:Tick
	Super::Tick(DeltaTime);

	if (MovementStatus == EMovementStatus::EMS_Dead) return; //check Main isn't dead before proceeding

	float DeltaStamina = StaminaDrainRate * DeltaTime; //create float variable called DeltaStamina that is equal to stamina drain rate times difference in time

	switch (StaminaStatus) //switch statement for all the cases of stamina statuses
	{
	case EStaminaStatus::ESS_Normal: //stamina status that we can sprint in
		if (bShiftKeyDown) //inside normal stamina status case, and shift key is down, do the following
		{
			if (Stamina - DeltaStamina <= MinSprintStamina) //if stamina - difference in stamina is less than or equal to minimum sprintable stamina,
			{
				SetStaminaStatus(EStaminaStatus::ESS_BelowMinimum); //then set stamina status to below the minimum sprintable state
				Stamina -= DeltaStamina; //and continue to drain stamina	
			}
			else //else stamina - difference in stamina is greater than minimum sprintable stanima
			{
				Stamina -= DeltaStamina; //or drain stamina
			}
			if (bMovingForward || bMovingRight) // if we are in normal stanima state, and moving, and shift key is down, change movement status to sprinting
			{
				SetMovementStatus(EMovementStatus::EMS_Sprinting); 
			}
			else //go back to Normal
			{
				SetMovementStatus(EMovementStatus::EMS_Normal);
			}
		}
		else //else we are in normal stamina status and shift key is up, do the following
		{
			if (Stamina + DeltaStamina >= MaxStamina) //if stamina plus regenerated is greater than or equal to maxstamina,
			{
				Stamina = MaxStamina; //then set stamina to max stamina
			}
			else //else stamina plus regenerated stamina is less than max
			{
				Stamina += DeltaStamina; //regenerate stamina
			}
			SetMovementStatus(EMovementStatus::EMS_Normal); // we are in normal stamina state and shift key is up so set movement status to normal
			
		}
		break;
	case EStaminaStatus::ESS_BelowMinimum: //case stamina status where we are at 0 stamina and/or below minimum sprintable stamina
		if (bShiftKeyDown) //shift key down
		{
			if (Stamina - DeltaStamina <= 0.f) //If stamina minus drained stamina is less than or equal to 0,
			{
				SetStaminaStatus(EStaminaStatus::ESS_Exhausted); //set stamina status to exhausted
				Stamina = 0.f; //and set stamina to 0
				SetMovementStatus(EMovementStatus::EMS_Normal); //and set movement status to normal
			}
			else //else stamina minus drained stamina is greater than 0
			{
				Stamina -= DeltaStamina; //then continue to drain stamina
				if (bMovingForward || bMovingRight) //if either Moving Forward or Moving Right is true, set Movement status to Sprinting
				{
					SetMovementStatus(EMovementStatus::EMS_Sprinting);
				}
				else //go back to Normal
				{
					SetMovementStatus(EMovementStatus::EMS_Normal);
				}
			}
		}
		else  //shift key up
		{
			if (Stamina + DeltaStamina >= MinSprintStamina) //check if stamina + regenerated stamina became greater than or equal to minimum sprintable stamina (case normal)
			{
				SetStaminaStatus(EStaminaStatus::ESS_Normal); //then set stamina status to normal state
				Stamina += DeltaStamina; //and regenerate stamina
			}
			else //else stamina + regenrated stamina is still less than minimum sprintable stamina (case normal)
			{
				Stamina += DeltaStamina; //regenerate stamina
			}
			SetMovementStatus(EMovementStatus::EMS_Normal); //set movement status to normal because shift key is up
		}
		break;
	case EStaminaStatus::ESS_Exhausted: //case stamina status where stamina has reached 0 and we are unable to sprint
		if (bShiftKeyDown) //check if shift key is down
		{
			Stamina = 0.f; //stamina is 0 and cannot regenerate
		}
		else //shift key is up
		{
			SetStaminaStatus(EStaminaStatus::ESS_ExhaustedRecovering); //set stamina status to recovering while still below minimum sprintable (case ExhaustedRecovering)
			Stamina += DeltaStamina; //regenerate stamina
		}
		SetMovementStatus(EMovementStatus::EMS_Normal); //whether shift key is up or down movement status gets set to normal
		break;
	case EStaminaStatus::ESS_ExhaustedRecovering: //case stamina status after reaching 0 and needs to regenerate to minimum sprintable stamina status (stamina status case normal)
		if (Stamina + DeltaStamina >= MinSprintStamina) //check if stamina plus regenerated stamina is greater than or equal to minimum sprintable stamina
		{
			SetStaminaStatus(EStaminaStatus::ESS_Normal); //then set stamina status to normal
			Stamina += DeltaStamina; //and continue to regenerate stamina
		}
		else //else stamina plus regenerated stamina is still less than minimum sprintable
		{
			Stamina += DeltaStamina; //continue to regenerate
		}
		SetMovementStatus(EMovementStatus::EMS_Normal); //whether shift key is up or down movement status remnains in normal
		break;
	default:
		;
	}

	if (bInterpToEnemy && CombatTarget)
	{
		FRotator LookAtYaw = GetLookAtRotationYaw(CombatTarget->GetActorLocation());
		//Use RInterpTo so that Main doesn't just snap immediately, will be a more smooth turn
		FRotator InterpRotation = FMath::RInterpTo(GetActorRotation(), LookAtYaw, DeltaTime, InterpSpeed);

		SetActorRotation(InterpRotation);
	}

	//update target location
	if (CombatTarget)
	{
		CombatTargetLocation = CombatTarget->GetActorLocation();

		//update player controller with combat target location
		if (MainPlayerController)
		{
			MainPlayerController->EnemyLocation = CombatTargetLocation;
		}
	}
}

FRotator AMain::GetLookAtRotationYaw(FVector Target)
{
	//Create variable of type FRotator, which is the function of UkismetMathLibrary with input paramaters of Main's ActorLocation and Target's location
	FRotator LookAtRotation = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), Target);
	//Isolate yaw from LookAtRotation
	FRotator LookAtRotationYaw(0.f, LookAtRotation.Yaw, 0.f);
	return LookAtRotationYaw;
}

// Called to bind functionality to input
void AMain::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	check(PlayerInputComponent); //check is playerinputcomponent is valid

	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &AMain::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);

	PlayerInputComponent->BindAction("Sprint", IE_Pressed, this, &AMain::ShiftKeyDown);
	PlayerInputComponent->BindAction("Sprint", IE_Released, this, &AMain::ShiftKeyUp);

	PlayerInputComponent->BindAction("LMB", IE_Pressed, this, &AMain::LMBDown);
	PlayerInputComponent->BindAction("LMB", IE_Released, this, &AMain::LMBUp);

	PlayerInputComponent->BindAxis("MoveForward", this, &AMain::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &AMain::MoveRight);

	PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);
	PlayerInputComponent->BindAxis("TurnRate", this, &AMain::TurnAtRate);
	PlayerInputComponent->BindAxis("LookUpRate", this, &AMain::LookUpAtRate);

	PlayerInputComponent->BindAction("ESC", IE_Pressed, this, &AMain::ESCDown);
	PlayerInputComponent->BindAction("ESC", IE_Released, this, &AMain::ESCUp);
}

void AMain::MoveForward(float Value)
{
	//Set MovingForward to false otherwise it will only ever stay on true.  before the next checks
	bMovingForward = false;
	////check for null pointer and need to make sure key is pressed so that we want to move forward (value != 0.0f). can't be attacking.  also check Main is not dead
	if ((Controller != nullptr) && (Value != 0.0f) && (!bAttacking) && (MovementStatus != EMovementStatus::EMS_Dead))
	{
		//find out which way is forward 
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0.f, Rotation.Yaw, 0.f);

		//create rotation matrix from YawRotation and this has Unit Axes that we can access and one of these is X axis
		// in other words we are accessing the local axis rotation and want to move forward based on that (usually X is forward)
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X); 
		AddMovementInput(Direction, Value);

		bMovingForward = true;
	}
}

void AMain::MoveRight(float Value)
{
	//Set MovingForward to false otherwise it will only ever stay on true.  before the next checks
	bMovingRight = false;
	//check for null pointer and need to make sure key is pressed so that we want to move forward (value != 0.0f). can't be attacking.  also check Main is not Dead
	if ((Controller != nullptr) && (Value != 0.0f) && (!bAttacking) && (MovementStatus != EMovementStatus::EMS_Dead))
	{
		//find out which way is forward 
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0.f, Rotation.Yaw, 0.f);

		//doing same as above but just for Y axis instead of X	
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		AddMovementInput(Direction, Value);

		bMovingRight = true;
	}
}

void AMain::TurnAtRate(float Rate)
{
	AddControllerYawInput(Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds()); //need to include Engine/World/h to access GetWorld->GetDeltaSeconds
}

void AMain::LookUpAtRate(float Rate)
{
	AddControllerPitchInput(Rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
}

//Use this function to check that the player wants to equip the new weapon
void AMain::LMBDown()
{
	bLMBDown = true;

	if (MovementStatus == EMovementStatus::EMS_Dead) return;

	if (ActiveOverlappingItem)
	{
		AWeapon* Weapon = Cast<AWeapon>(ActiveOverlappingItem); //Cast in to AWeapon, pass in OverLappingItem
		if (Weapon) //check weapon is valid
		{
			Weapon->Equip(this); //Call Equip inside Weapon
			SetActiveOverlappingItem(nullptr);
		}
	}
	else if (EquippedWeapon) //if LMB is down and we have a weapon equipped, it means we want to attack so call Attack function
	{
		Attack();
	}
}

void AMain::ESCDown()
{
	bESCDown = true;

	if (MainPlayerController)
	{
		MainPlayerController->TogglePauseMenu();
	}
}

void AMain::ESCUp()
{
	bESCDown = false;
}

void AMain::LMBUp()
{
	bLMBDown = false;
}


void AMain::DecrementHealth(float Amount)
{

}

void AMain::IncrementCoins(int32 Amount)
{
	Coins += Amount;
}

void AMain::IncrementHealth(float Amount)
{
	if (Health + Amount >= MaxHealth)
	{
		Health = MaxHealth;
	}
	else
	{
		Health += Amount;
	}
}

void AMain::Die()
{
	if (MovementStatus == EMovementStatus::EMS_Dead) return; //check Main is not already in the middle of dying before proceeding
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && CombatMontage) //is valid
	{
		AnimInstance->Montage_Play(CombatMontage, 1.0f);
		AnimInstance->Montage_JumpToSection(FName("Death"));
	}
	SetMovementStatus(EMovementStatus::EMS_Dead);
}

void AMain::Jump()
{
	if (MovementStatus != EMovementStatus::EMS_Dead)
	{
		ACharacter::Jump();
	}
}

void AMain::DeathEnd()
{
	//stop character from moving after death
	GetMesh()->bPauseAnims = true;
	GetMesh()->bNoSkeletonUpdate = true;
}

void AMain::SetMovementStatus(EMovementStatus Status)
{
	MovementStatus = Status;
	if (MovementStatus == EMovementStatus::EMS_Sprinting)
	{
		GetCharacterMovement()->MaxWalkSpeed = SprintingSpeed;
	}
	else
	{
		GetCharacterMovement()->MaxWalkSpeed = RunningSpeed;
	}
}

void AMain::ShiftKeyDown()
{
	bShiftKeyDown = true;
}


void AMain::ShiftKeyUp()
{
	bShiftKeyDown = false;

}

void AMain::ShowPickupLocations()
{

	for (FVector Location : PickupLocations)
	{
		UKismetSystemLibrary::DrawDebugSphere(this, Location, 25.f, 8, FLinearColor::Red, 10.f, 0.5f);
	}
	
}

void AMain::SetEquippedWeapon(AWeapon* WeaponToSet)
{
	if (EquippedWeapon)
	{
		EquippedWeapon->Destroy();
	}

	EquippedWeapon = WeaponToSet;
}

void AMain::Attack()
{
	if (!bAttacking && MovementStatus != EMovementStatus::EMS_Dead) //ensure we are not in already Attacking state and not dead before next attack
	{
	
		bAttacking = true;
		SetInterpToEnemy(true);

		UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
		if (AnimInstance && CombatMontage) //is valid
		{
			int32 Section = FMath::RandRange(0, 1); //Randomize the attack animation played from Combat Montage using int32 variable called Section and switch statement
			switch (Section)
			{
			case 0:
				//Play the Anim Instance's montage Combat Montage at 2.2x speed, and skip directly to Attack_1
				AnimInstance->Montage_Play(CombatMontage, 2.2f);
				AnimInstance->Montage_JumpToSection(FName("Attack_1"), CombatMontage);
				break;
			case 1:
				AnimInstance->Montage_Play(CombatMontage, 1.8f);
				AnimInstance->Montage_JumpToSection(FName("Attack_2"), CombatMontage);
				break;

			default:
				;
			}
		}
	}
}

void AMain::AttackEnd()
{
	bAttacking = false;
	SetInterpToEnemy(false);
	if (bLMBDown)
	{
		Attack();
	}
}

void AMain::PlaySwingSound()
{
	if (EquippedWeapon->SwingSound)
	{
		UGameplayStatics::PlaySound2D(this, EquippedWeapon->SwingSound);
	}
	
}

void AMain::SetInterpToEnemy(bool Interp)
{
	bInterpToEnemy = Interp;
}

float AMain::TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser)
{
	if (Health - DamageAmount <= 0.f)
	{
		Health -= DamageAmount;
		Die();
		//Cast in to Damage Causer and stop them from attacking once Main dies
		if (DamageCauser)
		{
			AEnemy* Enemy = Cast<AEnemy>(DamageCauser);
			if (Enemy)
			{
				Enemy->bHasValidTarget = false;
			}
		}
	}
	else
	{
		Health -= DamageAmount;
	}

	return DamageAmount;
}

void AMain::UpdateCombatTarget()
{
	TArray<AActor*> OverlappingActors; //create array of overlapping actors
	GetOverlappingActors(OverlappingActors, EnemyFilter); //get overlapping actors, place them in array OverlappingActors and filter for Enemy actors

	if (OverlappingActors.Num() == 0)//if the array of overlapping actors has nothing, remove health bars
	{
		{
			MainPlayerController->RemoveEnemyHealthBar();
		}
		return;
	}

	AEnemy* ClosestEnemy = Cast<AEnemy>(OverlappingActors[0]); //variable to hold the closest enemy
	if (ClosestEnemy)
	{
		FVector Location = GetActorLocation(); //create a variable to be used as shortcut to GetActorLocation function for use in following lines 
		//create variable of type float called MinDistance from getting the ClosestEnemy's location subtracted by Main's location
		float MinDistance = (ClosestEnemy->GetActorLocation() - Location).Size();

		for (auto Actor : OverlappingActors) //loop through array of overlapping actors to check which is closest and set that Enemy to Closest Enemy
		{
			AEnemy* Enemy = Cast<AEnemy>(Actor);
			if (Enemy)
			{
				float DistanceToActor = (Enemy->GetActorLocation() - Location).Size();
				if (DistanceToActor < MinDistance)
				{
					MinDistance = DistanceToActor;
					ClosestEnemy = Enemy;
				}
			}
		}
		if (MainPlayerController)
		{
			MainPlayerController->DisplayEnemyHealthBar();
		}
		SetCombatTarget(ClosestEnemy);
		bHasCombatTarget = true;
	}
}

void AMain::SwitchLevel(FName LevelName)
{
	//Create local variable of type UWorld called World by calling GetWorld so we can get to GetMapName
	UWorld* World = GetWorld();
	if (World)
	{
		FString CurrentLevel = World->GetMapName();  //create variable of type FString called CurrentLevel because GetMapName returns an FString

		FName CurrentLevelName(*CurrentLevel); //Initialize FName CurrentLevelName using FString literal (* = dereference operator) from CurrentLevel
		if (CurrentLevelName != LevelName) //ensure Current level and next level to transition to are not the same
		{
			UGameplayStatics::OpenLevel(World, LevelName);
		}
	}
}

void AMain::SaveGame()
{
	//UE_LOG(LogTemp, Warning, TEXT("SAVING GAME"));
	/*
	Passing in what is returned from StaticClass (UClass) to CreateSaveGameObject (from GameplayStatics) which returns object of type USaveGame and casts to UFirstSaveGame and stores it all in
	SaveGameInstance so we have an instance of the SaveGame object
	*/
	UFirstSaveGame* SaveGameInstance = Cast<UFirstSaveGame>(UGameplayStatics::CreateSaveGameObject(UFirstSaveGame::StaticClass()));

	//Next store the Main's stats in the SaveGame instance's corresponding variables
	SaveGameInstance->CharacterStats.Health = Health;
	SaveGameInstance->CharacterStats.MaxHealth = MaxHealth;
	SaveGameInstance->CharacterStats.Stamina = Stamina;
	SaveGameInstance->CharacterStats.MaxStamina = MaxStamina;
	SaveGameInstance->CharacterStats.Coins = Coins;

	if (EquippedWeapon)
	{
		SaveGameInstance->CharacterStats.WeaponName = EquippedWeapon->Name; //Get the WeaponName's string and save in SaveGameInstance WeaponName (so we can have a default value upon load)
	}

	SaveGameInstance->CharacterStats.Location = GetActorLocation();
	SaveGameInstance->CharacterStats.Rotation = GetActorRotation();

	//Use the SaveGame instance we have in memory as input to SaveGameToSlot
	UGameplayStatics::SaveGameToSlot(SaveGameInstance, SaveGameInstance->PlayerName, SaveGameInstance->UserIndex);

	//UE_LOG(LogTemp, Warning, TEXT("GAME SAVED"));
}

void AMain::LoadGame(bool SetPosition)
{
	//UE_LOG(LogTemp, Warning, TEXT("LOADING GAME"));

	//Retrieve instance of type UFirstSaveGame
	UFirstSaveGame* LoadGameInstance = Cast<UFirstSaveGame>(UGameplayStatics::CreateSaveGameObject(UFirstSaveGame::StaticClass()));

	//Use LoadGameInstance as input for LoadGameFromSlot, then cast to UFirstSaveGame, then data returned overwrites LoadGameInstance.  (Compiler goes from right to left in this case)
	LoadGameInstance = Cast<UFirstSaveGame>(UGameplayStatics::LoadGameFromSlot(LoadGameInstance->PlayerName, LoadGameInstance->UserIndex));

	//Set Main's stats to those in the loaded game instance
	Health = LoadGameInstance->CharacterStats.Health;
	MaxHealth = LoadGameInstance->CharacterStats.MaxHealth;
	Stamina = LoadGameInstance->CharacterStats.Stamina;
	MaxStamina = LoadGameInstance->CharacterStats.MaxStamina;
	Coins = LoadGameInstance->CharacterStats.Coins;

	//Retrieve weapon from SavedGame instance and equip
	if (WeaponStorage)
	{
		//Spawn actor of type ItemStorage called Weapons
		AItemStorage* Weapons = GetWorld()->SpawnActor<AItemStorage>(WeaponStorage); //WeaponStorage is stored on Main
		if (Weapons) //Check if it's valid
		{
			//retreive WeaponName from SaveGameInstance and store in FString called WeaponName
			FString WeaponName = LoadGameInstance->CharacterStats.WeaponName;

			//put this check here so that we don't crash if the map does not contain a key called WeaponName.  similar to why we would crash if we tried to access 5th item in array of 4
			if (Weapons->WeaponMap.Contains(WeaponName)) 
			{
				/*Take newly spawned actor ItemStorage, access its WeaponMap with WeaponName string we just retrieved. Returns a UClass which we pass in to SpawnActor to spawn a new Weapon based
				* on that UClass and store in variable of type AWeapon called WeaponToEquip which we then call Equip on (from Weapon.h)
				*/
				AWeapon* WeaponToEquip = GetWorld()->SpawnActor<AWeapon>(Weapons->WeaponMap[WeaponName]); //Spawn the weapon
				WeaponToEquip->Equip(this);
			}
		}
	}

	//if we want to set Main's position
	if (SetPosition)
	{
		SetActorRotation(LoadGameInstance->CharacterStats.Rotation);
		SetActorLocation(LoadGameInstance->CharacterStats.Location);
	}

	//UE_LOG(LogTemp, Warning, TEXT("GAME LOADED"));
}
