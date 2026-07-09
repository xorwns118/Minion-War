// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "../GlobalEnum.h"
#include "BuffComponent.generated.h"

USTRUCT(BlueprintType)
struct FBuffInfo
{
	GENERATED_BODY()

	UPROPERTY()
	EBuffType					Type;

	UPROPERTY()
	class UNiagaraComponent*	EffectComponent;

	UPROPERTY()
	float						BuffDuration;

	FTimerHandle				TimerHandle;
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class DIANA_ACTION_GAME_API UBuffComponent : public UActorComponent
{
	GENERATED_BODY()

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Buff|Info")
	TMap<FName, FBuffInfo> BuffInfo;

public:
	void AddBuff(FName _BuffName, EBuffType _Type, float _BuffDuration, float _Value, class UNiagaraSystem* _Effect = nullptr);

protected:
	void RemoveBuff(FName _BuffName);

protected:
	virtual void BeginPlay() override;

public:
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

public:
	UBuffComponent();
};
