// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SkillDataBase.h"
#include "SkillData_AoEAttack.generated.h"

UCLASS()
class DIANA_ACTION_GAME_API USkillData_AoEAttack : public USkillDataBase
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Skill|AoE")
	float HitRadius;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Skill|AoE")
	FVector LocationOffset;

public:
	virtual bool CanUseSkill_Implementation(class APawn* _SkillUser, class USkillComponent* _SkillCom);
	virtual void OnExecuteSkill_Implementation(class APawn* _SkillUser, class USkillComponent* _SkillCom);
	virtual void OnEndSkill_Implementation(class APawn* _SkillUser, class USkillComponent* _SkillCom);
};
