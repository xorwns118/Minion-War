// Fill out your copyright notice in the Description page of Project Settings.


#include "SkillData_Projectile.h"

bool USkillData_Projectile::CanUseSkill_Implementation(APawn* _SkillUser, USkillComponent* _SkillCom)
{
	if (!Super::CanUseSkill_Implementation(_SkillUser, _SkillCom))
		return false;

	return true;
}

void USkillData_Projectile::OnExecuteSkill_Implementation(APawn* _SkillUser, USkillComponent* _SkillCom)
{
	Super::OnExecuteSkill_Implementation(_SkillUser, _SkillCom);
}

void USkillData_Projectile::OnEndSkill_Implementation(APawn* _SkillUser, USkillComponent* _SkillCom)
{
	Super::OnEndSkill_Implementation(_SkillUser, _SkillCom);
}