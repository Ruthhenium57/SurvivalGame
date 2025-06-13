// Fill out your copyright notice in the Description page of Project Settings.


#include "SurvivalPlayerController.h"

#include "AbilitySystemGlobals.h"
#include "EnhancedInputSubsystems.h"
#include "InputMappingContext.h"
#include "SurvivalPlayerState.h"
#include "AbilitySystem/SurvivalAbilitySystemComponent.h"
#include "Characters/SurvivalCharacterComponent.h"
#include "Characters/SurvivalPawnExtensionComponent.h"
#include "Input/IMCDataAsset.h"
#include "UserSettings/EnhancedInputUserSettings.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(SurvivalPlayerController)

ASurvivalPlayerController::ASurvivalPlayerController()
{
}

ASurvivalPlayerState* ASurvivalPlayerController::GetSurvivalPlayerState() const
{
	return CastChecked<ASurvivalPlayerState>(PlayerState, ECastCheckedType::NullAllowed);
}

USurvivalAbilitySystemComponent* ASurvivalPlayerController::GetSurvivalAbilitySystemComponent() const
{
	const ASurvivalPlayerState* SurvivalPS = GetSurvivalPlayerState();
	return SurvivalPS ? SurvivalPS->GetSurvivalAbilitySystemComponent() : nullptr;
}

USurvivalPawnExtensionComponent* ASurvivalPlayerController::GetPawnExtensionComponent() const
{
	if (!GetPawn()) return nullptr;
	return GetPawn()->GetComponentByClass<USurvivalPawnExtensionComponent>();
}

void ASurvivalPlayerController::AddIMC(const UIMCAndPriority* Mapping) const
{
	UEnhancedInputLocalPlayerSubsystem* Subsystem = GetLocalPlayer()->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>();
	check(Subsystem);
	
	if (const UInputMappingContext* IMC = Mapping->InputMappingContext)
	{
		Subsystem->AddMappingContext(IMC, Mapping->Priority);
	}
}

void ASurvivalPlayerController::RemoveIMC(const UIMCAndPriority* Mapping) const
{
	UEnhancedInputLocalPlayerSubsystem* Subsystem = GetLocalPlayer()->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>();
	check(Subsystem);
	
	if (const UInputMappingContext* IMC = Mapping->InputMappingContext)
	{
		Subsystem->RemoveMappingContext(IMC);
	}
}

void ASurvivalPlayerController::BeginPlay()
{
	Super::BeginPlay();
}

void ASurvivalPlayerController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	if (!InPawn)
	{
		return;
	}

	USurvivalCharacterComponent* CharacterComponent = USurvivalCharacterComponent::FindCharacterComponent(InPawn);
    if (CharacterComponent)
    {
        for (UIMCAndPriority* IMCSetup : CharacterComponent->GetInputMappings())
        {
            if (IMCSetup && IMCSetup->InputMappingContext)
            {
                AddIMC(IMCSetup); 
            }
        }
    	
        if (InPawn->InputComponent)
        {
            CharacterComponent->InitializePlayerInput(InPawn->InputComponent);
        }
        else
        {
             UE_LOG(LogTemp, Warning, TEXT("ASurvivalPlayerController::OnPossess: Pawn %s does not have an InputComponent yet. Input initialization might be deferred."), *InPawn->GetName());
        }
    }
}

void ASurvivalPlayerController::OnUnPossess()
{
	// Make sure the pawn that is being unpossessed doesn't remain our ASC's avatar actor
	APawn* UnpossessedPawn = GetPawn(); 

    if (UnpossessedPawn)
    {
        if (UAbilitySystemComponent* ASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(PlayerState))
        {
            if (ASC->GetAvatarActor() == UnpossessedPawn)
            {
                ASC->SetAvatarActor(nullptr);
            }
        }
    }

    if (UnpossessedPawn)
    {
        USurvivalCharacterComponent* CharacterComponent = UnpossessedPawn->FindComponentByClass<USurvivalCharacterComponent>();
        if (CharacterComponent)
        {
            for (UIMCAndPriority* IMCSetup : CharacterComponent->GetInputMappings())
            {
                 if (IMCSetup && IMCSetup->InputMappingContext)
                 {
                    RemoveIMC(IMCSetup);
                 }
            }
        	
            // if (const USurvivalPawnExtensionComponent* PawnExtComp = USurvivalPawnExtensionComponent::FindPawnExtensionComponent(UnpossessedPawn))
            // {
            //     UEnhancedInputLocalPlayerSubsystem* Subsystem = GetLocalPlayer() ? GetLocalPlayer()->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>() : nullptr;
            //     if (Subsystem)
            //     {
            //         // Удаляем IMC из PawnData самого персонажа
            //         if (const USurvivalPawnData* PawnData = PawnExtComp->GetPawnData<USurvivalPawnData>())
            //         {
            //             if (PawnData->InputConfig)
            //             {
            //                 // Предположим, что USurvivalInputConfig - это UIMCDataAsset или содержит поле InputMappingContext
            //                 // Адаптируйте эту часть, если структура USurvivalInputConfig другая.
            //                 // Например, если USurvivalInputConfig сам является UDataAsset, содержащим IMC:
            //                 const UIMCDataAsset* ConfigAsIMCDataAsset = Cast<const UIMCDataAsset>(PawnData->InputConfig);
            //                 if (ConfigAsIMCDataAsset && ConfigAsIMCDataAsset->InputMappingContext)
            //                 {
            //                     Subsystem->RemoveMappingContext(ConfigAsIMCDataAsset->InputMappingContext);
            //                 }
            //                 // Если USurvivalInputConfig имеет поле типа TObjectPtr<UInputMappingContext> MyIMC;
            //                 // else if (PawnData->InputConfig->MyIMC) { Subsystem->RemoveMappingContext(PawnData->InputConfig->MyIMC); }
            //             }
            //         }
            //         // Удаляем IMC из PawnData в PlayerState (если он есть и отличается)
            //         if (ASurvivalPlayerState* SurvivalPS = GetSurvivalPlayerState())
            //         {
            //              if (const USurvivalPawnData* PlayerStatePawnData = SurvivalPS->GetPawnData<USurvivalPawnData>())
            //              {
            //                 if (PlayerStatePawnData->InputConfig)
            //                 {
            //                     const UIMCDataAsset* ConfigAsIMCDataAsset = Cast<const UIMCDataAsset>(PlayerStatePawnData->InputConfig);
            //                     if (ConfigAsIMCDataAsset && ConfigAsIMCDataAsset->InputMappingContext)
            //                     {
            //                         Subsystem->RemoveMappingContext(ConfigAsIMCDataAsset->InputMappingContext);
            //                     }
            //                     // Аналогично, адаптируйте, если структура другая
            //                 }
            //              }
            //         }
            //     }
            // }
        }
    }
	
	Super::OnUnPossess();
}

void ASurvivalPlayerController::SetPlayer(UPlayer* InPlayer)
{
	Super::SetPlayer(InPlayer);

	// if (const ULyraLocalPlayer* LyraLocalPlayer = Cast<ULyraLocalPlayer>(InPlayer))
	// {
	// 	ULyraSettingsShared* UserSettings = LyraLocalPlayer->GetSharedSettings();
	// 	UserSettings->OnSettingChanged.AddUObject(this, &ThisClass::OnSettingsChanged);
	//
	// 	OnSettingsChanged(UserSettings);
	// }
}

void ASurvivalPlayerController::PostProcessInput(const float DeltaTime, const bool bGamePaused)
{
	Super::PostProcessInput(DeltaTime, bGamePaused);

	if (USurvivalAbilitySystemComponent* SurvivalASC = GetSurvivalAbilitySystemComponent())
	{
		SurvivalASC->ProcessAbilityInput(DeltaTime, bGamePaused);
	}
}

void ASurvivalPlayerController::OnPlayerStateChanged()
{
}

void ASurvivalPlayerController::BroadcastOnPlayerStateChanged()
{
	OnPlayerStateChanged();

	LastSeenPlayerState = PlayerState;
}


// void ASurvivalPlayerController::UpdateRotation(float DeltaTime)
// {
// 	Super::UpdateRotation(DeltaTime);
// 	FRotator NewRotation = GetControlRotation();
// 	NewRotation.Pitch = FMath::ClampAngle(NewRotation.Pitch, -80.0f, 80.0f);
// 	SetControlRotation(NewRotation);
// }
