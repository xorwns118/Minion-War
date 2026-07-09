// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "../NPC.h"
#include "Monster.generated.h"

UCLASS()
class DIANA_ACTION_GAME_API AMonster : public ANPC
{
	GENERATED_BODY()

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Component", meta = (DisplayName = "StatComponent"))
	class UMonStatComponent*	StatCom;

	bool						IsCC; // 군중 제어

public:
	void SetCrowdControlled(bool _IsCC) { IsCC = _IsCC; }
	bool IsCrowdControlled() { return IsCC; }

protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;

public:
	AMonster();
};
