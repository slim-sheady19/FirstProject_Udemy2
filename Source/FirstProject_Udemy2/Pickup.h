// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Item.h"
#include "Pickup.generated.h"

/**
 * 
 */
UCLASS()
class FIRSTPROJECT_UDEMY2_API APickup : public AItem
{
	GENERATED_BODY()
public:


	APickup(); //constructor declaration

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Damage")
	int32 CoinCount;

	//we do not put UFUNCTION macro here because it is already inherited from parent class Item
	virtual void OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) override;
	virtual void OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex) override;
	
};
