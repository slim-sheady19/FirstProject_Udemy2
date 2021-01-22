// Fill out your copyright notice in the Description page of Project Settings.


#include "MyObject.h"

UMyObject::UMyObject() //defining constructor (default values)
{
	MyFloat = 0.f;
}

void UMyObject::MyFunction()
{
	UE_LOG(LogTemp, Warning, TEXT("This is our warning text!")); // create a warning
}