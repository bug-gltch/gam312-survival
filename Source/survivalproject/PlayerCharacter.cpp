// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerCharacter.h"

// Sets default values
APlayerCharacter::APlayerCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	PlayerCam = CreateDefaultSubobject<UCameraComponent>(TEXT("FP cam")); // camera

	PlayerCam->SetupAttachment(GetMesh(), "head");

	PlayerCam->bUsePawnControlRotation = true; // head attachment and player control

	BuildingArray.SetNum(3);
	ResourcesArray.SetNum(3); // array establish
	ResourcesNameArray.Add(TEXT("Wood"));
	ResourcesNameArray.Add(TEXT("Berry"));
	ResourcesNameArray.Add(TEXT("Stone"));
}

// Called when the game starts or when spawned
void APlayerCharacter::BeginPlay()
{
	Super::BeginPlay();
	
	FTimerHandle StatsTimerHandle; // timer for stat lowering
	GetWorld()->GetTimerManager().SetTimer(StatsTimerHandle, this, &APlayerCharacter::LowerStats, 2.0f, true); 

}

// Called every frame
void APlayerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (isBuilding)
	{
		if (spawnedPart) {
			FVector StartLocation = PlayerCam->GetComponentLocation();
			FVector Direction = PlayerCam->GetForwardVector() * 400.0f;
			FVector EndLocation = StartLocation + Direction;
			spawnedPart->SetActorLocation(EndLocation);
		}
	}
}

// Called to bind functionality to input
void APlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	PlayerInputComponent->BindAxis("MoveForward", this, &APlayerCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &APlayerCharacter::MoveRight);
	PlayerInputComponent->BindAxis("LookUp", this, &APlayerCharacter::AddControllerPitchInput);
	PlayerInputComponent->BindAxis("Turn", this, &APlayerCharacter::AddControllerYawInput);
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &APlayerCharacter::JumpBegin);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &APlayerCharacter::NoJump);
	PlayerInputComponent->BindAction("Interact", IE_Pressed, this, &APlayerCharacter::FindObj);
	PlayerInputComponent->BindAction("RotPart", IE_Pressed, this, &APlayerCharacter::RotateBuilding);

}

void APlayerCharacter::MoveForward(float axisValue)
{
	FVector Direction = FRotationMatrix(Controller->GetControlRotation()).GetScaledAxis(EAxis::X);
	Direction = FVector(Direction.X, Direction.Y, 0.f).GetSafeNormal(); // normalize stats
	AddMovementInput(Direction, axisValue);
}

void APlayerCharacter::MoveRight(float axisValue)
{
	FVector Direction = FRotationMatrix(Controller->GetControlRotation()).GetScaledAxis(EAxis::Y);
	AddMovementInput(Direction, axisValue);
}

void APlayerCharacter::JumpBegin()
{
	bPressedJump = true;
}

void APlayerCharacter::NoJump()
{
	bPressedJump = false;
}

void APlayerCharacter::FindObj()
{
	FHitResult HitResult;
	FVector StartLocation = PlayerCam->GetComponentLocation();
	FVector Direction = PlayerCam->GetForwardVector() * 800.0f;
	FVector EndLocation = StartLocation + Direction;

	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(this);
	QueryParams.bTraceComplex = true;
	QueryParams.bReturnFaceIndex = true;

	if (!isBuilding) {
		if (GetWorld()->LineTraceSingleByChannel(HitResult, StartLocation, EndLocation, ECC_Visibility, QueryParams)) // HIT ROCK
		{
			AResource_M* HitResource = Cast<AResource_M>(HitResult.GetActor());

			if (HitResource)
			{
				FString hitName = HitResource->resourceName;
				int resourceValue = HitResource->resourceAmt;

				HitResource->totalResource = HitResource->totalResource - resourceValue;

				if (HitResource->totalResource > resourceValue)
				{
					GiveResource(resourceValue, hitName);

					check(GEngine != nullptr);
					GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, TEXT("Resource Collected")); // mining :)

					UGameplayStatics::SpawnDecalAtLocation(GetWorld(), hitDecal, FVector(20.0f, 20.0f, 20.0f), HitResult.Location, FRotator(-90, 0, 0), 2.0f);
					SetStamina(-5.0f);
				}
				else
				{
					HitResource->Destroy();
					check(GEngine != nullptr);
					GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, TEXT("Resource Depleted"));
				}
			}
		}
	}

	else {
		isBuilding = false;
	}
}

void APlayerCharacter::SetHP(float amount) // health
{
	if (HP + amount < 100)
	{
		HP = HP + amount;
	}

	if (HP > 100)
	{
		HP = 100;
	}
}

void APlayerCharacter::SetHunger(float amount) // hunger
{
	if (Hunger + amount < 100)
	{
		Hunger = Hunger + amount;
	}

	if (Hunger > 100)
	{
		Hunger = 100;
	}
}

void APlayerCharacter::SetStamina(float amount) // stamina
{
	if (Stamina + amount < 100)
	{
		Stamina = Stamina + amount;
	}

	if (Stamina > 100)
	{
		Stamina = 100;
	}
}

void APlayerCharacter::LowerStats() // stat changes
{
	if (Hunger > 0)
	{
		SetHunger(-1.0f);
	}
	SetStamina(7.0f);

	if (Hunger <= 0)
	{
		SetHP(-3.0f);
	}
}

void APlayerCharacter::GiveResource(float amount, FString resourceType)
{
	if (resourceType == "Wood")
	{
		ResourcesArray[0] = ResourcesArray[0] + amount;
	}

	if (resourceType == "Berry")
	{
		ResourcesArray[1] = ResourcesArray[1] + amount;
	}

	if (resourceType == "Stone")
	{
		ResourcesArray[2] = ResourcesArray[2] + amount;
	}
}

void APlayerCharacter::UpdateResources(float woodAmount, float stoneAmount, FString buildingObject)
{
	if (woodAmount <= ResourcesArray[0])
	{
		if (stoneAmount <= ResourcesArray[2])
		{
			ResourcesArray[0] = ResourcesArray[0] - woodAmount;
			ResourcesArray[2] = ResourcesArray[2] - stoneAmount;

			if (buildingObject == "Wall")
			{
				BuildingArray[0] = BuildingArray[0] + 1;
			}

			if (buildingObject == "Floor")
			{
				BuildingArray[1] = BuildingArray[1] + 1;
			}

			if (buildingObject == "Ceiling")
			{
				BuildingArray[2] = BuildingArray[2] + 1;
			}
		}
	}
}

void APlayerCharacter::SpawnBuilding(int buildingID, bool& isSuccess)
{
	if (!isBuilding)
	{
		if (BuildingArray[buildingID] >= 1)
		{
			isBuilding = true;
			FActorSpawnParameters SpawnParams;
			FVector StartLocation = PlayerCam->GetComponentLocation();
			FVector Direction = PlayerCam->GetForwardVector() * 400.0f;
			FVector EndLocation = StartLocation + Direction;
			FRotator myRot(0, 0, 0);

			BuildingArray[buildingID] = BuildingArray[buildingID] - 1;

			spawnedPart = GetWorld()->SpawnActor<ABuildingPart>(BuildPartClass, EndLocation, myRot, SpawnParams);

			isSuccess = true;
		}
		isSuccess = false;
	}
}

void APlayerCharacter::RotateBuilding()
{
	if (isBuilding)
	{
		spawnedPart->AddActorWorldRotation(FRotator(0, 90, 0));
	}
}