#include "HAL/IConsoleManager.h"

namespace SurvivalProjectConsoleCommands
{
	static TAutoConsoleVariable<float> CVarCustomMovementSpeed(
		TEXT("SurvivalProject.Movement.CustomSpeed"), 
		100.0f,                               
		TEXT("Sets the custom movement speed for a specific feature.\n")
		TEXT("Value is in cm/s."),             
		ECVF_Cheat                           
	);

	static TAutoConsoleVariable<bool> CVarDebugInteractionTrace(
		TEXT("SurvivalProject.DebugInteraction"), 
		false, 
		TEXT("Enable debug visualization for the interaction check function.\n")
		TEXT("0: disabled, 1: enabled"),
		ECVF_Cheat 
);
}