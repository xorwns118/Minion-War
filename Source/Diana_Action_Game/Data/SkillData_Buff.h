// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SkillDataBase.h"
#include "../GlobalEnum.h"
#include "SkillData_Buff.generated.h"

UCLASS()
class DIANA_ACTION_GAME_API USkillData_Buff : public USkillDataBase
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Skill|Buff")
	EBuffType				BuffType;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Skill|Buff")
	float					BuffDuration;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Skill|Buff")
	float					Value;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Skill|Buff")
	class UNiagaraSystem*	BuffEffect;

public:
	virtual bool CanUseSkill_Implementation(class APawn* _SkillUser, class USkillComponent* _SkillCom);
	virtual void OnExecuteSkill_Implementation(class APawn* _SkillUser, class USkillComponent* _SkillCom);
	virtual void OnEndSkill_Implementation(class APawn* _SkillUser, class USkillComponent* _SkillCom);
};
