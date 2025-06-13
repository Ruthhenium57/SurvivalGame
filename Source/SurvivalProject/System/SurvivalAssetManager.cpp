// Fill out your copyright notice in the Description page of Project Settings.


#include "SurvivalAssetManager.h"
#include "SurvivalGameData.h"
#include "Characters/SurvivalPawnData.h"
#include "SurvivalAssetManagerStartupJob.h"
#include "../LogMacros.h"


const FName FSurvivalBundles::Equipped("Equipped");


static FAutoConsoleCommand CVarDumpLoadedAssets(
	TEXT("Survival.DumpLoadedAssets"),
	TEXT("Shows all assets that were loaded via the asset manager and are currently in memory."),
	FConsoleCommandDelegate::CreateStatic(USurvivalAssetManager::DumpLoadedAssets)
);


#define STARTUP_JOB_WEIGHTED(JobFunc, JobWeight) StartupJobs.Add(FSurvivalAssetManagerStartupJob(#JobFunc, [this](const FSurvivalAssetManagerStartupJob& StartupJob, TSharedPtr<FStreamableHandle>& LoadHandle){JobFunc;}, JobWeight))
#define STARTUP_JOB(JobFunc) STARTUP_JOB_WEIGHTED(JobFunc, 1.f)


USurvivalAssetManager::USurvivalAssetManager()
{
}

USurvivalAssetManager& USurvivalAssetManager::Get()
{
	check(GEngine)

	if (USurvivalAssetManager* Singleton = Cast<USurvivalAssetManager>(GEngine->AssetManager))
	{
		return *Singleton;
	}

	LOGF_FUNC(Error, "Invalid AssetManagerClassName in DefaultEngine.ini.  It must be set to SurvivalAssetManager!")

	// Fatal error above prevents this from being called.
	return *NewObject<USurvivalAssetManager>();
}

void USurvivalAssetManager::DumpLoadedAssets()
{
	LOGF_FUNC(Log, "========== Start Dumping Loaded Assets ==========");

	for (const UObject* LoadedAsset : Get().LoadedAssets)
	{
		LOGF_FUNC(Log, "  %s", *GetNameSafe(LoadedAsset));
	}

	LOGF_FUNC(Log, "... %d assets in loaded pool", Get().LoadedAssets.Num());
	LOGF_FUNC(Log, "========== Finish Dumping Loaded Assets =========");
}

const USurvivalGameData& USurvivalAssetManager::GetGameData()
{
	return GetOrLoadTypedGameData<USurvivalGameData>(SurvivalGameDataPath);
}

const UIMCDataAsset& USurvivalAssetManager::GetIMCDataAsset()
{
	return GetOrLoadTypedGameData<UIMCDataAsset>(IMCDataAssetPath);
}

const USurvivalPawnData* USurvivalAssetManager::GetDefaultPawnData() const
{
	return GetAsset(DefaultPawnData);
}

UObject* USurvivalAssetManager::SynchronousLoadAsset(const FSoftObjectPath& AssetPath)
{
	if (AssetPath.IsValid())
	{
		TUniquePtr<FScopeLogTime> LogTime;
		if (ShouldLogAssetLoads())
		{
			LogTime = MakeUnique<FScopeLogTime>(
				*FString::Printf(TEXT("Synchronously loaded asset [%s]"), *AssetPath.ToString()), nullptr,
				FScopeLogTime::ScopeLog_Seconds);
		}

		if (UAssetManager::IsInitialized())
		{
			return UAssetManager::GetStreamableManager().LoadSynchronous(AssetPath, false);
		}
	}
	
	return nullptr;
}

bool USurvivalAssetManager::ShouldLogAssetLoads()
{
	static bool bLogAssetLoads = FParse::Param(FCommandLine::Get(), TEXT("LogAssetLoads"));
	return bLogAssetLoads;
}

void USurvivalAssetManager::AddLoadedAsset(const UObject* Asset)
{
	if (ensureAlways(Asset))
	{
		FScopeLock LoadedAssetsLock(&LoadedAssetsCritical);
		LoadedAssets.Add(Asset);
	}
}

void USurvivalAssetManager::StartInitialLoading()
{
	SCOPED_BOOT_TIMING("USurvivalAssetManager::StartInitialLoading")

	Super::StartInitialLoading();

	STARTUP_JOB(GetIMCDataAsset());
	// STARTUP_JOB(InitializeGameplayCueManager());
	// {
	// 	STARTUP_JOB_WEIGHTED(GetGameData(), )
	// }

	DoAllStartupJobs();
}

#if WITH_EDITOR
void USurvivalAssetManager::PreBeginPIE(bool bStartSimulate)
{
	Super::PreBeginPIE(bStartSimulate);

	FScopedSlowTask SlowTask(0, NSLOCTEXT("SurvivalEditor", "BeginLoadingPIEData", "Loading PIE Data"));
	const bool bShowCancelButton = false;
	const bool bAllowInPIE = true;
	SlowTask.MakeDialog(bShowCancelButton, bAllowInPIE);

	SCOPE_LOG_TIME_IN_SECONDS(TEXT("PreBeginPIE asset preloading complete"), nullptr);
}
#endif

UPrimaryDataAsset* USurvivalAssetManager::LoadGameDataOfClass(TSubclassOf<UPrimaryDataAsset> DataClass,
                                                              const TSoftObjectPtr<UPrimaryDataAsset>& DataClassPath,
                                                              FPrimaryAssetType PrimaryAssetType)
{
	UPrimaryDataAsset* Asset = nullptr;

	DECLARE_SCOPE_CYCLE_COUNTER(TEXT("Loading GameData Object"), STAT_GameData, STATGROUP_LoadTime);
	if (!DataClassPath.IsNull())
	{
#if WITH_EDITOR
		FScopedSlowTask SlowTask(0, FText::Format(NSLOCTEXT("SurvivalEditor", "BeginLoadingGameDataTask", "Loading GameData {0}"), FText::FromName(DataClass->GetFName())));
		const bool bShowCancelButton = false;
		const bool bAllowInPIE = true;
		SlowTask.MakeDialog(bShowCancelButton, bAllowInPIE);
#endif
		LOGF_FUNC(Log, "Loading GameData: %s ...", *DataClassPath.ToString())
		SCOPE_LOG_TIME_IN_SECONDS(TEXT("    ... GameData loaded!"), nullptr);

		// This can be called recursively in the editor because it is called on demand from PostLoad so force a sync load for primary asset and async load the rest in that case
		if (GIsEditor)
		{
			Asset = DataClassPath.LoadSynchronous();
			LoadPrimaryAssetsWithType(PrimaryAssetType);
		}
		else
		{
			TSharedPtr<FStreamableHandle> Handle = LoadPrimaryAssetsWithType(PrimaryAssetType);
			if (Handle.IsValid())
			{
				Handle->WaitUntilComplete(0.0f, false);

				// This should always work
				Asset = Cast<UPrimaryDataAsset>(Handle->GetLoadedAsset());
			}
		}
	}

	if (Asset)
	{
		GameDataMap.Add(DataClass, Asset);
	}
	else
	{
		// It is not acceptable to fail to load any GameData asset. It will result in soft failures that are hard to diagnose.
		LOGF_FUNC(Fatal, "Failed to load GameData asset at %s. Type %s. This is not recoverable and likely means you do not have the correct data to run %s.", *DataClassPath.ToString(), *PrimaryAssetType.ToString(), FApp::GetProjectName())
	}

	return Asset;
}

void USurvivalAssetManager::DoAllStartupJobs()
{
	SCOPED_BOOT_TIMING("USurvivalAssetManager::DoAllStartupJobs");
	const double AllStartupJobsStartTime = FPlatformTime::Seconds();

	if (IsRunningDedicatedServer())
	{
		// No need for periodic progress updates, just run the jobs
		for (const FSurvivalAssetManagerStartupJob& StartupJob : StartupJobs)
		{
			StartupJob.DoJob();
		}
	}
	else
	{
		if (StartupJobs.Num() > 0)
		{
			float TotalJobValue = 0.f;
			for (const FSurvivalAssetManagerStartupJob& StartupJob : StartupJobs)
			{
				TotalJobValue += StartupJob.JobWeight;
			}

			float AccumulatedJobValue = 0.f;
			for (FSurvivalAssetManagerStartupJob& StartupJob : StartupJobs)
			{
				const float JobValue = StartupJob.JobWeight;
				StartupJob.SubstepProgressDelegate.BindLambda(
					[This = this, AccumulatedJobValue, JobValue, TotalJobValue](float NewProgress)
					{
						const float SubstepAdjustment = FMath::Clamp(NewProgress, 0.f, 1.f) * JobValue;
						const float OverallPercentWithSubstep = (AccumulatedJobValue + SubstepAdjustment) / TotalJobValue;

						This->UpdateInitialGameContentLoadPercent(OverallPercentWithSubstep);
					});

				StartupJob.DoJob();

				StartupJob.SubstepProgressDelegate.Unbind();

				AccumulatedJobValue += JobValue;

				UpdateInitialGameContentLoadPercent(AccumulatedJobValue / TotalJobValue);
			}
		}
		else
		{
			UpdateInitialGameContentLoadPercent(1.f);
		}
	}

	StartupJobs.Empty();

	LOGF_FUNC(Display, "All startup jobs took %.2f seconds to complete", FPlatformTime::Seconds() - AllStartupJobsStartTime);
}

void USurvivalAssetManager::UpdateInitialGameContentLoadPercent(float GameContentPercent)
{
	// Could route this to the early startup loading screen
}
