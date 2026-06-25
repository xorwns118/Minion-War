// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "SkillDataBase.generated.h"

UENUM(BlueprintType)
enum class ESkillType : uint8
{
	MELEE,
	PROJECTILE,
	BUFF,
};

UCLASS()
class DIANA_ACTION_GAME_API USkillDataBase : public UPrimaryDataAsset
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Skill|Info")
	ESkillType SkillType;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Skill|Info")
	FName SkillName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Skill|Info")
	FText DisplayName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Skill|Info")
	FText Description;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Skill|Anim")
	class UAnimMontage* Montage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Skill|Effect")
	class UNiagaraSystem* HitEffect;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Skill|Info")
	float CoolTime;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Skill|Info")
	float Cost;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Skill|Info")
	float Range;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Skill|Info")
	bool CanJump;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Skill|Info")
	bool CanMove;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Skill|Info")
	bool CanCombo;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Skill|Info", meta = (EditCondition = "CanCombo"))
	int32 ComboCount = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Skill|Info")
	bool UseInAir;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Skill|Info")
	float MoveSpeedScale;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Skill|Info")
	float Damage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Skill|HitBox")
	FName SocketName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Skill|HitBox")
	FVector BoxSize;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Skill|Test")
	bool IsLaunch;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Skill|Test", meta = (EditCondition = "IsLaunch"))
	FVector LaunchVector;

public:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Skill Logic")
	bool CanUseSkill(class APawn* _SkillUser, class USkillComponent* _SkillCom);
	virtual bool CanUseSkill_Implementation(class APawn* _SkillUser, class USkillComponent* _SkillCom);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Skill Logic")
	void OnExecuteSkill(class APawn* _SkillUser, class USkillComponent* _SkillCom);
	virtual void OnExecuteSkill_Implementation(class APawn* _SkillUser, class USkillComponent* _SkillCom);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Skill Logic")
	void OnEndSkill(class APawn* _SkillUser, class USkillComponent* _SkillCom);
	virtual void OnEndSkill_Implementation(class APawn* _SkillUser, class USkillComponent* _SkillCom);

	virtual FPrimaryAssetId GetPrimaryAssetId() const override
	{
		return FPrimaryAssetId("SkillData", GetFName());
	}
};
