// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "GenericTeamAgentInterface.h"
#include "MyPlayer.generated.h"

UCLASS()
class DIANA_ACTION_GAME_API AMyPlayer : public ACharacter, public IGenericTeamAgentInterface
{
	GENERATED_BODY()

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Component", meta = (DisplayName = "SpringArm"))
	class USpringArmComponent*	SpringArmCom;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Component", meta = (DisplayName = "CameraComponent"))
	class UCameraComponent*		CameraCom;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Component", meta = (DisplayName = "SkillComponent"))
	class USkillComponent*		SkillCom;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Component", meta = (DisplayName = "StatComponent"))
	class UPlayerStatComponent* StatCom;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Component", meta = (DisplayName = "BuffComponent"))
	class UBuffComponent*		BuffCom;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Input", meta = (AllowPrivateAccess = "true"), Instanced)
	class UInputContainer*		InputContainer;
	
	float			BaseMoveSpeed;

	FGenericTeamId	TeamId;

	bool			IsCC;

public:
	
public:
	class USkillComponent* GetSkillCom() { return SkillCom; }
	class UBuffComponent* GetBuffCom() { return BuffCom; }

	void SetCrowdControlled(bool _IsCC) { IsCC = _IsCC; }
	bool IsCrowdControlled() { return IsCC; }

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	virtual FGenericTeamId GetGenericTeamId() const override { return TeamId; }
	virtual void SetGenericTeamId(const FGenericTeamId& _NewId) override { TeamId = _NewId; }

	virtual ETeamAttitude::Type GetTeamAttitudeTowards(const AActor& _Other) const override;

	virtual float TakeDamage(float _DamageAmount, FDamageEvent const& _DamageEvent, AController* _EventInstigator, AActor* _DamageCauser) override;

public:
	void MoveAction(const struct FInputActionValue& _Value);
	void LookAction(const struct FInputActionValue& _Value);

public:
	AMyPlayer();
};
