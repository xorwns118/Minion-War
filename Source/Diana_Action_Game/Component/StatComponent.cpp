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

	Modify();
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