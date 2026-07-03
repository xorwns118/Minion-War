// Fill out your copyright notice in the Description page of Project Settings.


#include "SkillDataBase.h"

#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Animation/AnimInstance.h"

#include "../Component/SkillComponent.h"

bool USkillDataBase::CanUseSkill_Implementation(APawn* _SkillUser, USkillComponent* _SkillCom)
{
	if (_SkillCom->IsSkillCoolTime(GetPrimaryAssetId(), CoolTime))
	{
		float RemainCoolTime = _SkillCom->GetSkillRemainCoolTime(GetPrimaryAssetId(), CoolTime);

		UE_LOG(LogTemp, Warning, TEXT("Skill Remain CoolTime : %f"), RemainCoolTime);

		return false;
	}

	return true;
}

void USkillDataBase::OnExecuteSkill_Implementation(APawn* _SkillUser, USkillComponent* _SkillCom)
{
	_SkillCom->AddSkillUseTime(GetPrimaryAssetId());

	ACharacter* Character = Cast<ACharacter>(_SkillUser);

	if (Character == nullptr)
		return;

	if (CanMove)
		Character->GetCharacterMovement()->MaxWalkSpeed *= MoveSpeedScale;
	else
		Character->GetCharacterMovement()->MaxWalkSpeed *= 0;

	Character->GetCharacterMovement()->RotationRate.Yaw *= RotateSpeedScale;

	Character->GetMesh()->GetAnimInstance()->Montage_Play(_SkillCom->GetCurSkillData()->Montage);
}

void USkillDataBase::OnEndSkill_Implementation(APawn* _SkillUser, USkillComponent* _SkillCom)
{
	ACharacter* Character = Cast<ACharacter>(_SkillUser);

	if (Character == nullptr)
		return;

	Character->GetCharacterMovement()->MaxWalkSpeed = _SkillCom->GetBaseMoveSpeed();
	Character->GetCharacterMovement()->RotationRate.Yaw = _SkillCom->GetBaseYawRotateSpeed();
}
