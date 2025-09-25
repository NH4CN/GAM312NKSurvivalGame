// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerChar.h"

// Sets default values
APlayerChar::APlayerChar()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
	// Initial setup of the camera
	PlayerCameraComp = CreateDefaultSubobject<UCameraComponent>(TEXT("First Person Cam"));
	
	// Attaching camera to the character mesh
	PlayerCameraComp->SetupAttachment(GetMesh(), "head");

	// Share rotation with controller
	PlayerCameraComp->bUsePawnControlRotation = true;

	ResourcesArray.SetNum(3);
	ResourceNameArray.Add("Wood");
	ResourceNameArray.Add("Stone");
	ResourceNameArray.Add("Berry");
	
}

// Called when the game starts or when spawned
void APlayerChar::BeginPlay()
{
	Super::BeginPlay();
	FTimerHandle StatsTimerHandle;
	GetWorld()->GetTimerManager().SetTimer(StatsTimerHandle, this, &APlayerChar::DecreaseStats, 2.f, true);
}

// Called every frame
void APlayerChar::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void APlayerChar::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	PlayerInputComponent->BindAxis("MoveForward", this, &APlayerChar::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &APlayerChar::MoveRight);
	PlayerInputComponent->BindAxis("LookUp", this, &APlayerChar::AddControllerPitchInput);
	PlayerInputComponent->BindAxis("Turn", this, &APlayerChar::AddControllerYawInput);
	PlayerInputComponent->BindAction("JumpEvent", IE_Pressed, this, &APlayerChar::StartJump);
	PlayerInputComponent->BindAction("JumpEvent", IE_Released, this, &APlayerChar::StopJump);
	PlayerInputComponent->BindAction("Interact", IE_Pressed, this, &APlayerChar::FindObject);
}

void APlayerChar::MoveForward(float axisValue)
{
	FVector Direction = FRotationMatrix(Controller->GetControlRotation()).GetScaledAxis(EAxis::X);
	AddMovementInput(Direction, axisValue);
}

void APlayerChar::MoveRight(float axisValue)
{
	FVector Direction = FRotationMatrix(Controller->GetControlRotation()).GetScaledAxis(EAxis::Y);
	AddMovementInput(Direction, axisValue);
}

void APlayerChar::StartJump()
{
	bPressedJump = true;
}

void APlayerChar::StopJump()
{
	bPressedJump = false;
}

void APlayerChar::FindObject()
{
	FHitResult HitResult;
	FVector StartLocation = PlayerCameraComp->GetComponentLocation();
	FVector Direction = PlayerCameraComp->GetForwardVector() * 800.0f;
	FVector EndLocation = StartLocation + Direction;
	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(this);
	QueryParams.bTraceComplex = true;
	QueryParams.bReturnFaceIndex = true;
	if (GetWorld()->LineTraceSingleByChannel(HitResult, StartLocation, EndLocation, ECC_Visibility, QueryParams))
	{
		if (Stamina > 5.0f)
		{
			AResource_M* HitResource = Cast<AResource_M>(HitResult.GetActor());
			if (HitResource)
			{
				FString hitName = HitResource->resourceName;
				int resourceValue = HitResource->resourceAmount;
				HitResource->totalResource = HitResource->totalResource - resourceValue;
				if (HitResource->totalResource > resourceValue)
				{
					GiveResource(resourceValue, hitName);
					check(GEngine != nullptr);
					GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, TEXT("Resource Collected"));
					UGameplayStatics::SpawnDecalAtLocation(GetWorld(), hitDecal, FVector(10.0f, 10.0f, 10.0f), HitResult.Location, FRotator(-90, 0, 0), 2.0f);
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
}

void APlayerChar::SetHealth(float amount)
{
	if (Health + amount < 100)
	{
		Health = Health + amount;
	}
}

void APlayerChar::SetHunger(float amount)
{
	if (Hunger + amount < 100)
	{
		Hunger = Hunger + amount;
	}
}

void APlayerChar::SetStamina(float amount)
{
	if (Stamina + amount < 100)
	{
		Stamina = Stamina + amount;
	}
}


void APlayerChar::DecreaseStats()
{
	if (Hunger > 0)
	{
		SetHunger(-1.0f);
	}
	SetStamina(10.0f);
	if (Hunger <= 0)
	{
		SetHealth(-3.0f);
	}
}

void APlayerChar::GiveResource(float amount, FString resourceType)
{
	if (resourceType == "Wood")
	{
		ResourcesArray[0] = ResourcesArray[0] + amount;
	}
	if (resourceType == "Stone")
	{
		ResourcesArray[1] = ResourcesArray[1] + amount;
	}
	if (resourceType == "Berry")
	{
		ResourcesArray[2] = ResourcesArray[2] + amount;
	}
}

