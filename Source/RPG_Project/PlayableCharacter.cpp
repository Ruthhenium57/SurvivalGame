// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayableCharacter.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputMappingContext.h"
#include "InputAction.h"
#include "MainHUDWidget.h"
#include "HealthBarWidget.h"
#include "StaminaBarWidget.h"
#include "HungerBarWidget.h"
#include "ThirstBarWidget.h"
#include "InteractionInfoWidget.h"
#include "GameHUD.h"
#include "GameFramework/PlayerController.h"
#include "Components/InputComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "TimerManager.h"

APlayableCharacter::APlayableCharacter()
{
	FirstPersonCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera"));
	FirstPersonCamera->SetupAttachment(GetMesh(), TEXT("Head"));

	PlayerStatsComp = CreateDefaultSubobject<UPlayerStatsComp>(TEXT("PlayerStatsComp"));
	InventoryComponent = CreateDefaultSubobject<UInventoryComponent>(TEXT("InventoryComponent"));

	DefaultWalkSpeed = 600.0f;
	SprintSpeed = 1200.0f;
	bIsSprinting = false;
	StaminaRegenDelay = 1.5f;
	NeedStaminaToJump = 12.0f;

	BaseTurnRate = 45.0f;
	BaseLookUpRate = 45.0f;

	InteractionDistance = 400.0f;
}

void APlayableCharacter::BeginPlay()
{
	Super::BeginPlay();

	GetCharacterMovement()->MaxWalkSpeed = DefaultWalkSpeed;
	
	APlayerController* PlayerController = Cast<APlayerController>(GetController());
	if (PlayerController)
	{
		AGameHUD* GameHUD = Cast<AGameHUD>(PlayerController->GetHUD());
		if (GameHUD)
		{
			MainHUDWidget = GameHUD->MainHUDWidget;
		}
	}

	if (PlayerStatsComp)
	{
		PlayerStatsComp->OnStaminaEnd.AddDynamic(this, &APlayableCharacter::OnStaminaEnd);
	}
}

void APlayableCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	UpdateStaminaBar();
	UpdateHealthBar();
	UpdateHungerBar();
	UpdateThirstBar();
	UpdateInteractInfo();
}

void APlayableCharacter::SetupPlayerInputComponent(UInputComponent * MainPlayerInput)
{
	Super::SetupPlayerInputComponent(MainPlayerInput);

	
	MainPlayerInput->BindAxis("MoveForward", this, &APlayableCharacter::MoveForward);
	MainPlayerInput->BindAxis("MoveRight", this, &APlayableCharacter::MoveRight);
	
	MainPlayerInput->BindAxis("LookUpDown", this, &APlayableCharacter::Look);
	MainPlayerInput->BindAxis("Turn", this, &APlayableCharacter::Turn);
	
	MainPlayerInput->BindAction("Jump", IE_Pressed, this, &APlayableCharacter::Jump);
	MainPlayerInput->BindAction("Jump", IE_Released, this, &APlayableCharacter::StopJump);

	MainPlayerInput->BindAction("Sprint", IE_Pressed, this, &APlayableCharacter::Sprint);
	MainPlayerInput->BindAction("Sprint", IE_Released, this, &APlayableCharacter::StopSprint);


	MainPlayerInput->BindAction("Interact", IE_Pressed, this, &APlayableCharacter::Interact);
	MainPlayerInput->BindAction("PutItemToStorage", IE_Pressed, this, &APlayableCharacter::PutItemToStorage);
}

void APlayableCharacter::MoveForward(float Value)
{
	if (Value != 0.0f)
	{
		AddMovementInput(GetActorForwardVector(), Value);
	}
}

void APlayableCharacter::MoveRight(float Value)
{
	FVector Direction = FRotationMatrix(Controller->GetControlRotation()).GetScaledAxis(EAxis::Y);
	AddMovementInput(Direction, Value);
}

void APlayableCharacter::Look(float Value)
{
	AddControllerPitchInput(Value * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
}

void APlayableCharacter::Turn(float Value)
{
	AddControllerYawInput(Value * BaseTurnRate * GetWorld()->GetDeltaSeconds());
}

void APlayableCharacter::Jump()
{
	if (PlayerStatsComp->CurrentStamina > NeedStaminaToJump)
	{
		PlayerStatsComp->bCanStaminaRegen = false;
		bPressedJump = true;
		if (HasAuthority())
		{
			PlayerStatsComp->OneTimeStaminaReduction(NeedStaminaToJump);
			PlayerStatsComp->MulticastOneTimeStaminaReduction(NeedStaminaToJump);
		}
		else
		{
			PlayerStatsComp->ServerOneTimeStaminaReduction_Implementation(NeedStaminaToJump);
		}
	}
}

void APlayableCharacter::StopJump()
{
	bPressedJump = false;
	PlayerStatsComp->bCanStaminaRegen = true;
}

void APlayableCharacter::OnStaminaEnd()
{
	StopSprint();
}

void APlayableCharacter::SetWalkSpeed(float NewSpeed)
{
	DefaultWalkSpeed = NewSpeed;
	if (!bIsSprinting)
	{
		GetCharacterMovement()->MaxWalkSpeed = DefaultWalkSpeed;
	}
}

void APlayableCharacter::SetSprintSpeed(float NewSpeed)
{
	SprintSpeed = NewSpeed;
}

void APlayableCharacter::Sprint()
{
	if (PlayerStatsComp && PlayerStatsComp->CurrentStamina > 0)
	{
		GetWorld()->GetTimerManager().ClearTimer(StaminaRegenTimerHandle);
		bIsSprinting = true;
		PlayerStatsComp->StartSprint();
		if (HasAuthority())
		{
			GetCharacterMovement()->MaxWalkSpeed = SprintSpeed;
			MulticastSetSprintSpeed(SprintSpeed);
		}
		else
		{
			ServerSetSprintSpeed(SprintSpeed);
		}
	}
}

void APlayableCharacter::StopSprint()
{
	bIsSprinting = false;
	PlayerStatsComp->StopSprint();
	GetWorld()->GetTimerManager().SetTimer(StaminaRegenTimerHandle, this, &APlayableCharacter::BeginStaminaRegen, StaminaRegenDelay, false);
	if (HasAuthority())
	{
		GetCharacterMovement()->MaxWalkSpeed = DefaultWalkSpeed;
		MulticastSetSprintSpeed(DefaultWalkSpeed);
	}
	else
	{
		ServerSetSprintSpeed(DefaultWalkSpeed);
	}
}

void APlayableCharacter::UpdateHealthBar()
{
	if (PlayerStatsComp && MainHUDWidget)
	{
		float HealthPercentage = PlayerStatsComp->CurrentHealth / PlayerStatsComp->MaxHealth;
		MainHUDWidget->HealthBarWidget->SetHealth(HealthPercentage);
	}
}

void APlayableCharacter::UpdateStaminaBar()
{
	if (PlayerStatsComp && MainHUDWidget)
	{
		float StaminaPercentage = PlayerStatsComp->CurrentStamina / PlayerStatsComp->MaxStamina;
		MainHUDWidget->StaminaBarWidget->SetStamina(StaminaPercentage);
	}
}

void APlayableCharacter::UpdateThirstBar()
{
	if (PlayerStatsComp && MainHUDWidget)
	{
		float ThirstPercentage = PlayerStatsComp->CurrentThirst / PlayerStatsComp->MaxThirst;
		MainHUDWidget->ThirstBarWidget->SetThirst(ThirstPercentage);
	}
}

void APlayableCharacter::UpdateHungerBar()
{
	if (PlayerStatsComp && MainHUDWidget)
	{
		float HungerPercentage = PlayerStatsComp->CurrentHunger / PlayerStatsComp->MaxHunger;
		MainHUDWidget->HungerBarWidget->SetHunger(HungerPercentage);
	}
}

void APlayableCharacter::UpdateInteractInfo()
{
	if (MainHUDWidget)
	{
		FHitResult HitResult;
		PerformLineTrace(HitResult);
		AActor* HitActor = HitResult.GetActor();
		if (HitActor && HitActor->GetClass()->ImplementsInterface(UInteractableInterface::StaticClass()))
		{
			IInteractableInterface* Interactable = Cast<IInteractableInterface>(HitActor);
			if (Interactable)
			{
				MainHUDWidget->InteractionInfoWidget->ShowInteractInfo(Interactable->InteractTextBlockName);
				if (!Interactable->InteractTextBlockName2.IsEmpty())
				{
					MainHUDWidget->InteractionInfoWidget->ShowInteractInfo(Interactable->InteractTextBlockName2);
					return;
				}
				return;
			}
		}
		MainHUDWidget->InteractionInfoWidget->HideInteractInfo();
	}
}

void APlayableCharacter::BeginStaminaRegen()
{
	PlayerStatsComp->bCanStaminaRegen = true;
}

void APlayableCharacter::Interact()
{
	FHitResult HitResult;
	PerformLineTrace(HitResult);

	if (HitResult.GetActor() && HitResult.GetActor()->Implements<UInteractableInterface>())
	{
		if (HasAuthority())
		{
			ServerInteract(HitResult.GetActor());
		}
		else
		{
			ServerInteract(HitResult.GetActor());
		}
	}
}

void APlayableCharacter::ServerInteract_Implementation(AActor* HitActor)
{
	if (HitActor && HitActor->Implements<UInteractableInterface>())
	{
		IInteractableInterface* Interactable = Cast<IInteractableInterface>(HitActor);
		if (Interactable)
		{
			//UE_LOG(LogTemp, Warning, TEXT("CharacterInteract"));
			Interactable->Interact(this);
		}
	}
}

bool APlayableCharacter::ServerInteract_Validate(AActor* HitActor)
{
	return true;
}

void APlayableCharacter::PutItemToStorage()
{
	FHitResult HitResult;
	PerformLineTrace(HitResult);

	if (HitResult.GetActor() && HitResult.GetActor()->Implements<UInteractableInterface>())
	{
		if (HasAuthority())
		{
			ServerPutItemToStorage(HitResult.GetActor());
		}
		else
		{
			ServerPutItemToStorage(HitResult.GetActor());
		}
	}
}

void APlayableCharacter::ServerPutItemToStorage_Implementation(AActor* HitActor)
{
	if (HitActor && HitActor->Implements<UInteractableInterface>())
	{
		IInteractableInterface* Interactable = Cast<IInteractableInterface>(HitActor);
		if (Interactable)
		{
			//UE_LOG(LogTemp, Warning, TEXT("CharacterTakeItem"));
			Interactable->PutItemToStorage(this);
		}
	}
}

bool APlayableCharacter::ServerPutItemToStorage_Validate(AActor* HitActor)
{
	return true;
}

void APlayableCharacter::PerformLineTrace(FHitResult& HitResult)
{
	FVector Start = FirstPersonCamera->GetComponentLocation();
	FVector ForwardVector = FirstPersonCamera->GetForwardVector();
	FVector End = ((ForwardVector * InteractionDistance) + Start);
	FCollisionQueryParams CollisionParams;
	CollisionParams.AddIgnoredActor(this);

	//DrawDebugLine(GetWorld(), Start, End, FColor::Red, false, 2, 0, 1);
	GetWorld()->LineTraceSingleByChannel(HitResult, Start, End, ECC_Visibility, CollisionParams);
}

void APlayableCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	//DOREPLIFETIME(AMyCharacter, Health);
}

void APlayableCharacter::ServerSetSprintSpeed_Implementation(float NewSpeed)
{
	GetCharacterMovement()->MaxWalkSpeed = NewSpeed;
	MulticastSetSprintSpeed(NewSpeed);
}

bool APlayableCharacter::ServerSetSprintSpeed_Validate(float NewSpeed)
{
	return true;
}

void APlayableCharacter::MulticastSetSprintSpeed_Implementation(float NewSpeed)
{
	if (!HasAuthority())
	{
		GetCharacterMovement()->MaxWalkSpeed = NewSpeed;
	}
}