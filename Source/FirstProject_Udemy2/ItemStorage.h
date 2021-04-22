// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ItemStorage.generated.h"

UCLASS()
class FIRSTPROJECT_UDEMY2_API AItemStorage : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AItemStorage();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	

	/*instead of an array we are using a map.  An array is actually a type of map, where the list of values are denoted by integerts [0, 1, 2] so forth.
	* A map uses keys to denote values.  Keys can be strings or other things.
	* So below we are creating a Map (called WeaponMap) and we create a template <[key type], [value type]> in this case <FString, TsubClassof AWeapon>.  Also exposing to BP 
	*/
	UPROPERTY(EditDefaultsOnly, Category = "SaveData")
	TMap<FString, TSubclassOf<class AWeapon>> WeaponMap;


};
