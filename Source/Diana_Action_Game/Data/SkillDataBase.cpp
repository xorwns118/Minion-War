// Fill out your copyright notice in the Description page of Project Settings.


#include "SkillDataBase.h"

#include "../Component/SkillComponent.h"

#include "GameFramework/Character.h"
#include "Animation/AnimInstance.h"

#include "GameFramework/CharacterMovementComponent.h"

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

	Character->GetMesh()->GetAnimInstance()->Montage_Play(_SkillCom->GetCurSkillData()->Montage);

	FRotator ActorRotation = _SkillUser->GetActorRotation();
	FRotator YawRotation(0.0f, ActorRotation.Yaw, 0.0f);

	// 2. 회전 값을 바탕으로 앞(X축방향)과 오른쪽(Y축방향) 벡터 구하기
	FVector ForwardVector = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	FVector RightVector = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

	FVector vLaunch = (ForwardVector * LaunchVector.X) + (RightVector * LaunchVector.Y) + (FVector::UpVector * LaunchVector.Z);

	Character->GetCharacterMovement()->Launch(vLaunch);
}

void USkillDataBase::OnEndSkill_Implementation(APawn* _SkillUser, USkillComponent* _SkillCom)
{
}
