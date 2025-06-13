// Fill out your copyright notice in the Description page of Project Settings.


#include "SurvivalCharacterComponent.h"
#include "EnhancedInputSubsystems.h"
#include "SurvivalGameplayTags.h"
#include "SurvivalLocalPlayer.h"
#include "SurvivalPawnData.h"
#include "InputMappingContext.h"
#include "LogMacros.h"
#include "SkeletalMeshAttributes.h"
#include "SurvivalCharacter.h"
#include "SurvivalPawnExtensionComponent.h"
#include "AbilitySystem/SurvivalAbilitySystemComponent.h"
#include "Components/GameFrameworkComponentDelegates.h"
#include "Components/GameFrameworkComponentManager.h"
#include "GameFramework/SurvivalGameInstance.h"
#include "GameFramework/SurvivalPlayerController.h"
#include "GameFramework/SurvivalPlayerState.h"
#include "Input/IMCDataAsset.h"
#include "Input/SurvivalInputComponent.h"
#include "Input/SurvivalInputMappingSubsystem.h"


#if WITH_EDITOR
#include "Misc/UObjectToken.h"
#endif

namespace SurvivalCharacter
{
	static const float LookYawRate = 300.0f;
	static const float LookPitchRate = 165.0f;
}

const FName USurvivalCharacterComponent::NAME_BindInputsNow("BindInputsNow");
const FName USurvivalCharacterComponent::NAME_ActorFeatureName("SurvivalCharacter");

// Sets default values for this component's properties
USurvivalCharacterComponent::USurvivalCharacterComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	PrimaryComponentTick.bCanEverTick = true;
	bReadyToBindInputs = false;
}

TArray<UIMCAndPriority*> USurvivalCharacterComponent::GetInputMappings()
{
	TArray<UIMCAndPriority*> IMCs;
	
	if (InputMappings.IsEmpty())
	{
		LOGF_FUNC(Error, "No IMCs")
		return IMCs;
	}
	
	for (const FName& Name : InputMappings)
	{
		if (UGameInstance* GI = GetWorld()->GetGameInstance<UGameInstance>())
		{
			const USurvivalInputMappingSubsystem* Subsystem = GI->GetSubsystem<USurvivalInputMappingSubsystem>();
			if (Subsystem)
			{
				IMCs.Add(Subsystem->GetContextByName(Name));
			}
			else
			{
				LOGF_FUNC(Warning, "Subsystem is 0")
			}
		}
		else
		{
			LOGF_FUNC(Warning, "Game instance is 0")
		}
	}
	return IMCs;
}

void USurvivalCharacterComponent::AddAdditionalInputConfig(const USurvivalInputConfig* InputConfig)
{
	TArray<uint32> BindHandles;

	const APawn* Pawn = GetPawn<APawn>();
	if (!Pawn) return;

	const APlayerController* PC = GetController<APlayerController>();
	check(PC);

	const ULocalPlayer* LP = PC->GetLocalPlayer();
	check(LP);

	UEnhancedInputLocalPlayerSubsystem* Subsystem = LP->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>();
	check(Subsystem);

	if (const USurvivalPawnExtensionComponent* PawnExtComp =
		USurvivalPawnExtensionComponent::FindPawnExtensionComponent(Pawn))
	{
		USurvivalInputComponent* SurvivalIC = Pawn->FindComponentByClass<USurvivalInputComponent>();
		if (ensureMsgf(
			SurvivalIC,
			TEXT(
				"Unexpected Input Component class! The Gameplay Abilities will not be bound to their inputs. Change the input component to USurvivalInputComponent or a subclass of it."
			)))
		{
			SurvivalIC->BindAbilityActions(InputConfig, this, &ThisClass::Input_AbilityInputTagPressed,
			                               &ThisClass::Input_AbilityInputTagReleased, /*out*/ BindHandles);
		}
	}
}

void USurvivalCharacterComponent::RemoveAdditionalInputConfig(const USurvivalInputConfig* InputConfig)
{
	//@TODO: Implement me!
}

bool USurvivalCharacterComponent::IsReadyToBindInputs() const
{
	return bReadyToBindInputs;
}

bool USurvivalCharacterComponent::CanChangeInitState(UGameFrameworkComponentManager* Manager, FGameplayTag CurrentState,
                                                     FGameplayTag DesiredState) const
{
	check(Manager);

	APawn* Pawn = GetPawn<APawn>();

	if (!CurrentState.IsValid() && DesiredState == SurvivalGameplayTags::InitState_Spawned)
	{
		// As long as we have a real pawn, let us transition
		if (Pawn)
		{
			return true;
		}
	}
	else if (CurrentState == SurvivalGameplayTags::InitState_Spawned && DesiredState ==
		SurvivalGameplayTags::InitState_DataAvailable)
	{
		// The player state is required.
		if (!GetPlayerState<ASurvivalPlayerState>())
		{
			LOGF_FUNC(Error, "Failed on: !GetPlayerState<ASurvivalPlayerState>()")
			return false;
		}

		// If we're authority or autonomous, we need to wait for a controller with registered ownership of the player state.
		if (Pawn->GetLocalRole() != ROLE_SimulatedProxy)
		{
			AController* Controller = GetController<AController>();

			const bool bHasControllerPairedWithPS = (Controller != nullptr) &&
				(Controller->PlayerState != nullptr) &&
				(Controller->PlayerState->GetOwner() == Controller);

			if (!bHasControllerPairedWithPS)
			{
				LOGF_FUNC(Error, "Failed on: !bHasControllerPairedWithPS")
				return false;
			}
		}

		const bool bIsLocallyControlled = Pawn->IsLocallyControlled();
		const bool bIsBot = Pawn->IsBotControlled();

		if (bIsLocallyControlled && !bIsBot)
		{
			ASurvivalPlayerController* SurvivalPC = GetController<ASurvivalPlayerController>();

			// The input component and local player is required when locally controlled.
			if (!Pawn->InputComponent || !SurvivalPC || !SurvivalPC->GetLocalPlayer())
			{
				LOGF_FUNC(Error, "Failed on: !Pawn->InputComponent || !SurvivalPC || !SurvivalPC->GetLocalPlayer()")
				return false;
			}
		}

		return true;
	}
	else if (CurrentState == SurvivalGameplayTags::InitState_DataAvailable && DesiredState ==
		SurvivalGameplayTags::InitState_DataInitialized)
	{
		// Wait for player state and extension component
		ASurvivalPlayerState* SurvivalPS = GetPlayerState<ASurvivalPlayerState>();

		if (SurvivalPS)
		{
			return true;
		}
		LOGF_FUNC(Error, "Failed on: SurvivalPS")
		//*&& Manager->HasFeatureReachedInitState(
		//Pawn, USurvivalPawnExtensionComponent::NAME_ActorFeatureName,
		//SurvivalGameplayTags::InitState_DataInitialized *//);
	}
	else if (CurrentState == SurvivalGameplayTags::InitState_DataInitialized && DesiredState ==
		SurvivalGameplayTags::InitState_GameplayReady)
	{
		// TODO add ability initialization checks?
		return true;
	}

	return false;
}

void USurvivalCharacterComponent::HandleChangeInitState(UGameFrameworkComponentManager* Manager,
                                                        FGameplayTag CurrentState, FGameplayTag DesiredState)
{
	UE_LOG(LogTemp, Warning, TEXT("[SCC] HandleChangeInitState: Current=%s (%d), Desired=%s (%d)"),
	       *CurrentState.ToString(), CurrentState.IsValid(),
	       *DesiredState.ToString(), DesiredState.IsValid());
	if (CurrentState == SurvivalGameplayTags::InitState_DataAvailable && DesiredState ==
		SurvivalGameplayTags::InitState_DataInitialized)
	{
		APawn* Pawn = GetPawn<APawn>();
		ASurvivalPlayerState* SurvivalPS = GetPlayerState<ASurvivalPlayerState>();
		if (!ensure(Pawn && SurvivalPS))
		{
			return;
		}

		const USurvivalPawnData* PawnData = nullptr;

		if (USurvivalPawnExtensionComponent* PawnExtComp =
			USurvivalPawnExtensionComponent::FindPawnExtensionComponent(Pawn))
		{
			PawnData = PawnExtComp->GetPawnData<USurvivalPawnData>();

			PawnExtComp->InitializeAbilitySystem(SurvivalPS->GetSurvivalAbilitySystemComponent(), SurvivalPS);
		}

		if (ASurvivalPlayerController* SurvivalPC = GetController<ASurvivalPlayerController>())
		{
			if (Pawn->InputComponent != nullptr)
			{
				InitializePlayerInput(Pawn->InputComponent);
			}
		}

		// Hook up the delegate for all pawns, in case we spectate later
		if (PawnData)
		{
		}
	}
}

void USurvivalCharacterComponent::OnActorInitStateChanged(const FActorInitStateChangedParams& Params)
{
	if (Params.FeatureName == USurvivalPawnExtensionComponent::NAME_ActorFeatureName)
	{
		if (Params.FeatureState == SurvivalGameplayTags::InitState_DataInitialized)
		{
			// If the extension component says all other components are initialized, try to progress to next state
			CheckDefaultInitialization();
		}
	}
}

void USurvivalCharacterComponent::CheckDefaultInitialization()
{
	static const TArray<FGameplayTag> StateChain = {
		SurvivalGameplayTags::InitState_Spawned, SurvivalGameplayTags::InitState_DataAvailable,
		SurvivalGameplayTags::InitState_DataInitialized, SurvivalGameplayTags::InitState_GameplayReady
	};

	// This will try to progress from spawned (which is only set in BeginPlay) through the data initialization stages until it gets to gameplay ready
	ContinueInitStateChain(StateChain);
}

void USurvivalCharacterComponent::OnRegister()
{
	Super::OnRegister();

	if (!GetPawn<APawn>())
	{
		LOGF_FUNC(
			Error,
			"This component has been added to a blueprint whose base class is not a Pawn. To use this component, it MUST be placed on a Pawn Blueprint.")

#if WITH_EDITOR
		if (GIsEditor)
		{
			static const FText Message = NSLOCTEXT("SurvivalCharacterComponent", "NotOnPawnError",
			                                       "has been added to a blueprint whose base class is not a Pawn. To use this component, it MUST be placed on a Pawn Blueprint. This will cause a crash if you PIE!");
			static const FName CharacterMessageLogName = TEXT("SurvivalCharacterComponent");

			FMessageLog(CharacterMessageLogName).Error()
			                                    ->AddToken(FUObjectToken::Create(
				                                    this, FText::FromString(GetNameSafe(this))))
			                                    ->AddToken(FTextToken::Create(Message));

			FMessageLog(CharacterMessageLogName).Open();
		}
#endif
	}
	else
	{
		// Register with the init state system early, this will only work if this is a game world
		RegisterInitStateFeature();
	}
}

void USurvivalCharacterComponent::BeginPlay()
{
	Super::BeginPlay();

	// Listen for when the pawn extension component changes init state
	BindOnActorInitStateChanged(USurvivalPawnExtensionComponent::NAME_ActorFeatureName, FGameplayTag(), false);

	// Notifies that we are done spawning, then try the rest of initialization
	ensure(TryToChangeInitState(SurvivalGameplayTags::InitState_Spawned));
	CheckDefaultInitialization();
}

void USurvivalCharacterComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	UnregisterInitStateFeature();

	Super::EndPlay(EndPlayReason);
}

void USurvivalCharacterComponent::TickComponent(float DeltaTime, enum ELevelTick TickType,
	FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// GEngine->AddOnScreenDebugMessage(0, 0, FColor::Green, TEXT("Current IMCs:"));
	//
	// if (GetInputMappings().IsEmpty())
	// {
	// 	LOGF_FUNC(Error, "NoInputs");
	// 	return;
	// }
	//
	// for (UIMCAndPriority* IMCAndP : GetInputMappings())
	// {
	// 	if (IMCAndP->InputMappingContext)
	// 	{
	// 		FString NameIMC = IMCAndP->InputMappingContext.GetName();
	// 		if (!NameIMC.IsEmpty())
	// 		{
	// 			GEngine->AddOnScreenDebugMessage(0, 0, FColor::Green, NameIMC);
	// 		}
	// 	}
	// }
}

void USurvivalCharacterComponent::InitializePlayerInput(UInputComponent* PlayerInputComponent)
{
	check(PlayerInputComponent);

	const APawn* Pawn = GetPawn<APawn>();
	if (!Pawn)
	{
		return;
	}

	APlayerController* PC = GetController<APlayerController>();
	check(PC);

	if (!PC->GetLocalPlayer())
	{
		return;
	}
	
	const USurvivalLocalPlayer* LP = Cast<USurvivalLocalPlayer>(PC->GetLocalPlayer());
	check(LP);

	UEnhancedInputLocalPlayerSubsystem* Subsystem = LP->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>();
	check(Subsystem);

	//Subsystem->ClearAllMappings();

	if (const USurvivalPawnExtensionComponent* PawnExtComp =
		USurvivalPawnExtensionComponent::FindPawnExtensionComponent(Pawn))
	{
		if (const USurvivalPawnData* PawnData = PawnExtComp->GetPawnData<USurvivalPawnData>())
		{
			TArray<const USurvivalInputConfig*> AllInputConfigs;
			if (PawnData->InputConfig) 
			{
				AllInputConfigs.Add(PawnData->InputConfig);
			}
			if (ASurvivalPlayerState* SurvivalPS = GetPlayerState<ASurvivalPlayerState>())
			{
				if (const USurvivalPawnData* PlayerStatePawnData = SurvivalPS->GetPawnData<USurvivalPawnData>())
				{
					if (PlayerStatePawnData->InputConfig)
					{
						AllInputConfigs.AddUnique(PlayerStatePawnData->InputConfig); 
					}
				}
			}
			
			for (const USurvivalInputConfig* InputConfig : AllInputConfigs)
			{
				USurvivalInputComponent* SurvivalIC = Cast<USurvivalInputComponent>(PlayerInputComponent);
				if (ensureMsgf(SurvivalIC, TEXT("Unexpected Input Component class! The Gameplay Abilities will "
					               "not be bound to their inputs. Change the input component to USurvivalInputComponent "
					               "or a subclass of it.")))
				{
					// Add the key mappings that may have been set by the player
					SurvivalIC->AddInputMappings(InputConfig, Subsystem);

					// This is where we actually bind and input action to a gameplay tag, which means that Gameplay Ability Blueprints will
					// be triggered directly by these input actions Triggered events. 
					TArray<uint32> BindHandles;
					SurvivalIC->BindAbilityActions(InputConfig, this, &ThisClass::Input_AbilityInputTagPressed,
					                               &ThisClass::Input_AbilityInputTagReleased, /*out*/ BindHandles);

					SurvivalIC->BindNativeAction(InputConfig, SurvivalGameplayTags::InputTag_Move,
					                             ETriggerEvent::Triggered, this, &ThisClass::Input_Move,
					                             /*bLogIfNotFound=*/ false);
					SurvivalIC->BindNativeAction(InputConfig, SurvivalGameplayTags::InputTag_Look_Mouse,
					                             ETriggerEvent::Triggered, this, &ThisClass::Input_LookMouse,
					                             /*bLogIfNotFound=*/ false);
					SurvivalIC->BindNativeAction(InputConfig, SurvivalGameplayTags::InputTag_Look_Stick,
					                             ETriggerEvent::Triggered, this, &ThisClass::Input_LookStick,
					                             /*bLogIfNotFound=*/ false);
				}
			}
		}
	}

	if (/*ensure*/(!bReadyToBindInputs))
	{
		bReadyToBindInputs = true;
	}

	UGameFrameworkComponentManager::SendGameFrameworkComponentExtensionEvent(
		PC, NAME_BindInputsNow);
	UGameFrameworkComponentManager::SendGameFrameworkComponentExtensionEvent(
		const_cast<APawn*>(Pawn), NAME_BindInputsNow);
}

void USurvivalCharacterComponent::Input_AbilityInputTagPressed(FGameplayTag InputTag)
{
	if (const APawn* Pawn = GetPawn<APawn>())
	{
		if (const USurvivalPawnExtensionComponent* PawnExtComp =
			USurvivalPawnExtensionComponent::FindPawnExtensionComponent(Pawn))
		{
			if (USurvivalAbilitySystemComponent* SurvivalASC = PawnExtComp->GetSurvivalAbilitySystemComponent())
			{
				SurvivalASC->AbilityInputTagPressed(InputTag);
			}
		}
	}
}

void USurvivalCharacterComponent::Input_AbilityInputTagReleased(FGameplayTag InputTag)
{
	const APawn* Pawn = GetPawn<APawn>();
	if (!Pawn)
	{
		return;
	}

	if (const USurvivalPawnExtensionComponent* PawnExtComp =
		USurvivalPawnExtensionComponent::FindPawnExtensionComponent(Pawn))
	{
		if (USurvivalAbilitySystemComponent* SurvivalASC = PawnExtComp->GetSurvivalAbilitySystemComponent())
		{
			SurvivalASC->AbilityInputTagReleased(InputTag);
		}
	}
}

void USurvivalCharacterComponent::Input_Move(const FInputActionValue& InputActionValue)
{
	APawn* Pawn = GetPawn<APawn>();
	AController* Controller = Pawn ? Pawn->GetController() : nullptr;

	if (Controller)
	{
		const FVector2D Value = InputActionValue.Get<FVector2D>();
		const FRotator MovementRotation(0.0f, Controller->GetControlRotation().Yaw, 0.0f);

		if (Value.X != 0.0f)
		{
			const FVector MovementDirection = MovementRotation.RotateVector(FVector::RightVector);
			Pawn->AddMovementInput(MovementDirection, Value.X);
		}

		if (Value.Y != 0.0f)
		{
			const FVector MovementDirection = MovementRotation.RotateVector(FVector::ForwardVector);
			Pawn->AddMovementInput(MovementDirection, Value.Y);
		}
	}
}

void USurvivalCharacterComponent::Input_LookMouse(const FInputActionValue& InputActionValue)
{
	APawn* Pawn = GetPawn<APawn>();

	if (!Pawn)
	{
		return;
	}

	const FVector2D Value = InputActionValue.Get<FVector2D>();

	if (Value.X != 0.0f)
	{
		Pawn->AddControllerYawInput(Value.X);
	}

	if (Value.Y != 0.0f)
	{
		Pawn->AddControllerPitchInput(Value.Y);
	}
}

void USurvivalCharacterComponent::Input_LookStick(const FInputActionValue& InputActionValue)
{
	APawn* Pawn = GetPawn<APawn>();

	if (!Pawn)
	{
		return;
	}

	const FVector2D Value = InputActionValue.Get<FVector2D>();

	const UWorld* World = GetWorld();
	check(World);

	if (Value.X != 0.0f)
	{
		Pawn->AddControllerYawInput(Value.X * SurvivalCharacter::LookYawRate * World->GetDeltaSeconds());
	}

	if (Value.Y != 0.0f)
	{
		Pawn->AddControllerPitchInput(Value.Y * SurvivalCharacter::LookPitchRate * World->GetDeltaSeconds());
	}
}