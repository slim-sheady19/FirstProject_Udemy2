// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "MyObject.generated.h"

/**
 * 
 */
UCLASS(Blueprintable) //Blueprintable lets it be made in to BP
class FIRSTPROJECT_UDEMY2_API UMyObject : public UObject
{
	GENERATED_BODY()
public:
	UMyObject(); //constructor

	UPROPERTY(BlueprintReadOnly, Category = "MyVariables") //macro.  lets us access from BP/  added category My Variables / made it read only
	float MyFloat;
	
	UFUNCTION(BlueprintCallable, Category = "MyFunctions") //macro.  lets us access from BP
	void MyFunction();
};
