// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "MonsterAnimInst.generated.h"

UCLASS()
class DIANA_ACTION_GAME_API UMonsterAnimInst : public UAnimInstance
{
	GENERATED_BODY()

protected:
	UPROPERTY()
	class AMonster* Monster;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	float	GroundSpeed;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	bool	IsCombat;

public:
	virtual void NativeInitializeAnimation() override;
	virtual void NativeUpdateAnimation(float _DT) override;

public:
	UFUNCTION()
	void AnimNotify_SkillStart();

	UFUNCTION()
	void AnimNotify_SkillEnd();
};
