#include "SurvivalAssetManagerStartupJob.h"
#include "SurvivalProject/LogMacros.h"

TSharedPtr<FStreamableHandle> FSurvivalAssetManagerStartupJob::DoJob() const
{
	const double JobStartTime = FPlatformTime::Seconds();

	TSharedPtr<FStreamableHandle> Handle;
	LOGF_FUNC(Display, "Startup job \"%s\" starting", *JobName);
	JobFunc(*this, Handle);

	if (Handle.IsValid())
	{
		Handle->BindUpdateDelegate(
			FStreamableUpdateDelegate::CreateRaw(
				this, &FSurvivalAssetManagerStartupJob::UpdateSubstepProgressFromStreamable));
		Handle->WaitUntilComplete(0.f, false);
		Handle->BindUpdateDelegate(FStreamableUpdateDelegate());
	}

	LOGF_FUNC(Display, "Startup job \"%s\" took %.2f seconds to complete", *JobName, FPlatformTime::Seconds() - JobStartTime);

	return Handle;
}
