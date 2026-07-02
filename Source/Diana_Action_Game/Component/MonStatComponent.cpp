// Fill out your copyright notice in the Description page of Project Settings.


#include "MonStatComponent.h"

#include "../GlobalData.h"

UMonStatComponent::UMonStatComponent()
{
}

void UMonStatComponent::BeginPlay()
{
	Super::BeginPlay();

	InitStat();
}

void UMonStatComponent::InitStat()
{
	if (Table == nullptr || RowName.IsNone())
		return;

	FMonsterStatData* MonData = Table->FindRow<FMonsterStatData>(RowName, TEXT("MonsterStat"));

	InitStatFromStruct(FMonsterStatData::StaticStruct(), MonData);

	Super::InitStat();
}
