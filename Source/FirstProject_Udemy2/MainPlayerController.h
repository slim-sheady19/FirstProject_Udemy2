// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "MainPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class FIRSTPROJECT_UDEMY2_API AMainPlayerController : public APlayerController
{
	GENERATED_BODY()
public:

	//Reference to the UMG asset in the editor.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Widgets")
	TSubclassOf<class UUserWidget> HUDOverlayAsset;

	//Variable to hold the widget after creating it.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Widgets")
	UUserWidget* HUDOverlay;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Widgets")
	TSubclassOf<UUserWidget> WEnemyHealthBar;

	//Variable for enemy health bar widget (goal is to make it only appear when player is in combatsphere and disappear after leaving agrosphere)
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Widgets")
	UUserWidget* EnemyHealthBar;

	//Variable to hold the widget after creating it.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Widgets")
	UUserWidget* PauseMenu;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Widgets")
	TSubclassOf<UUserWidget> WPauseMenu;

	bool bPauseMenuVisible;

	//functions to show or hide Pause Menu.  Note when you add the macro "BlueprintNativeEvent" you have to add _Implementation to the function in the cpp file
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "HUD")
	void DisplayPauseMenu();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "HUD")
	void RemovePauseMenu();

	void TogglePauseMenu();

	bool bEnemyHealthBarVisible;

	//functions to show or hide enemy health bar
	void DisplayEnemyHealthBar();
	void RemoveEnemyHealthBar();

	FVector EnemyLocation;

protected:
	virtual void BeginPlay() override;
	
	//override Tick (Player controller class is an Actor so it inherits Tick)
	virtual void Tick(float DeltaTime) override;
};
