#pragma once

#include "CoreMinimal.h"

UENUM(BlueprintType)
enum class ETeamType : uint8
{
	Player	UMETA(DisplayName = "Player"),
	Enemy	UMETA(DisplayName = "Enemy"),
	Neutral	UMETA(DisplayName = "None"),
};