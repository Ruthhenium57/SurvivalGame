// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "SurvivalInputMappingSubsystem.generated.h"

class UIMCAndPriority;
class UInputMappingContext;
/**
 * 
 */
UCLASS()
class SURVIVALPROJECT_API USurvivalInputMappingSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

	UIMCAndPriority* GetContextByName(FName Name) const;
	const TMap<FName, UIMCAndPriority*>& GetAllContexts() const { return ContextRegistry; }

	const TArray<FName>& GetGlobalContexts() const { return GlobalContexts; }
	void SetGlobalContexts(const TArray<FName>& Contexts) { GlobalContexts = Contexts; }

protected:
	UPROPERTY()
	TMap<FName, UIMCAndPriority*> ContextRegistry;
	TArray<FName> GlobalContexts;
};
