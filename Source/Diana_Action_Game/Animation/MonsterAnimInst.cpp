// Fill out your copyright notice in the Description page of Project Settings.


#include "MonsterAnimInst.h"

#include "../Actor/Monster/Monster.h"
#include "../Component/SkillComponent.h"

void UMonsterAnimInst::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	Monster = Cast<AMonster>(TryGetPawnOwner());
}

void UMonsterAnimInst::NativeUpdateAnimation(float _DT)
{
	Super::NativeUpdateAnimation(_DT);

	if (Monster == nullptr)
		return;

	ACharacter* Character = Cast<ACharacter>(Monster);

	if (Character == nullptr)
		return;

	FVector Velocity = Character->GetVelocity();
	GroundSpeed = Velocity.Size2D();
}

void UMonsterAnimInst::AnimNotify_SkillStart()
{
	Monster->GetSkillCom()->SkillStart();
}

void UMonsterAnimInst::AnimNotify_SkillEnd()
{
	Monster->GetSkillCom()->SkillEnd();
}
