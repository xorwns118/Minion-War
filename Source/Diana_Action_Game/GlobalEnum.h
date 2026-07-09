#pragma once

#include "CoreMinimal.h"

UENUM(BlueprintType)
enum class ETeamType : uint8
{
	Player	UMETA(DisplayName = "Player"),
	Enemy	UMETA(DisplayName = "Enemy"),
	Neutral	UMETA(DisplayName = "None"),
};

UENUM(BlueprintType)
enum class EBuffType : uint8
{
	None,

	AtkModifier,	// 공격력 증가 효과
	DefModifier,	// 방어력 증가 효과
	Shield,			// 보호막
	PeriodicDamage,	// 지속 데미지
	PeriodicHeal,	// 지속 효과 (체력 재생, 마나 재생...)
	CrowdControl,	// 군중 제어
	StateFlag,		// 무적, 슈퍼아머, 은신
};