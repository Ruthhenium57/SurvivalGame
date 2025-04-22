// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MainCharacter.h"
#include "PlayerCharacter.generated.h"

class AMainPlayerController;
class UInteractionInfoWidget;
class UCameraComponent;
class USphereComponent;
class AMainPlayerState;

UCLASS()
class RPG_PROJECT_API APlayerCharacter : public AMainCharacter
{
	GENERATED_BODY()

public:
	APlayerCharacter();

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UFUNCTION()
	void InitializeAll();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Components)
	TObjectPtr<AMainPlayerState> MainPS;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Interaction", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<UInteractionInfoWidget> InteractionWidgetClass;
	
	UPROPERTY()
	TObjectPtr<UInteractionInfoWidget> InteractionWidget;
	
	UPROPERTY()
	TObjectPtr<USphereComponent> InteractionArea;
	
	UPROPERTY()
	TArray<AActor*> NearbyInteractiveActors;

	UPROPERTY()
	TObjectPtr<AMainPlayerController> PC;
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Camera)
	TObjectPtr<UCameraComponent> FirstPersonCamera;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Camera)
	TObjectPtr<UMeshComponent> FirstPersonMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Camera)
	TObjectPtr<UMeshComponent> ThirdPersonMesh;
	
	/** find the best actor nearby to interact with and return it */
	UFUNCTION(BlueprintCallable, Category = "Interaction")
	AActor* CheckForInteractableTarget();

	/** called when press interact button */
	UFUNCTION(BlueprintCallable, Category = "Interaction")
	void TryInteract();

	void StartSprint();
	void StopSprint();

protected:
	UFUNCTION()
	void TimerUpdateHighlightedActor();
	
	/** create some widgets on client when interact */
	UFUNCTION(BlueprintCallable, Category = "Interaction")
	void ClientPredictInteract(AActor* HitActor);
	
	/** call interact on server */
	UFUNCTION(Server, Reliable, WithValidation)
	void ServerInteraction(AActor* TargetActor);

	/** called when something enters the sphere component */
	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep,
	                    const FHitResult& SweepResult);

	/** called when something leaves the sphere component */
	UFUNCTION()
	void OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

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

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Interaction", meta = (AllowPrivateAccess = "true"))
	float SprintSpeed;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Interaction", meta = (AllowPrivateAccess = "true"))
	float WalkSpeed;
};
