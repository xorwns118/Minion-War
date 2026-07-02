// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerStatComponent.h"

#include "../GlobalData.h"

UPlayerStatComponent::UPlayerStatComponent()
{
}

void UPlayerStatComponent::BeginPlay()
{
	Super::BeginPlay();

	InitStat();
}

void UPlayerStatComponent::InitStat()
{
	if (Table == nullptr || RowName.IsNone())
		return;

	FPlayerStatData* PlayerStat = Table->FindRow<FPlayerStatData>(RowName, TEXT("PlayerStat"));

	InitStatFromStruct(FPlayerStatData::StaticStruct(), PlayerStat);

	AddStat(TEXT("CurUP"), 0.f);

	Super::InitStat();
}