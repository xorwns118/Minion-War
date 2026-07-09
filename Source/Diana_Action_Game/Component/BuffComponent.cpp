// Fill out your copyright notice in the Description page of Project Settings.


#include "BuffComponent.h"

#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"

#include "StatComponent.h"

#include "../GlobalEnum.h"

UBuffComponent::UBuffComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

}

void UBuffComponent::BeginPlay()
{
	Super::BeginPlay();
	
}

void UBuffComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

}

void UBuffComponent::AddBuff(FName _BuffName, EBuffType _Type, float _BuffDuration, float _Value, class UNiagaraSystem* _Effect)
{
	FBuffInfo Info;

	Info.Type = _Type;
	Info.BuffDuration = _BuffDuration;

	if (_Effect)
	{
		FVector SpawnLocation = FVector::ZeroVector;
		SpawnLocation.Z -= 95.f; // Capsule Half Size, Test

		Info.EffectComponent = UNiagaraFunctionLibrary::SpawnSystemAttached(
			_Effect,
			GetOwner()->GetRootComponent(),
			NAME_None,
			SpawnLocation,
			FRotator::ZeroRotator,
			EAttachLocation::SnapToTarget,
			false
		);
	}

	BuffInfo.Add(_BuffName, Info);

	FTimerDelegate TimerDelegate;
	TimerDelegate.BindUObject(this, &UBuffComponent::RemoveBuff, _BuffName);

	GetWorld()->GetTimerManager().SetTimer(
		Info.TimerHandle,
		TimerDelegate,
		Info.BuffDuration,
		false
	);

	UStatComponent* StatCom = GetOwner()->GetComponentByClass<UStatComponent>();
	if (StatCom == nullptr)
		return;

	switch (Info.Type)
	{
	case EBuffType::None:
		break;
	case EBuffType::AtkModifier:
		break;
	case EBuffType::Shield:
	{
		float ApplyValue = StatCom->GetStat(TEXT("Shield"));
		ApplyValue += _Value;

		StatCom->AddStat(TEXT("Shield"), ApplyValue);
	}
		break;
	case EBuffType::PeriodicDamage:
		break;
	case EBuffType::PeriodicHeal:
		break;
	case EBuffType::CrowdControl:
		break;
	case EBuffType::StateFlag:
		break;
	default:
		break;
	}
}

void UBuffComponent::RemoveBuff(FName _BuffName)
{
	FBuffInfo* Info = BuffInfo.Find(_BuffName);
	if (Info == nullptr)
		return;

	GetWorld()->GetTimerManager().ClearTimer(Info->TimerHandle);

	if (Info->EffectComponent)
	{
		Info->EffectComponent->Deactivate();
		Info->EffectComponent = nullptr;
	}

	BuffInfo.Remove(_BuffName);

	UStatComponent* StatCom = GetOwner()->GetComponentByClass<UStatComponent>();
	if (StatCom == nullptr)
		return;

	switch (Info->Type)
	{
	case EBuffType::None:
		break;
	case EBuffType::AtkModifier:
		break;
	case EBuffType::Shield:
		StatCom->AddStat(TEXT("Shield"), 0.f);
		break;
	case EBuffType::PeriodicDamage:
		break;
	case EBuffType::PeriodicHeal:
		break;
	case EBuffType::CrowdControl:
		break;
	case EBuffType::StateFlag:
		break;
	default:
		break;
	}
}
