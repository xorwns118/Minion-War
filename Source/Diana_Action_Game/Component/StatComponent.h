// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "StatComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class DIANA_ACTION_GAME_API UStatComponent : public UActorComponent
{
	GENERATED_BODY()

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stat")
	FName RowName;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Stat")
	TMap<FName, float> Stats;

public:
	void AddStat(FName _StatName, float _Amount) { Stats.Add(_StatName, _Amount); }
	float GetStat(FName _StatName)
	{
		if (float* Data = Stats.Find(_StatName))
			return *Data;
		else
			return 0.f;
	}

protected:
	void InitStatFromStruct(UScriptStruct* _InStruct, const void* _StructPtr);

protected:
	virtual void BeginPlay() override;

	virtual void InitStat();

	virtual void PostEditChangeProperty(FPropertyChangedEvent& _Event) override;

public:
	UStatComponent();
};
