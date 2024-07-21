// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayableCharacter.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputMappingContext.h"
#include "InputAction.h"
#include "MainHUDWidget.h"
#include "HealthBarWidget.h"
#include "StaminaBarWidget.h"
#include "GameHUD.h"
#include "GameFramework/PlayerController.h"
#include "Components/InputComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "TimerManager.h"

APlayableCharacter::APlayableCharacter()
{
	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArmComponent"));
	SpringArm->SetupAttachment(RootComponent);
	SpringArm->TargetArmLength = 500.0f;


	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("ThirstPersonCamera"));
	Camera->SetupAttachment(SpringArm);

	SpringArm->bUsePawnControlRotation = true;
	Camera->bUsePawnControlRotation = false;

	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	PlayerStatsComp = CreateDefaultSubobject<UPlayerStatsComp>(TEXT("PlayerStatsComp"));

	DefaultWalkSpeed = 600.0f;
	SprintSpeed = 1200.0f;
	bIsSprinting = false;
	StaminaRegenRate = 5.0f;
	StaminaDecreaseRate = 10.0f;
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
}

void APlayableCharacter::MoveForward(float Value)
{
	FVector Direction = FRotationMatrix(Controller->GetControlRotation()).GetScaledAxis(EAxis::X);
	AddMovementInput(Direction, Value);
}

void APlayableCharacter::MoveRight(float Value)
{
	FVector Direction = FRotationMatrix(Controller->GetControlRotation()).GetScaledAxis(EAxis::Y);
	AddMovementInput(Direction, Value);
}

void APlayableCharacter::Look(float Value)
{
	AddControllerPitchInput(Value);
}

void APlayableCharacter::Turn(float Value)
{
	AddControllerYawInput(Value);
}

void APlayableCharacter::Jump()
{
	bPressedJump = true;
}

void APlayableCharacter::StopJump()
{
	bPressedJump = false;
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

void APlayableCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	//DOREPLIFETIME(AMyCharacter, Health); // Добавляем свойство Health для репликации
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