// Fill out your copyright notice in the Description page of Project Settings.


#include "SkillData_Buff.h"

#include "../Component/SkillComponent.h"

bool USkillData_Buff::CanUseSkill_Implementation(APawn* _SkillUser, USkillComponent* _SkillCom)
{
	if (!Super::CanUseSkill_Implementation(_SkillUser, _SkillCom))
		return false;

	return true;
}

void USkillData_Buff::OnExecuteSkill_Implementation(APawn* _SkillUser, USkillComponent* _SkillCom)
{
	Super::OnExecuteSkill_Implementation(_SkillUser, _SkillCom);

	_SkillCom->DianaShield();
}

void USkillData_Buff::OnEndSkill_Implementation(APawn* _SkillUser, USkillComponent* _SkillCom)
{
	Super::OnEndSkill_Implementation(_SkillUser, _SkillCom);
}
