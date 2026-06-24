// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "MyAnimInst.generated.h"

UCLASS()
class DIANA_ACTION_GAME_API UMyAnimInst : public UAnimInstance
{
	GENERATED_BODY()

protected:
	UPROPERTY()
	class AMyPlayer* Player;
	
protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	float GroundSpeed;

public:
	virtual void NativeInitializeAnimation() override;
	virtual void NativeUpdateAnimation(float _DT) override;

public:
	UFUNCTION()
	void AnimNotify_SkillStart();

	UFUNCTION()
	void AnimNotify_SkillEnd();
};
