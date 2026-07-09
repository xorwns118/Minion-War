// Fill out your copyright notice in the Description page of Project Settings.


#include "StatComponent.h"

UStatComponent::UStatComponent()
{
	PrimaryComponentTick.bCanEverTick = false;

}

void UStatComponent::BeginPlay()
{
	Super::BeginPlay();

}

void UStatComponent::PostEditChangeProperty(FPropertyChangedEvent& _Event)
{
	Super::PostEditChangeProperty(_Event);

	InitStat();
}

void UStatComponent::InitStat()
{
	float Value = GetStat("MaxHP");
	if (Value)
		AddStat(TEXT("CurHP"), Value);

	Value = GetStat("MaxMP");
	if (Value)
		AddStat(TEXT("CurMP"), Value);

	AddStat(TEXT("Shield"), 0.f);

	Modify();
}

float UStatComponent::ApplyDamage(float _AmountDamage)
{
	float ApplyDamage = _AmountDamage;
	float ShieldValue = GetStat(TEXT("Shield"));
	float CurHPValue = GetStat(TEXT("CurHP"));
	float Def = GetStat(TEXT("Def"));

	// 방어력 적용 데미지
	ApplyDamage *= (100 / (100 + Def));

	if (ShieldValue > 0.f)
	{
		if (ShieldValue >= ApplyDamage)
		{
			ShieldValue -= ApplyDamage;
			ApplyDamage = 0.f;
		}
		else
		{
			ShieldValue = 0.f;
			ApplyDamage -= ShieldValue;
		}

		AddStat(TEXT("Shield"), ShieldValue);
	}

	if (ApplyDamage > 0.f)
	{
		if (CurHPValue > ApplyDamage)
			CurHPValue -= ApplyDamage;
		else
			CurHPValue = 0.f;

		AddStat(TEXT("CurHP"), CurHPValue);
	}

	return ApplyDamage;
}

void UStatComponent::InitStatFromStruct(UScriptStruct* _InStruct, const void* _StructPtr)
{
	if (_InStruct == nullptr || _StructPtr == nullptr)
		return;

	for (TFieldIterator<FProperty> Iter(_InStruct); Iter; ++Iter)
	{
		FProperty* Property = *Iter;

		FName StatName = Property->GetFName();

		FFloatProperty* FloatProperty = CastField<FFloatProperty>(Property);

		if (FloatProperty)
		{
			float Value = FloatProperty->GetPropertyValue_InContainer(_StructPtr);

			AddStat(StatName, Value);
		}
	}
}