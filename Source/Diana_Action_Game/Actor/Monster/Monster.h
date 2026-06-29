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


protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;

public:
	AMonster();
};
