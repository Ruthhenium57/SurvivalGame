// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/SurvivalCharacter.h"

#include "SurvivalCharacterComponent.h"
#include "SurvivalCharacterMovementComponent.h"
#include "SurvivalGameplayTags.h"
#include "SurvivalPawnExtensionComponent.h"
#include "AbilitySystem/SurvivalAbilitySystemComponent.h"
#include "Blueprint/UserWidget.h"
#include "Camera/CameraComponent.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/CapsuleComponent.h"
#include "Components/SphereComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SurvivalPlayerController.h"
#include "GameFramework/SurvivalPlayerState.h"
#include "Interfaces/InteractableInterface.h"
#include "Inventory/InventoryComponent.h"
#include "UI/HUD/InteractInfoWidget.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(SurvivalCharacter)

ASurvivalCharacter::ASurvivalCharacter(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer.SetDefaultSubobjectClass<USurvivalCharacterMovementComponent>(
		ACharacter::CharacterMovementComponentName))
{
	FirstPersonCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera"));
	FirstPersonMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("FirstPersonMesh"));
	ThirdPersonMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("ThirdPersonMesh"));

	FirstPersonCamera->SetupAttachment(RootComponent);
	FirstPersonMesh->SetupAttachment(FirstPersonCamera);
	ThirdPersonMesh->SetupAttachment(GetMesh());
	FirstPersonMesh->SetIsReplicated(false);

	PawnExtComponent = CreateDefaultSubobject<USurvivalPawnExtensionComponent>(TEXT("PawnExtensionComponent"));
	PawnExtComponent->OnAbilitySystemInitialized_RegisterAndCall(
		FSimpleMulticastDelegate::FDelegate::CreateUObject(this, &ThisClass::OnAbilitySystemInitialized));
	PawnExtComponent->OnAbilitySystemUninitialized_Register(
		FSimpleMulticastDelegate::FDelegate::CreateUObject(this, &ThisClass::OnAbilitySystemUninitialized));

	CharacterComponent = CreateDefaultSubobject<USurvivalCharacterComponent>(TEXT("CharacterComponent"));
	
	InteractDistance = 300.0f;
	InteractDotProduct = 0.7f;
	HighlightedActorUpdateRate = 0.1f;

	InteractionArea = CreateDefaultSubobject<USphereComponent>(TEXT("InteractionArea"));
	InteractionArea->SetupAttachment(RootComponent);
	InteractionArea->SetSphereRadius(InteractDistance);
	InteractionArea->SetGenerateOverlapEvents(true);
}

ASurvivalPlayerController* ASurvivalCharacter::GetSurvivalPlayerController() const
{
	return CastChecked<ASurvivalPlayerController>(Controller, ECastCheckedType::NullAllowed);
}

ASurvivalPlayerState* ASurvivalCharacter::GetSurvivalPlayerState() const
{
	return CastChecked<ASurvivalPlayerState>(GetPlayerState(), ECastCheckedType::NullAllowed);
}

USurvivalAbilitySystemComponent* ASurvivalCharacter::GetSurvivalAbilitySystemComponent() const
{
	return Cast<USurvivalAbilitySystemComponent>(GetAbilitySystemComponent());
}

UAbilitySystemComponent* ASurvivalCharacter::GetAbilitySystemComponent() const
{
	if (PawnExtComponent == nullptr)
	{
		return nullptr;
	}

	return PawnExtComponent->GetSurvivalAbilitySystemComponent();
}

void ASurvivalCharacter::GetOwnedGameplayTags(FGameplayTagContainer& TagContainer) const
{
	if (const USurvivalAbilitySystemComponent* SurvivalASC = GetSurvivalAbilitySystemComponent())
	{
		SurvivalASC->GetOwnedGameplayTags(TagContainer);
	}
}

bool ASurvivalCharacter::HasMatchingGameplayTag(FGameplayTag TagToCheck) const
{
	if (const USurvivalAbilitySystemComponent* SurvivalASC = GetSurvivalAbilitySystemComponent())
	{
		return SurvivalASC->HasMatchingGameplayTag(TagToCheck);
	}

	return false;
}

bool ASurvivalCharacter::HasAllMatchingGameplayTags(const FGameplayTagContainer& TagContainer) const
{
	if (const USurvivalAbilitySystemComponent* SurvivalASC = GetSurvivalAbilitySystemComponent())
	{
		return SurvivalASC->HasAllMatchingGameplayTags(TagContainer);
	}

	return false;
}

bool ASurvivalCharacter::HasAnyMatchingGameplayTags(const FGameplayTagContainer& TagContainer) const
{
	if (const USurvivalAbilitySystemComponent* SurvivalASC = GetSurvivalAbilitySystemComponent())
	{
		return SurvivalASC->HasAnyMatchingGameplayTags(TagContainer);
	}

	return false;
}

void ASurvivalCharacter::ToggleCrouch()
{
	const USurvivalCharacterMovementComponent* MoveComp = CastChecked<USurvivalCharacterMovementComponent>(
		GetCharacterMovement());

	if (bIsCrouched || MoveComp->bWantsToCrouch)
	{
		UnCrouch();
	}
	else if (MoveComp->IsMovingOnGround())
	{
		Crouch();
	}
}

void ASurvivalCharacter::PreInitializeComponents()
{
	Super::PreInitializeComponents();
}

void ASurvivalCharacter::BeginPlay()
{
	Super::BeginPlay();
	FirstPersonMesh->SetOnlyOwnerSee(true);
	ThirdPersonMesh->SetOwnerNoSee(true);
	if (!IsLocallyControlled())
	{
		FirstPersonMesh->SetVisibility(false, true);
	}

	if (IsLocallyControlled())
	{
		if (InteractionWidgetClass)
		{
			InteractionWidget = CreateWidget<UInteractInfoWidget>(GetWorld(), InteractionWidgetClass);
			if (InteractionWidget)
			{
				InteractionWidget->AddToViewport();
				InteractionWidget->SetVisibility(ESlateVisibility::Hidden);
			}
		}
	}

	if (InteractionArea)
	{
		InteractionArea->OnComponentBeginOverlap.AddDynamic(this, &ASurvivalCharacter::OnOverlapBegin);
		InteractionArea->OnComponentEndOverlap.AddDynamic(this, &ASurvivalCharacter::OnOverlapEnd);
	}

	TimerUpdateHighlightedActor();
}

void ASurvivalCharacter::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
}

void ASurvivalCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
}

void ASurvivalCharacter::PreReplication(IRepChangedPropertyTracker& ChangedPropertyTracker)
{
	Super::PreReplication(ChangedPropertyTracker);
}

void ASurvivalCharacter::NotifyControllerChanged()
{
	Super::NotifyControllerChanged();
}

void ASurvivalCharacter::OnAbilitySystemInitialized()
{
	USurvivalAbilitySystemComponent* SurvivalASC = GetSurvivalAbilitySystemComponent();
	check(SurvivalASC);

	//HealthComponent->InitializeWithAbilitySystem(SurvivalASC);

	InitializeGameplayTags();
}

void ASurvivalCharacter::OnAbilitySystemUninitialized()
{
	//HealthComponent->UninitializeFromAbilitySystem();
}

void ASurvivalCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController); 

	PawnExtComponent->HandleControllerChanged();
}

void ASurvivalCharacter::UnPossessed()
{
	Super::UnPossessed();

	PawnExtComponent->HandleControllerChanged();
}

void ASurvivalCharacter::OnRep_Controller()
{
	Super::OnRep_Controller();

	PawnExtComponent->HandleControllerChanged();
}

void ASurvivalCharacter::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();

	PawnExtComponent->HandlePlayerStateReplicated();
}

void ASurvivalCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PawnExtComponent->SetupPlayerInputComponent();
}

void ASurvivalCharacter::InitializeGameplayTags()
{
	// Clear tags that may be lingering on the ability system from the previous pawn.
	if (USurvivalAbilitySystemComponent* SurvivalASC = GetSurvivalAbilitySystemComponent())
	{
		for (const TPair<uint8, FGameplayTag>& TagMapping : SurvivalGameplayTags::MovementModeTagMap)
		{
			if (TagMapping.Value.IsValid())
			{
				SurvivalASC->SetLooseGameplayTagCount(TagMapping.Value, 0);
			}
		}

		for (const TPair<uint8, FGameplayTag>& TagMapping : SurvivalGameplayTags::CustomMovementModeTagMap)
		{
			if (TagMapping.Value.IsValid())
			{
				SurvivalASC->SetLooseGameplayTagCount(TagMapping.Value, 0);
			}
		}

		USurvivalCharacterMovementComponent* SurvivalMoveComp = CastChecked<USurvivalCharacterMovementComponent>(GetCharacterMovement());
		SetMovementModeTag(SurvivalMoveComp->MovementMode, SurvivalMoveComp->CustomMovementMode, true);
	}
}

void ASurvivalCharacter::OnDeathStarted(AActor* OwningActor)
{
	DisableMovementAndCollision();
}

void ASurvivalCharacter::OnDeathFinished(AActor* OwningActor)
{
	//GetWorld()->GetTimerManager().SetTimerForNextTick(this, &ThisClass::DestroyDueToDeath);
}

void ASurvivalCharacter::DisableMovementAndCollision()
{
	if (Controller)
	{
		Controller->SetIgnoreMoveInput(true);
	}

	UCapsuleComponent* CapsuleComp = GetCapsuleComponent();
	check(CapsuleComp);
	CapsuleComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	CapsuleComp->SetCollisionResponseToAllChannels(ECR_Ignore);

	USurvivalCharacterMovementComponent* SurvivalMoveComp = CastChecked<USurvivalCharacterMovementComponent>(
		GetCharacterMovement());
	SurvivalMoveComp->StopMovementImmediately();
	SurvivalMoveComp->DisableMovement();
}

void ASurvivalCharacter::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
}

void ASurvivalCharacter::Reset()
{
	DisableMovementAndCollision();

	Super::Reset();
}

void ASurvivalCharacter::TimerUpdateHighlightedActor()
{
	FTimerHandle TimerHandle;
	GetWorldTimerManager().SetTimer(TimerHandle, this, &ASurvivalCharacter::TimerUpdateHighlightedActor,
	                                HighlightedActorUpdateRate, false);

	if (!IsLocallyControlled())
	{
		return;
	}

	AActor* TargetActor = CheckForInteractableTarget();
	if (!TargetActor) return;
	if (TargetActor == CurrentHighlightedActor) return;
	LastHighlightedActor = CurrentHighlightedActor;
	CurrentHighlightedActor = TargetActor;

	if (CurrentHighlightedActor)
	{
		FVector WorldLocation = CurrentHighlightedActor->GetActorLocation() + FVector(0.f, 0.f, 30.f);
		FVector2D ScreenLocation;
		ASurvivalPlayerController* PlayerController = GetSurvivalPlayerController();

		if (PlayerController && PlayerController->ProjectWorldLocationToScreen(WorldLocation, ScreenLocation, true))
		{
			InteractionWidget->SetVisibility(ESlateVisibility::Visible);
			if (UCanvasPanelSlot* CanvasSlot = Cast<UCanvasPanelSlot>(InteractionWidget->Slot))
			{
				FVector2D OffsetFromObject = FVector2D(0, -50);
				CanvasSlot->SetPosition(ScreenLocation + OffsetFromObject);
				// CanvasSlot->SetAlignment(FVector2D(0.5f, 0.5f));
			}
			if (IInteractableInterface* Interface = Cast<IInteractableInterface>(TargetActor))
			{
				TMap<EInteractType, TArray<FKey>> Bindings;
				for (auto Pair : Interface->GetInteractTypes())
				{
					//Bindings.Add(Pair, PlayerController->GetKeysByInteractType(Pair));
				}
				InteractionWidget->UpdateInteractContainer(Bindings,
				                                           Interface->GetObjectName());
			}
		}
		else
		{
			InteractionWidget->SetVisibility(ESlateVisibility::Hidden);
		}
	}
	else
	{
		InteractionWidget->SetVisibility(ESlateVisibility::Hidden);
	}
}

void ASurvivalCharacter::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
                                        UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
                                        bool bFromSweep, const FHitResult& SweepResult)
{
	if (Cast<IInteractableInterface>(OtherActor))
	{
		NearbyInteractiveActors.Add(OtherActor);
	}
}

void ASurvivalCharacter::OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
                                      UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (Cast<IInteractableInterface>(OtherActor))
	{
		NearbyInteractiveActors.Remove(OtherActor);
	}
}

void ASurvivalCharacter::OnMovementModeChanged(EMovementMode PrevMovementMode, uint8 PreviousCustomMode)
{
	Super::OnMovementModeChanged(PrevMovementMode, PreviousCustomMode);

	const USurvivalCharacterMovementComponent* MoveComp = CastChecked<USurvivalCharacterMovementComponent>(
		GetCharacterMovement());

	SetMovementModeTag(PrevMovementMode, PreviousCustomMode, false);
	SetMovementModeTag(MoveComp->MovementMode, MoveComp->CustomMovementMode, true);
}

void ASurvivalCharacter::SetMovementModeTag(EMovementMode MovementMode, uint8 CustomMovementMode, bool bTagEnabled)
{
	if (USurvivalAbilitySystemComponent* SurvivalASC = GetSurvivalAbilitySystemComponent())
	{
		const FGameplayTag* MovementModeTag = nullptr;
		if (MovementMode == MOVE_Custom)
		{
			MovementModeTag = SurvivalGameplayTags::CustomMovementModeTagMap.Find(CustomMovementMode);
		}
		else
		{
			MovementModeTag = SurvivalGameplayTags::MovementModeTagMap.Find(MovementMode);
		}

		if (MovementModeTag && MovementModeTag->IsValid())
		{
			SurvivalASC->SetLooseGameplayTagCount(*MovementModeTag, (bTagEnabled ? 1 : 0));
		}
	}
}

void ASurvivalCharacter::OnStartCrouch(float HalfHeightAdjust, float ScaledHalfHeightAdjust)
{
	if (USurvivalAbilitySystemComponent* SurvivalASC = GetSurvivalAbilitySystemComponent())
	{
		SurvivalASC->SetLooseGameplayTagCount(SurvivalGameplayTags::Status_Crouching, 1);
	}

	Super::OnStartCrouch(HalfHeightAdjust, ScaledHalfHeightAdjust);
}

void ASurvivalCharacter::OnEndCrouch(float HalfHeightAdjust, float ScaledHalfHeightAdjust)
{
	if (USurvivalAbilitySystemComponent* SurvivalASC = GetSurvivalAbilitySystemComponent())
	{
		SurvivalASC->SetLooseGameplayTagCount(SurvivalGameplayTags::Status_Crouching, 0);
	}

	Super::OnEndCrouch(HalfHeightAdjust, ScaledHalfHeightAdjust);
}

bool ASurvivalCharacter::CanJumpInternal_Implementation() const
{
	return Super::CanJumpInternal_Implementation();
}

AActor* ASurvivalCharacter::CheckForInteractableTarget()
{
	static const auto CVar = IConsoleManager::Get().FindConsoleVariable(TEXT("SurvivalProject.DebugInteraction"));
	const bool bDebugEnabled = CVar ? CVar->GetBool() : false;

	UWorld* World = GetWorld();
	if (!GetController() || !World) return nullptr;

	if (!GetController()) return nullptr;
	FVector PlayerViewLocation;
	FRotator PlayerViewRotation;
	GetController()->GetPlayerViewPoint(PlayerViewLocation, PlayerViewRotation);
	FVector PlayerViewDirection = PlayerViewRotation.Vector();

	if (bDebugEnabled)
	{
		DrawDebugLine(
			World,
			PlayerViewLocation,
			PlayerViewLocation + PlayerViewDirection * 1000.0f,
			FColor::White,
			false,
			0.0f,
			0,
			1.0f
		);
	}

	AActor* BestTargetForInteract = nullptr;
	float BestDotProduct = 0;

	AActor* PotentialBestTarget = nullptr;
	float PotentialBestDot = 0.0f;

	for (AActor* TargetActor : NearbyInteractiveActors)
	{
		if (!TargetActor) continue;

		FVector TargetLocation = TargetActor->GetActorLocation();
		FVector DirectionToTarget = (TargetActor->GetActorLocation() - PlayerViewLocation).GetSafeNormal();
		float DotProduct = FVector::DotProduct(PlayerViewDirection, DirectionToTarget);

		if (bDebugEnabled)
		{
			DrawDebugLine(
				World,
				PlayerViewLocation,
				TargetLocation,
				FColor::Yellow,
				false, 0.0f, 0, 1.0f
			);
			DrawDebugSphere(
				World,
				TargetLocation,
				25.0f,
				12,
				FColor::Yellow,
				false, 0.0f, 0, 1.0f
			);
			DrawDebugString(
				World,
				TargetLocation + FVector(0, 0, 50.0f),
				FString::Printf(TEXT("%.2f"), DotProduct),
				nullptr,
				FColor::White,
				0.0f,
				true
			);
		}

		if (DotProduct > InteractDotProduct)
		{
			if (bDebugEnabled)
			{
				DrawDebugSphere(
					World,
					TargetLocation,
					30.0f,
					12,
					FColor::Orange,
					false, 0.0f, 0, 1.5f
				);
			}

			if (DotProduct > BestDotProduct)
			{
				if (DotProduct > PotentialBestDot)
				{
					PotentialBestDot = DotProduct;
					PotentialBestTarget = TargetActor;
				}
			}
		}
	}

	if (PotentialBestTarget)
	{
		FCollisionQueryParams QueryParams;
		QueryParams.AddIgnoredActor(this);
		QueryParams.bTraceComplex = false;
		FHitResult OutHit;

		if (bDebugEnabled)
		{
			DrawDebugLine(
				World,
				PlayerViewLocation,
				PotentialBestTarget->GetActorLocation(),
				FColor::Red,
				false, 1.0f,
				0, 2.0f
			);
		}

		bool bHit = World->LineTraceSingleByChannel(
			OutHit,
			PlayerViewLocation,
			PotentialBestTarget->GetActorLocation(),
			ECC_Visibility,
			QueryParams
		);

		if (bDebugEnabled && bHit)
		{
			DrawDebugSphere(
				World,
				OutHit.ImpactPoint,
				15.0f,
				12,
				FColor::Red,
				false, 1.0f, 0, 2.0f
			);
		}

		if (bHit && OutHit.GetActor())
		{
			if (OutHit.GetActor() == PotentialBestTarget)
			{
				BestTargetForInteract = PotentialBestTarget;
				//BestDotProduct = PotentialBestDot; 

				if (bDebugEnabled)
				{
					DrawDebugSphere(
						World,
						BestTargetForInteract->GetActorLocation(),
						35.0f,
						12,
						FColor::Green,
						false, 0.0f, 0, 3.0f
					);
					DrawDebugLine(
						World,
						PlayerViewLocation,
						BestTargetForInteract->GetActorLocation(),
						FColor::Green,
						false, 0.0f, 0, 3.0f
					);
				}
			}
		}
	}
	return BestTargetForInteract;
}

void ASurvivalCharacter::TryInteract()
{
	if (AActor* TargetActor = CheckForInteractableTarget())
	{
		ClientPredictInteract(TargetActor);
		ServerInteraction(TargetActor);
	}
}

void ASurvivalCharacter::ClientPredictInteract(AActor* HitActor)
{
	if (HitActor && !HasAuthority())
	{
		if (AMainItemActor* Item = Cast<AMainItemActor>(HitActor))
		{
			//GetMainPlayerState()->GetInventoryComponent()->ClientPredictAddItem(Item->ItemID, 1);
		}
	}
}

void ASurvivalCharacter::ServerInteraction_Implementation(AActor* TargetActor)
{
	AActor* ServerActor = CheckForInteractableTarget();
	if (TargetActor != ServerActor)
	{
	}
	if (IInteractableInterface* Interface = Cast<IInteractableInterface>(ServerActor))
	{
		//Interface->Interact(GetMainPlayerState()->GetInventoryComponent());
	}
}

bool ASurvivalCharacter::ServerInteraction_Validate(AActor* TargetActor)
{
	return TargetActor != nullptr;
}
