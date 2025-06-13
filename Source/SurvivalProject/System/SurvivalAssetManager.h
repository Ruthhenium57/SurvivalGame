// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SurvivalAssetManagerStartupJob.h"
#include "Engine/AssetManager.h"
#include "Characters/SurvivalPawnData.h"
#include "Input/IMCDataAsset.h"
#include "SurvivalAssetManager.generated.h"

class UInputMappingContext;
class USurvivalGameData;

struct FSurvivalBundles
{
	static const FName Equipped;
};

/**
 * 
 */
UCLASS(Config = Game)
class SURVIVALPROJECT_API USurvivalAssetManager : public UAssetManager
{
	GENERATED_BODY()

public:
	USurvivalAssetManager();

	// Returns the AssetManager singleton object.
	static USurvivalAssetManager& Get();

	// Returns the asset referenced by a TSoftObjectPtr.  This will synchronously load the asset if it's not already loaded.
	template <typename AssetType>
	static AssetType* GetAsset(const TSoftObjectPtr<AssetType>& AssetPtr, bool bKeepInMemory = true);

	// Returns the subclass referenced by a TSoftClassPtr.  This will synchronously load the asset if it's not already loaded.
	template <typename AssetType>
	static TSubclassOf<AssetType> GetSubclass(const TSoftClassPtr<AssetType>& AssetPtr, bool bKeepInMemory = true);

	// Logs all assets currently loaded and tracked by the asset manager.
	static void DumpLoadedAssets();

	const USurvivalGameData& GetGameData();
	const UIMCDataAsset& GetIMCDataAsset();
	const USurvivalPawnData* GetDefaultPawnData() const;

protected:
	template <typename GameDataClass>
	const GameDataClass& GetOrLoadTypedGameData(const TSoftObjectPtr<GameDataClass>& DataPath)
	{
		if (TObjectPtr<UPrimaryDataAsset> const* pResult = GameDataMap.Find(GameDataClass::StaticClass()))
		{
			return *CastChecked<GameDataClass>(*pResult);
		}

		// Does a blocking load if needed
		return *CastChecked<const GameDataClass>(
			LoadGameDataOfClass(GameDataClass::StaticClass(), DataPath, GameDataClass::StaticClass()->GetFName()));
	}

	static UObject* SynchronousLoadAsset(const FSoftObjectPath& AssetPath);
	static bool ShouldLogAssetLoads();

	// Thread safe way of adding a loaded asset to keep in memory.
	void AddLoadedAsset(const UObject* Asset);

	//~ Begin UAssetManager interface
	virtual void StartInitialLoading() override;
#if WITH_EDITOR
	virtual void PreBeginPIE(bool bStartSimulate) override;
#endif
	//~ End of UAssetManager interface

	UPrimaryDataAsset* LoadGameDataOfClass(TSubclassOf<UPrimaryDataAsset> DataClass,
	                                       const TSoftObjectPtr<UPrimaryDataAsset>& DataClassPath,
	                                       FPrimaryAssetType PrimaryAssetType);

	// Global game data asset to use.
	UPROPERTY(Config)
	TSoftObjectPtr<USurvivalGameData> SurvivalGameDataPath;

	// Data asset with all IMC settings
	UPROPERTY(Config)
	TSoftObjectPtr<UIMCDataAsset> IMCDataAssetPath;
	
	// Loaded version of the game data
	UPROPERTY(Transient)
	TMap<TObjectPtr<UClass>, TObjectPtr<UPrimaryDataAsset>> GameDataMap;

	// Pawn data used when spawning player pawns if there isn't one set on the player state.
	UPROPERTY(Config)
	TSoftObjectPtr<USurvivalPawnData> DefaultPawnData;

private:
	// Flushes the StartupJobs array. Processes all startup work.
	void DoAllStartupJobs();

	// Sets up the ability system
	//void InitializeGameplayCueManager();

	// Called periodically during loads, could be used to feed the status to a loading screen
	void UpdateInitialGameContentLoadPercent(float GameContentPercent);

	// The list of tasks to execute on startup. Used to track startup progress.
	TArray<FSurvivalAssetManagerStartupJob> StartupJobs;

	// Assets loaded and tracked by the asset manager.
	UPROPERTY()
	TSet<TObjectPtr<const UObject>> LoadedAssets;

	// Used for a scope lock when modifying the list of load assets.
	FCriticalSection LoadedAssetsCritical;
};

template <typename AssetType>
AssetType* USurvivalAssetManager::GetAsset(const TSoftObjectPtr<AssetType>& AssetPtr, bool bKeepInMemory)
{
	AssetType* LoadedAsset = nullptr;

	const FSoftObjectPath& AssetPath = AssetPtr.ToSoftObjectPath();

	if (AssetPath.IsValid())
	{
		LoadedAsset = AssetPtr.Get();
		if (!LoadedAsset)
		{
			LoadedAsset = Cast<AssetType>(SynchronousLoadAsset(AssetPath));
			ensureAlwaysMsgf(LoadedAsset, TEXT("Failed to load asset [&s]"), *AssetPtr.ToString());
		}

		if (LoadedAsset && bKeepInMemory)
		{
			Get().AddLoadedAsset(Cast<UObject>(LoadedAsset));
		}
	}

	return LoadedAsset;
}

template <typename AssetType>
TSubclassOf<AssetType> USurvivalAssetManager::GetSubclass(const TSoftClassPtr<AssetType>& AssetPtr, bool bKeepInMemory)
{
	TSubclassOf<AssetType> LoadedSubclass;

	const FSoftObjectPath& AssetPath = AssetPtr.ToSoftObjectPath();

	if (AssetPath.IsValid())
	{
		LoadedSubclass = AssetPtr.Get();
		if (!LoadedSubclass)
		{
			LoadedSubclass = Cast<UClass>(SynchronousLoadAsset(AssetPath));
			ensureAlwaysMsgf(LoadedSubclass, TEXT("Failed to load asset class [%s]"), *AssetPath.ToString());
		}

		if (LoadedSubclass && bKeepInMemory)
		{
			Get().AddLoadedAsset(Cast<UClass>(LoadedSubclass));
		}
	}

	return LoadedSubclass;
}
