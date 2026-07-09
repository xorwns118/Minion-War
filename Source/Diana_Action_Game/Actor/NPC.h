// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "GenericTeamAgentInterface.h"
#include "NPC.generated.h"

UCLASS()
class DIANA_ACTION_GAME_API ANPC : public ACharacter, public IGenericTeamAgentInterface
{
	GENERATED_BODY()

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Component", meta = (DisplayName = "SkillComponent"))
	class USkillComponent*	SkillCom;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Component", meta = (DisplayName = "BuffComponent"))
	class UBuffComponent*	BuffCom;

	FGenericTeamId TeamId;

public:
	class USkillComponent* GetSkillCom() { return SkillCom; }
	class UBuffComponent* GetBuffCom() { return BuffCom; }

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;

	virtual FGenericTeamId GetGenericTeamId() const override { return TeamId; }
	virtual void SetGenericTeamId(const FGenericTeamId& _NewId) override { TeamId = _NewId; }

	virtual ETeamAttitude::Type GetTeamAttitudeTowards(const AActor& _Other) const override;

	virtual float TakeDamage(float _DamageAmount, FDamageEvent const& _DamageEvent, AController* _EventInstigator, AActor* _DamageCauser) override;

public:
	ANPC();
};
