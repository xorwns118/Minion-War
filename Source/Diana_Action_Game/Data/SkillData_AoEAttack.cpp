// Fill out your copyright notice in the Description page of Project Settings.


#include "SkillData_AoEAttack.h"

bool USkillData_AoEAttack::CanUseSkill_Implementation(APawn* _SkillUser, USkillComponent* _SkillCom)
{
	if (!Super::CanUseSkill_Implementation(_SkillUser, _SkillCom))
		return false;

	return true;
}

void USkillData_AoEAttack::OnExecuteSkill_Implementation(APawn* _SkillUser, USkillComponent* _SkillCom)
{
	Super::OnExecuteSkill_Implementation(_SkillUser, _SkillCom);
}

void USkillData_AoEAttack::OnEndSkill_Implementation(APawn* _SkillUser, USkillComponent* _SkillCom)
{
}
