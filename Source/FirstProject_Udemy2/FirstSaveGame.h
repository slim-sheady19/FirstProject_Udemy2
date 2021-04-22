// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "FirstSaveGame.generated.h"

USTRUCT(BlueprintType) //USTRUCT allows us to use the reflection system.  BlueprintType exposes to BP
struct FCharacterStats //The F is a convention for structs in UE.
{
	GENERATED_BODY() //Needs this macro to generate "boilerplate code" that takes care of behind the scenes functionality also used for reflection system

	UPROPERTY(VisibleAnywhere, Category = "SaveGameData")
	float Health;

	UPROPERTY(VisibleAnywhere, Category = "SaveGameData")
	float MaxHealth;

	UPROPERTY(VisibleAnywhere, Category = "SaveGameData")
	float Stamina;

	UPROPERTY(VisibleAnywhere, Category = "SaveGameData")
	float MaxStamina;

	UPROPERTY(VisibleAnywhere, Category = "SaveGameData")
	int32 Coins;

	UPROPERTY(VisibleAnywhere, Category = "SaveGameData")
	FVector Location;

	UPROPERTY(VisibleAnywhere, Category = "SaveGameData")
	FRotator Rotation;

};

/**
 * 
 */
UCLASS()
class FIRSTPROJECT_UDEMY2_API UFirstSaveGame : public USaveGame
{
	GENERATED_BODY()

public:

	UFirstSaveGame();

	UPROPERTY(VisibleAnywhere, Category = Basic) //Must have category = basic for Save Game class
	FString PlayerName;

	UPROPERTY(VisibleAnywhere, Category = Basic)
	uint32 UserIndex;

	UPROPERTY(VisibleAnywhere, Category = Basic)
	FCharacterStats CharacterStats; //refers to struct above where all the stats data are contained
	
};

/*
* 
* SR
Why use structs?
Can you explain what the benefit of using a struct is compared to just creating variables inside of the object?
I plan to make a tower defense game for my class I am taking, so would storing variables like damage, range, firerate, etc in a struct be better than
storing them in the main area(I can't think of the proper term) of the header file? Why or why not?

Stephen — Instructor

Hi Samuel,

Structs are useful for storing information in a neat little package. They're by no means necessary, but sometimes it can be useful.
When a function takes a lot of parameters, it starts getting unwieldy, and passing in a single struct is sometimes cleaner.
So it really just depends. I show you how to use a struct in the lectures more for educational purposes, so you can decide what's best for your game.
When you have a Save Game object, it's not entirely important to use structs, since the object itself is a sort of package holding your data,
but you could characterize data into smaller categories and use structs for each one, if you wanted.

So what I gather from your explanation, structs are useful for keeping variables that have similar properties together,
such as health, stamina, damage, etc like player stats. I see it could also be useful for functions in a class that require multiple variables from another class,
for example, if AEnemy had a function that referenced AMain's health, stamina, coins, etc.
I'm still a little confused on that second part, if I am understanding it correctly.
From what we've done through most of the course, we simply have a reference to the other class, such as AMain in AEnemy, rather than passing inputs into functions.
Could you just clarify this for me?

Sure, what you're saying is an example of when you could use structs.
Technically speaking, structs are a C++ idea. When to use them depends on your coding philosophy. Let's talk a little bit about what a struct is.
In C++, a struct and a class are identical in all ways except one: The body of a class is private by default,
and the body of a struct is public by default. So technically, in C++, a struct is capable of all things a class is capable of (inheritance, polymorphism, member variables and functions, etc.).
In Unreal Engine, classes are primarily used to do the heavy lifting, but structs are also frequently employed, particularly for math utilities
(the FVector and FRotator are structs... and they have variables and functions).
They are used to contain a collection of parameters to pass into a function that needs them... FActorSpawnParameters is a struct that contains... well... spawn parameters for the SpawnActor function.
So the way they're used in Unreal Engine are typically for smaller objects like FVectors and packages of data like FActorSpawnParameters, etc.
*/