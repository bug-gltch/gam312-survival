// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Camera/CameraComponent.h"
#include "Resource_M.h"
#include "Kismet/GameplayStatics.h"
#include "BuildingPart.h"
#include "PlayerWidget.h"
#include "PlayerCharacter.generated.h"

UCLASS()
class SURVIVALPROJECT_API APlayerCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	APlayerCharacter();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UFUNCTION() // back and forth
	void MoveForward(float axisValue);

	UFUNCTION() // left and right
	void MoveRight(float axisValue);

	UFUNCTION() // jump
	void JumpBegin();

	UFUNCTION()
	void NoJump();

	UFUNCTION() // interact
	void FindObj();

	UPROPERTY(VisibleAnywhere) // cameraaa
	UCameraComponent* PlayerCam;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PlayerStats") // stats
	float HP = 100.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PlayerStats")
	float Hunger = 100.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PlayerStats")
	float Stamina = 100.0f;

	UPROPERTY(EditAnywhere, Category = "Resources") // resources
	int Wood;

	UPROPERTY(EditAnywhere, Category = "Resources")
	int Berry;

	UPROPERTY(EditAnywhere, Category = "Resources")
	int Stone;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Resources")
	TArray<int> ResourcesArray;

	UPROPERTY(EditAnywhere, Category = "Resources")
	TArray<FString> ResourcesNameArray;

	UPROPERTY(EditAnywhere, Category = "HitMarker")
	UMaterialInterface* hitDecal;

	UPROPERTY(EditAnywhere, BlueprintReadwrite, Category = "Building Supplies")
	TArray<int> BuildingArray;

	UPROPERTY()
	bool isBuilding;

	UPROPERTY(EditDefaultsOnly, BlueprintReadwrite)
	TSubclassOf<ABuildingPart> BuildPartClass;

	UPROPERTY()
	ABuildingPart* spawnedPart;

	UPROPERTY(EditAnywhere, BlueprintReadwrite)
	UPlayerWidget* playerUI;

	UFUNCTION(BlueprintCallable) // stat callers
	void SetHP(float amount);

	UFUNCTION(BlueprintCallable)
	void SetHunger(float amount);

	UFUNCTION(BlueprintCallable)
	void SetStamina(float amount);

	UFUNCTION()
	void LowerStats();

	UFUNCTION()
	void GiveResource(float amount, FString resourceType);

	UFUNCTION(BlueprintCallable)
	void UpdateResources(float woodAmount, float stoneAmount, FString buildingObject);

	UFUNCTION(BlueprintCallable)
	void SpawnBuilding(int buildingID, bool& isSuccess);

	UFUNCTION()
	void RotateBuilding();

};
