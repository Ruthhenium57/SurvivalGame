#pragma once

SURVIVALPROJECT_API DECLARE_LOG_CATEGORY_EXTERN(LogSurvival, Log, All);
SURVIVALPROJECT_API DECLARE_LOG_CATEGORY_EXTERN(LogSurvivalAbilitySystem, Log, All);

#define LOGF_FUNC(Verbosity, Format, ...) \
UE_LOG(LogSurvival, Verbosity, TEXT("[%s:%d] " Format), TEXT(__FUNCTION__), __LINE__, ##__VA_ARGS__)

#define LOGF_CATEGORY(Category, Verbosity, Format, ...) \
UE_LOG(Category, Verbosity, TEXT("[%s:%d] " Format), TEXT(__FUNCTION__), __LINE__, ##__VA_ARGS__)

#define LOGW(Format, ...) LOGF_FUNC(Warning, Format, ##__VA_ARGS__)
#define LOGE(Format, ...) LOGF_FUNC(Error, Format, ##__VA_ARGS__)
#define LOGD(Format, ...) LOGF_FUNC(Display, Format, ##__VA_ARGS__)
#define LOGL(Format, ...) LOGF_FUNC(Log, Format, ##__VA_ARGS__)