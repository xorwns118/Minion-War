#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "GlobalData.generated.h"

USTRUCT(BlueprintType)
struct FStatData : public FTableRowBase // DataTable 로 노출되기 위해 꼭 상속 받아햐 하는 구조체
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stat")
	float MaxHP;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stat")
	float MaxMP;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stat")
	float Att;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stat")
	float Def;
};

// Player 전용
USTRUCT(BlueprintType)
struct FPlayerStatData : public FStatData
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stat")
	float MaxUP; // 궁극기 게이지
};

// Monster 전용
USTRUCT(BlueprintType)
struct FMonsterStatData : public FStatData
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stat")
	float DropExp;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stat")
	float DetectRange;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stat")
	float LoseDetectRange;
};