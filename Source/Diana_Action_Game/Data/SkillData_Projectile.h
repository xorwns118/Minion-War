// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SkillDataBase.h"
#include "SkillData_Projectile.generated.h"

UCLASS()
class DIANA_ACTION_GAME_API USkillData_Projectile : public USkillDataBase
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Skill|Projectile")
	TSubclassOf<class AProjectile> ProjectileClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Skill|Projectile")
	FName SpawnSocketName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Skill|Projectile")
	bool IsHitExplosion;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Skill|Projectile", meta = (EditCondition = "IsHitExplosion"))
	float DamageRadius;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Skill|Projectile", meta = (EditCondition = "IsHitExplosion"))
	class UNiagaraSystem* ExplosionEffect;

public:
	virtual bool CanUseSkill_Implementation(class APawn* _SkillUser, class USkillComponent* _SkillCom);
	virtual void OnExecuteSkill_Implementation(class APawn* _SkillUser, class USkillComponent* _SkillCom);
	virtual void OnEndSkill_Implementation(class APawn* _SkillUser, class USkillComponent* _SkillCom);
};
