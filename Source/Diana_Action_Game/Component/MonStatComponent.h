// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "StatComponent.h"
#include "MonStatComponent.generated.h"

UCLASS()
class DIANA_ACTION_GAME_API UMonStatComponent : public UStatComponent
{
	GENERATED_BODY()
	
protected:
	UPROPERTY(EditAnywhere, Category = "Stat", meta = (RequiredAssetDataTags = "RowStructure=/Script/Diana_Action_Game.MonsterStatData"))
	UDataTable* Table;

public:
	virtual void BeginPlay() override;
	virtual void InitStat() override;

public:
	UMonStatComponent();
};
