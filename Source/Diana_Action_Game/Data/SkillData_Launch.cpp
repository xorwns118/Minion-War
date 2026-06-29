// Fill out your copyright notice in the Description page of Project Settings.


#include "SkillData_Launch.h"

#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"

#include "../Component/SkillComponent.h"

bool USkillData_Launch::CanUseSkill_Implementation(APawn* _SkillUser, USkillComponent* _SkillCom)
{
	if (!Super::CanUseSkill_Implementation(_SkillUser, _SkillCom))
		return false;

	return true;
}

void USkillData_Launch::OnExecuteSkill_Implementation(APawn* _SkillUser, USkillComponent* _SkillCom)
{
	Super::OnExecuteSkill_Implementation(_SkillUser, _SkillCom);

	ACharacter* Character = Cast<ACharacter>(_SkillUser);

	if (Character == nullptr)
		return;

	FRotator ActorRotation = _SkillUser->GetActorRotation();
	FRotator YawRotation(0.0f, ActorRotation.Yaw, 0.0f);

	FVector ForwardVector = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	FVector RightVector = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

	FVector vLaunch = (ForwardVector * LaunchVector.X) + (RightVector * LaunchVector.Y) + (FVector::UpVector * LaunchVector.Z);

	Character->GetCharacterMovement()->Launch(vLaunch);
}

void USkillData_Launch::OnEndSkill_Implementation(APawn* _SkillUser, USkillComponent* _SkillCom)
{
}
