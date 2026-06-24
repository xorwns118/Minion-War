// Fill out your copyright notice in the Description page of Project Settings.


#include "MyAnimInst.h"

#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"

#include "KismetAnimationLibrary.h"

#include "../Actor/MyPlayer.h"
#include "../Component/SkillComponent.h"

void UMyAnimInst::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	Player = Cast<AMyPlayer>(TryGetPawnOwner());
}

void UMyAnimInst::NativeUpdateAnimation(float _DT)
{
	Super::NativeUpdateAnimation(_DT);

	if (Player == nullptr)
		return;

	ACharacter* Character = Cast<ACharacter>(Player);

	if (Character == nullptr)
		return;

	FVector Velocity = Character->GetVelocity();
	GroundSpeed = Velocity.Size2D();
}

void UMyAnimInst::AnimNotify_SkillStart()
{
	Player->GetSkillCom()->SkillStart();
}

void UMyAnimInst::AnimNotify_SkillEnd()
{
	Player->GetSkillCom()->SkillEnd();
}
