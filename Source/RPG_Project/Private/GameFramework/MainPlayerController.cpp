// Fill out your copyright notice in the Description page of Project Settings.


#include "GameFramework/MainPlayerController.h"
#include "Characters/PlayerCharacter.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputMappingContext.h"
#include "Blueprint/UserWidget.h"
#include "Net/UnrealNetwork.h"
#include "PlayerStats/PlayerStatsComponent.h"
#include "GameFramework/PlayerState.h"
#include "UI/HUD/MainHUDWidget.h"


AMainPlayerController::AMainPlayerController()
{
	bCanSprint = true;
}

void AMainPlayerController::BeginPlay()
{
	Super::BeginPlay();

	TObjectPtr<UEnhancedInputLocalPlayerSubsystem> InputLocalPlayerSubsystem
		= ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer());

	if (InputLocalPlayerSubsystem && IsLocalController())
	{
		InputLocalPlayerSubsystem->AddMappingContext(CurrentMappingContext.Get(), 0);
	}

	UPlayerStatsComponent* PlayerStatsComponent = PlayerState
		                                              ? PlayerState->GetComponentByClass<UPlayerStatsComponent>()
		                                              : nullptr;
	if (PlayerStatsComponent)
	{
		PlayerStatsComponent->OnStaminaEnd.AddUObject(this, &AMainPlayerController::OnStaminaEnd);
	}

	if (IsLocalController())
	{
		TryToCreateMainHUDWidget();
	}

	UpdateActionToKeysCache();
}

void AMainPlayerController::UpdateActionToKeysCache()
{
	for (const FEnhancedActionKeyMapping& Mapping : CurrentMappingContext->GetMappings())
	{
		if (Mapping.Action)
		{
			CacheActionToKeys.FindOrAdd(Mapping.Action).Add(Mapping.Key);
		}
	}
	UpdateInteractTypeToKeyCache();
}

void AMainPlayerController::UpdateInteractTypeToKeyCache()
{
	for (const auto& Pair : InteractActions)
	{
		EInteractType InteractType = Pair.Key;
		UInputAction* Action = Pair.Value;

		if (const TArray<FKey>* Keys = CacheActionToKeys.Find(Action))
		{
			if (Keys->Num() > 0)
			{
				CacheInteractTypeToKey.Add(InteractType, (*Keys)[0]);
			}
		}
	}
}

void AMainPlayerController::TryToCreateMainHUDWidget()
{
	if (MainHUDWidgetClass)
	{
		if (!GetPlayerState<AMainPlayerState>())
		{
			FTimerHandle TimerHandle;
			GetWorldTimerManager().SetTimer(TimerHandle, this, &AMainPlayerController::TryToCreateMainHUDWidget, 0.1f,
			                                false);
			return;
		}
		auto Widget = CreateWidget(this, MainHUDWidgetClass, TEXT("MainHUDWidget"));
		if (Widget)
		{
			Widget->AddToViewport();
			UMainHUDWidget* MainHUDWidget = Cast<UMainHUDWidget>(Widget);
			if (MainHUDWidget)
			{
				MainHUDWidget->MainPlayerState = GetPlayerState<AMainPlayerState>();
			}
		}
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("MainPC: MainHUDWidgetClass is nullptr"));
	}
}

void AMainPlayerController::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION(AMainPlayerController, bCanSprint, COND_OwnerOnly);
}

void AMainPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();
	UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(this->InputComponent);
	if (EnhancedInputComponent)
	{
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AMainPlayerController::Move);
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &AMainPlayerController::Look);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &AMainPlayerController::StartJump);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this,
		                                   &AMainPlayerController::StopJump);
		EnhancedInputComponent->BindAction(SprintAction, ETriggerEvent::Started, this,
		                                   &AMainPlayerController::StartSprint);
		EnhancedInputComponent->BindAction(SprintAction, ETriggerEvent::Completed, this,
		                                   &AMainPlayerController::StopSprint);

		EnhancedInputComponent->BindAction(InteractAction, ETriggerEvent::Started, this,
		                                   &AMainPlayerController::Interact);
		EnhancedInputComponent->BindAction(SecondInteractAction, ETriggerEvent::Started, this,
		                                   &AMainPlayerController::SecondInteract);
		EnhancedInputComponent->BindAction(UseItemAction, ETriggerEvent::Triggered, this,
		                                   &AMainPlayerController::UseItem);

		EnhancedInputComponent->BindAction(ToggleInventoryAction, ETriggerEvent::Triggered, this,
		                                   &AMainPlayerController::ToggleInventory);
		EnhancedInputComponent->BindAction(ToggleCraftMenuAction, ETriggerEvent::Triggered, this,
		                                   &AMainPlayerController::ToggleCraftMenu);
	}
}

void AMainPlayerController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);
}

void AMainPlayerController::OnStaminaEnd(bool StaminaEnd)
{
	if (!HasAuthority()) return;
	bCanSprint = !StaminaEnd;
	if (StaminaEnd)
	{
		StopSprint();
	}
}

void AMainPlayerController::Move(const FInputActionValue& Value)
{
	APlayerCharacter* LCharacter = Cast<APlayerCharacter>(GetPawn());
	if (!LCharacter) return;

	const FVector2D MovementVector = Value.Get<FVector2D>();

	const FRotator Rotation = GetControlRotation();
	const FRotator YawRotation(0, Rotation.Yaw, 0);

	const FVector ForwardDuration = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	const FVector RightDuration = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

	LCharacter->AddMovementInput(ForwardDuration, MovementVector.Y);
	LCharacter->AddMovementInput(RightDuration, MovementVector.X);
}

void AMainPlayerController::Look(const FInputActionValue& Value)
{
	APlayerCharacter* LCharacter = Cast<APlayerCharacter>(GetPawn());
	if (!LCharacter) return;

	FVector2D LookValue = Value.Get<FVector2D>();

	LCharacter->AddControllerYawInput(LookValue.X);
	LCharacter->AddControllerPitchInput(LookValue.Y * -1.0f);
}

void AMainPlayerController::StartJump()
{
	APlayerCharacter* LCharacter = Cast<APlayerCharacter>(GetPawn());
	if (!LCharacter) return;
	LCharacter->Jump();
}

void AMainPlayerController::StopJump()
{
	APlayerCharacter* LCharacter = Cast<APlayerCharacter>(GetPawn());
	if (!LCharacter) return;
	LCharacter->StopJumping();
}

void AMainPlayerController::ServerStartSprint_Implementation()
{
	APlayerCharacter* LCharacter = Cast<APlayerCharacter>(GetPawn());
	if (!LCharacter) return;
	if (!bCanSprint) return;
	LCharacter->StartSprint();
	OnSprintStateChanged.Broadcast(true);
}

bool AMainPlayerController::ServerStartSprint_Validate()
{
	return bCanSprint;
}

void AMainPlayerController::ServerStopSprint_Implementation()
{
	APlayerCharacter* LCharacter = Cast<APlayerCharacter>(GetPawn());
	if (!LCharacter) return;
	LCharacter->StopSprint();
	OnSprintStateChanged.Broadcast(false);
}

bool AMainPlayerController::ServerStopSprint_Validate()
{
	return true;
}

void AMainPlayerController::StartSprint()
{
	if (!IsLocalController()) return;
	ServerStartSprint();
}

void AMainPlayerController::StopSprint()
{
	if (!IsLocalController()) return;
	ServerStopSprint();
}

void AMainPlayerController::Interact()
{
	if (!IsLocalController()) return;
	APlayerCharacter* LCharacter = Cast<APlayerCharacter>(GetPawn());
	if (!LCharacter) return;
	LCharacter->TryInteract();
}

void AMainPlayerController::SecondInteract()
{
}

void AMainPlayerController::UseItem()
{
}

void AMainPlayerController::ToggleInventory()
{
	OnToggleInventory.Broadcast();
}

void AMainPlayerController::ToggleCraftMenu()
{
	OnToggleCraftMenu.Broadcast();
}
