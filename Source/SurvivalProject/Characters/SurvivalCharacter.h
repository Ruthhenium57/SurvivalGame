// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemInterface.h"
#include "ModularCharacter.h"
#include "GameplayTagAssetInterface.h"
#include "SurvivalCharacter.generated.h"

class USurvivalPawnData;
class UPlayerStatsComponent;
class USurvivalPawnExtensionComponent;
class UAbilitySystemComponent;
class USurvivalAbilitySystemComponent;
class USurvivalCharacterComponent;
class ASurvivalPlayerState;
class ASurvivalPlayerController;
class UInteractInfoWidget;
class UCameraComponent;
class USphereComponent;

UCLASS()
class SURVIVALPROJECT_API ASurvivalCharacter : public AModularCharacter, public IAbilitySystemInterface, public IGameplayTagAssetInterface
{
	GENERATED_BODY()

public:
	ASurvivalCharacter(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	UFUNCTION(BlueprintCallable, Category = "Survival|Character")
	ASurvivalPlayerController* GetSurvivalPlayerController() const;

	UFUNCTION(BlueprintCallable, Category = "Survival|Character")
	ASurvivalPlayerState* GetSurvivalPlayerState() const;

	UFUNCTION(BlueprintCallable, Category = "Survival|Character")
	USurvivalAbilitySystemComponent* GetSurvivalAbilitySystemComponent() const;
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;
	
	virtual void GetOwnedGameplayTags(FGameplayTagContainer& TagContainer) const override;
	virtual bool HasMatchingGameplayTag(FGameplayTag TagToCheck) const override;
	virtual bool HasAllMatchingGameplayTags(const FGameplayTagContainer& TagContainer) const override;
	virtual bool HasAnyMatchingGameplayTags(const FGameplayTagContainer& TagContainer) const override;

	void ToggleCrouch();
	
	//~AActor interface
	virtual void PreInitializeComponents() override;
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	virtual void Tick(float DeltaSeconds) override;
	virtual void Reset() override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void PreReplication(IRepChangedPropertyTracker& ChangedPropertyTracker) override;
	//~End of AActor interface

	//~APawn interface
	virtual void NotifyControllerChanged() override;
	//~End of APawn interface

protected:
	
	virtual void OnAbilitySystemInitialized();
	virtual void OnAbilitySystemUninitialized();

	virtual void PossessedBy(AController* NewController) override;
	virtual void UnPossessed() override;

	virtual void OnRep_Controller() override;
	virtual void OnRep_PlayerState() override;

	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;

	void InitializeGameplayTags();

	// Begins the death sequence for the character (disables collision, disables movement, etc...)
	UFUNCTION()
	virtual void OnDeathStarted(AActor* OwningActor);

	// Ends the death sequence for the character (detaches controller, destroys pawn, etc...)
	UFUNCTION()
	virtual void OnDeathFinished(AActor* OwningActor);

	void DisableMovementAndCollision();
	
	virtual void OnMovementModeChanged(EMovementMode PrevMovementMode, uint8 PreviousCustomMode) override;
	void SetMovementModeTag(EMovementMode MovementMode, uint8 CustomMovementMode, bool bTagEnabled);
	
	virtual void OnStartCrouch(float HalfHeightAdjust, float ScaledHalfHeightAdjust) override;
	virtual void OnEndCrouch(float HalfHeightAdjust, float ScaledHalfHeightAdjust) override;

	virtual bool CanJumpInternal_Implementation() const;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera")
	TObjectPtr<UCameraComponent> FirstPersonCamera;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera")
	TObjectPtr<UMeshComponent> FirstPersonMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera")
	TObjectPtr<UMeshComponent> ThirdPersonMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interaction")
	TObjectPtr<USphereComponent> InteractionArea;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Interaction")
	TSubclassOf<UInteractInfoWidget> InteractionWidgetClass;

	void TimerUpdateHighlightedActor();
	// find the best actor nearby to interact with and return it 
	AActor* CheckForInteractableTarget();
	// called when press interact button 
	void TryInteract();
	// create some widgets on client when interact 
	void ClientPredictInteract(AActor* HitActor);
	// call interact on server 
	UFUNCTION(Server, Reliable, WithValidation)
	void ServerInteraction(AActor* TargetActor);

	UPROPERTY()
	TObjectPtr<UInteractInfoWidget> InteractionWidget;

	UPROPERTY()
	TArray<TObjectPtr<AActor>> NearbyInteractiveActors;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Interaction", meta = (AllowPrivateAccess = "true"))
	float InteractDistance;
	//dot product. 1 is a small interact angle. 0 - large (90)
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Interaction", meta = (AllowPrivateAccess = "true"))
	float InteractDotProduct;
	//0.1 - 10 tps; 0.5 - 20 tps
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Interaction", meta = (AllowPrivateAccess = "true"))
	float HighlightedActorUpdateRate;

	UPROPERTY()
	TObjectPtr<AActor> CurrentHighlightedActor;
	UPROPERTY()
	TObjectPtr<AActor> LastHighlightedActor;

	/** called when something enters the sphere component */
	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep,
	                    const FHitResult& SweepResult);

	/** called when something leaves the sphere component */
	UFUNCTION()
	void OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Survival|Character", Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<USurvivalPawnExtensionComponent> PawnExtComponent;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Survival|Character", Meta = (AllowPrivateAccess = "true"))
    TObjectPtr<USurvivalCharacterComponent> CharacterComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Survival|Character", Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UPlayerStatsComponent> PlayerStatsComponent;
};
