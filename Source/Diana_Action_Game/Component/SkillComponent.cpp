// Fill out your copyright notice in the Description page of Project Settings.

#include "SkillComponent.h"

#include "Engine/AssetManager.h"

#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"

#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"

#include "NiagaraFunctionLibrary.h"
#include "Components/SkeletalMeshComponent.h"
#include "GenericTeamAgentInterface.h"

#include "../Container/InputContainer.h"
#include "../Data/SkillDataBase.h"

USkillComponent::USkillComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

	for (uint8 i = 0; i < (uint8)ESkillSlot::End; ++i)
	{
		SkillSlots.Add(FSkillSlotInfo{ (ESkillSlot)i });
	}
}

void USkillComponent::BeginPlay()
{
	Super::BeginPlay();
	
	LoadSkill();
}

void USkillComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

}

void USkillComponent::Bind(UEnhancedInputComponent* _EIC, UInputContainer* _InputContainer)
{
	if (_EIC == nullptr || _InputContainer == nullptr)
		return;

	UEnum* EnumClass = StaticEnum<ESkillSlot>();
	if (!EnumClass)
		return;

	for (int32 i = 0; i < SkillSlots.Num(); ++i)
	{
		if (!EnumClass->IsValidEnumValue(i))
			continue;

		FString SlotName = EnumClass->GetNameStringByValue(i);
		FString IAName = FString::Printf(TEXT("IA_%s"), *SlotName);

		if (const UInputAction* Action = _InputContainer->FindIAByName(IAName))
		{
			UE_LOG(LogTemp, Warning, TEXT("Bind Skill : %s"), *IAName);
			_EIC->BindAction(Action, ETriggerEvent::Started, this, &USkillComponent::UseSkill, i);
		}
	}
}

bool USkillComponent::TryExecuteSkill(int32 _SlotIdx)
{
	if (!SkillLoaded)
	{
		return false;
	}

	if (!IsValid(SkillSlots[_SlotIdx].SkillData.Get()))
	{
		return false;
	}

	APawn* OwnerPawn = Cast<APawn>(GetOwner());

	if (SkillSlots[_SlotIdx].SkillData->CanUseSkill(OwnerPawn, this))
	{
		if (CurSkillData == nullptr)
		{
			CurSkillData = SkillSlots[_SlotIdx].SkillData;

			CurSkillData->OnExecuteSkill(OwnerPawn, this);
		}
		else
		{
			if (CurSkillData->CanCombo)
			{
				IsComboWindowOn = true;
			}
		}

		return true;
	}

	return false;
}

void USkillComponent::SkillStart()
{
	// 스킬 시작 시
}

void USkillComponent::SkillEnd()
{
	if (CurSkillData == nullptr)
		return;

	CurSkillData = nullptr;
}

void USkillComponent::OpenComboWindow()
{
	UE_LOG(LogTemp, Warning, TEXT("Combo Opened"));
}

void USkillComponent::CloseComboWindow()
{
	UE_LOG(LogTemp, Warning, TEXT("Combo Closed"));
}

void USkillComponent::HitCheckStart()
{
	UE_LOG(LogTemp, Warning, TEXT("Hit Check Start"));
}

void USkillComponent::HitCheck()
{
	UE_LOG(LogTemp, Warning, TEXT("Hit Checking"));
}

void USkillComponent::HitCheckEnd()
{
	UE_LOG(LogTemp, Warning, TEXT("Hit Check Finish"));
}



void USkillComponent::LoadSkill()
{
	TArray<FSoftObjectPath> AssetToLoad;

	for (const FSkillSlotInfo& SlotInfo : SkillSlots)
	{
		if (!SlotInfo.SkillData.IsNull())
		{
			AssetToLoad.Add(SlotInfo.SkillData.ToSoftObjectPath());
			FString Log = SlotInfo.SkillData.ToSoftObjectPath().ToString();
		}
	}

	if (AssetToLoad.Num() > 0)
	{
		FStreamableManager& Streamable = UAssetManager::Get().GetStreamableManager();

		AsyncLoadHandle
			= Streamable.RequestAsyncLoad(AssetToLoad, FStreamableDelegate::CreateUObject(this, &USkillComponent::OnSkillLoaded));
	}
}

void USkillComponent::OnSkillLoaded()
{
	if (!AsyncLoadHandle.IsValid())
	{
		UE_LOG(LogTemp, Error, TEXT("Skill async load handle is invalid"));
		SkillLoaded = false;
		return;
	}

	if (AsyncLoadHandle->WasCanceled())
	{
		UE_LOG(LogTemp, Error, TEXT("Skill async load was canceled"));
		SkillLoaded = false;
		return;
	}

	if (!AsyncLoadHandle->HasLoadCompleted())
	{
		UE_LOG(LogTemp, Error, TEXT("Skill async load callback fired before completion"));
		SkillLoaded = false;
		return;
	}

	bool bAllLoaded = true;

	for (const FSkillSlotInfo& SlotInfo : SkillSlots)
	{
		if (SlotInfo.SkillData.IsNull())
			continue;

		USkillDataBase* LoadedSkill = SlotInfo.SkillData.Get();

		if (!IsValid(LoadedSkill))
		{
			bAllLoaded = false;

			UE_LOG(
				LogTemp,
				Error,
				TEXT("Failed to load skill asset. Slot=%s Path=%s"),
				*StaticEnum<ESkillSlot>()->GetNameStringByValue((int64)SlotInfo.SlotType),
				*SlotInfo.SkillData.ToSoftObjectPath().ToString()
			);
		}
		else
		{
			UE_LOG(
				LogTemp,
				Warning,
				TEXT("Loaded skill asset. Slot=%s Asset=%s"),
				*StaticEnum<ESkillSlot>()->GetNameStringByValue((int64)SlotInfo.SlotType),
				*LoadedSkill->GetPathName()
			);
		}
	}

	SkillLoaded = bAllLoaded;

	UE_LOG(LogTemp, Warning, TEXT("Skill Asset Load Complete. Success=%s"), SkillLoaded ? TEXT("true") : TEXT("false"));
}
