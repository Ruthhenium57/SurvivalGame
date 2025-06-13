// Fill out your copyright notice in the Description page of Project Settings.


#include "SurvivalCharacterMovementComponent.h"
#include "AbilitySystemGlobals.h"
#include "AbilitySystemComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/Character.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(SurvivalCharacterMovementComponent)

UE_DEFINE_GAMEPLAY_TAG(TAG_Gameplay_MovementStopped, "Gameplay.MovementStopped");

namespace SurvivalCharacter
{
	static float GroundTraceDistance = 100000.0f;
	FAutoConsoleVariableRef CVar_GroundTraceDistance(
		TEXT("SurvivalCharacter.GroundTraceDistance"), GroundTraceDistance,
		TEXT("Distance to trace down when generating ground information."), ECVF_Cheat);
}

USurvivalCharacterMovementComponent::USurvivalCharacterMovementComponent(
	const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
}

const FSurvivalCharacterGroundInfo& USurvivalCharacterMovementComponent::GetGroundInfo()
{
	if (!CharacterOwner || (GFrameCounter == CachedGroundInfo.LastUpdateFrame))
	{
		return CachedGroundInfo;
	}

	if (MovementMode == MOVE_Walking)
	{
		CachedGroundInfo.GroundHitResult = CurrentFloor.HitResult;
		CachedGroundInfo.GroundDistance = 0.0f;
	}
	else
	{
		const UCapsuleComponent* CapsuleComp = CharacterOwner->GetCapsuleComponent();
		check(CapsuleComp);

		const float CapsuleHalfHeight = CapsuleComp->GetUnscaledCapsuleHalfHeight();
		const ECollisionChannel CollisionChannel = (UpdatedComponent
			                                            ? UpdatedComponent->GetCollisionObjectType()
			                                            : ECC_Pawn);
		const FVector TraceStart(GetActorLocation());
		const FVector TraceEnd(TraceStart.X, TraceStart.Y,
		                       (TraceStart.Z - SurvivalCharacter::GroundTraceDistance - CapsuleHalfHeight));

		FCollisionQueryParams QueryParams(
			SCENE_QUERY_STAT(LyraCharacterMovementComponent_GetGroundInfo), false, CharacterOwner);
		FCollisionResponseParams ResponseParam;
		InitCollisionParams(QueryParams, ResponseParam);

		FHitResult HitResult;
		GetWorld()->LineTraceSingleByChannel(HitResult, TraceStart, TraceEnd, CollisionChannel, QueryParams,
		                                     ResponseParam);

		CachedGroundInfo.GroundHitResult = HitResult;
		CachedGroundInfo.GroundDistance = SurvivalCharacter::GroundTraceDistance;

		if (MovementMode == MOVE_NavWalking)
		{
			CachedGroundInfo.GroundDistance = 0.0f;
		}
		else if (HitResult.bBlockingHit)
		{
			CachedGroundInfo.GroundDistance = FMath::Max((HitResult.Distance - CapsuleHalfHeight), 0.0f);
		}
	}

	CachedGroundInfo.LastUpdateFrame = GFrameCounter;

	return CachedGroundInfo;
}

float USurvivalCharacterMovementComponent::GetMaxSpeed() const
{
	if (const UAbilitySystemComponent* ASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(GetOwner()))
	{
		if (ASC->HasMatchingGameplayTag(TAG_Gameplay_MovementStopped))
		{
			return 0;
		}
	}

	return Super::GetMaxSpeed();
}

void USurvivalCharacterMovementComponent::InitializeComponent()
{
	Super::InitializeComponent();
}
