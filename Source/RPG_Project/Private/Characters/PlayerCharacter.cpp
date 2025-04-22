// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/PlayerCharacter.h"
#include "Blueprint/UserWidget.h"
#include "Camera/CameraComponent.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/SphereComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/MainPlayerController.h"
#include "GameFramework/MainPlayerState.h"
#include "Interfaces/InteractableInterface.h"
#include "Inventory/InventoryComponent.h"
#include "UI/HUD/InteractionInfoWidget.h"

APlayerCharacter::APlayerCharacter()
{
	FirstPersonCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera"));
	FirstPersonMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("FirstPersonMesh"));
	ThirdPersonMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("ThirdPersonMesh"));

	FirstPersonCamera->SetupAttachment(RootComponent);
	FirstPersonMesh->SetupAttachment(FirstPersonCamera);
	ThirdPersonMesh->SetupAttachment(GetMesh());
	FirstPersonMesh->SetIsReplicated(false);

	InteractDistance = 300.0f;
	InteractDotProduct = 0.7f;
	HighlightedActorUpdateRate = 0.1f;

	InteractionArea = CreateDefaultSubobject<USphereComponent>(TEXT("InteractionArea"));
	InteractionArea->SetupAttachment(RootComponent);
	InteractionArea->SetSphereRadius(InteractDistance);
	InteractionArea->SetGenerateOverlapEvents(true);
	InteractionArea->SetHiddenInGame(false);

	SprintSpeed = 1000.0f;
	WalkSpeed = 500.0f;
}

void APlayerCharacter::BeginPlay()
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
			InteractionWidget = CreateWidget<UInteractionInfoWidget>(GetWorld(), InteractionWidgetClass);
			if (InteractionWidget)
			{
				InteractionWidget->AddToViewport();
				InteractionWidget->SetVisibility(ESlateVisibility::Hidden);
			}
		}
	}

	if (InteractionArea)
	{
		InteractionArea->OnComponentBeginOverlap.AddDynamic(this, &APlayerCharacter::OnOverlapBegin);
		InteractionArea->OnComponentEndOverlap.AddDynamic(this, &APlayerCharacter::OnOverlapEnd);
	}

	InitializeAll();
	TimerUpdateHighlightedActor();
}

void APlayerCharacter::InitializeAll()
{
	if (!MainPS)
	{
		if (GetPlayerState())
		{
			MainPS = Cast<AMainPlayerState>(GetPlayerState());
		}
	}

	if (GetController())
	{
		PC = Cast<AMainPlayerController>(GetController());
	}

	if (!MainPS || !PC)
	{
		FTimerHandle TimerHandle;
		GetWorldTimerManager().SetTimer(TimerHandle, this, &APlayerCharacter::InitializeAll, 0.1, false);
	}
}

void APlayerCharacter::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
}

void APlayerCharacter::TimerUpdateHighlightedActor()
{
	FTimerHandle TimerHandle;
	GetWorldTimerManager().SetTimer(TimerHandle, this, &APlayerCharacter::TimerUpdateHighlightedActor,
	                                HighlightedActorUpdateRate, false);

	if (!IsLocallyControlled())
	{
		return;
	}

	AActor* TargetActor = CheckForInteractableTarget();
	if (TargetActor != CurrentHighlightedActor)
	{
		LastHighlightedActor = CurrentHighlightedActor;
		CurrentHighlightedActor = TargetActor;

		if (CurrentHighlightedActor)
		{
			FVector WorldLocation = CurrentHighlightedActor->GetActorLocation() + FVector(0.f, 0.f, 30.f);
			FVector2D ScreenLocation;
			AMainPlayerController* PlayerController = GetController<AMainPlayerController>();

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
					InteractionWidget->UpdateInteractContainer(Interface->GetInteractTypes(),
					                                           PlayerController->CacheInteractTypeToKey, Interface->GetObjectName());
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
}

void APlayerCharacter::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
                                      UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
                                      bool bFromSweep, const FHitResult& SweepResult)
{
	if (Cast<IInteractableInterface>(OtherActor))
	{
		NearbyInteractiveActors.Add(OtherActor);
	}
}

void APlayerCharacter::OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
                                    UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (Cast<IInteractableInterface>(OtherActor))
	{
		NearbyInteractiveActors.Remove(OtherActor);
	}
}

void APlayerCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
}

AActor* APlayerCharacter::CheckForInteractableTarget()
{
	if (!GetController()) return nullptr;
	FVector PlayerViewLocation;
	FRotator PlayerViewRotation;
	GetController()->GetPlayerViewPoint(PlayerViewLocation, PlayerViewRotation);
	FVector PlayerViewDirection = PlayerViewRotation.Vector();

	AActor* BestTargetForInteract = nullptr;
	float BestDotProduct = 0;
	for (AActor* TargetActor : NearbyInteractiveActors)
	{
		FVector DirectionToTarget = (TargetActor->GetActorLocation() - PlayerViewLocation).GetSafeNormal();
		float DotProduct = FVector::DotProduct(PlayerViewDirection, DirectionToTarget);
		if (DotProduct > InteractDotProduct)
		{
			if (DotProduct > BestDotProduct)
			{
				FCollisionQueryParams QueryParams;
				QueryParams.AddIgnoredActor(this);
				FHitResult OutHit;
				bool bHit = GetWorld()->LineTraceSingleByChannel(
					OutHit,
					PlayerViewLocation,
					TargetActor->GetActorLocation(),
					ECC_Visibility,
					QueryParams
				);

				if (bHit && OutHit.GetActor())
				{
					if (OutHit.GetActor() == TargetActor)
					{
						BestDotProduct = DotProduct;
						BestTargetForInteract = TargetActor;
					}
				}
			}
		}
	}
	return BestTargetForInteract;
}

void APlayerCharacter::TryInteract()
{
	if (AActor* TargetActor = CheckForInteractableTarget())
	{
		ClientPredictInteract(TargetActor);
		ServerInteraction(TargetActor);
	}
}

void APlayerCharacter::ClientPredictInteract(AActor* HitActor)
{
	if (HitActor && !HasAuthority() && MainPS)
	{
		if (AMainItemActor* Item = Cast<AMainItemActor>(HitActor))
		{
			MainPS->InventoryComponent->ClientPredictAddItem(Item->GetClass(), 1);
		}
	}
}

void APlayerCharacter::ServerInteraction_Implementation(AActor* TargetActor)
{
	AActor* ServerActor = CheckForInteractableTarget();
	if (TargetActor != ServerActor)
	{
	}
	if (IInteractableInterface* Interface = Cast<IInteractableInterface>(ServerActor))
	{
		Interface->Interact(MainPS->InventoryComponent);
	}
}

bool APlayerCharacter::ServerInteraction_Validate(AActor* TargetActor)
{
	return TargetActor != nullptr;
}

void APlayerCharacter::StartSprint()
{
	GetCharacterMovement()->MaxWalkSpeed = SprintSpeed;
}

void APlayerCharacter::StopSprint()
{
	GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;
}
