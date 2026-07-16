// Fill out your copyright notice in the Description page of Project Settings.

#include "MyHUD.h"

#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"
#include "Components/Image.h"

#include "Materials/MaterialInstanceDynamic.h"

#include "../Data/CharacterData/CharacterDefinition.h"

bool UMyHUD::Initialize()
{
	if (!Super::Initialize())
		return false;

	SkillCoolTimes = { Cooldown_1, Cooldown_2, Cooldown_3, Cooldown_4 };
	CooldownOverlays = { CooldownOverlay_1, CooldownOverlay_2, CooldownOverlay_3, CooldownOverlay_4 };

	CooldownOverlayMIDs.Empty();

	for (int32 i = 0; i < CooldownOverlays.Num(); ++i)
	{
		if (CooldownOverlays[i] == nullptr)
		{
			CooldownOverlayMIDs.Add(nullptr);
			continue;
		}

		UMaterialInstanceDynamic* MID = CooldownOverlays[i]->GetDynamicMaterial();
		CooldownOverlayMIDs.Add(MID);

		if (MID)
			MID->SetScalarParameterValue("Percent", 0.f);

		CooldownOverlays[i]->SetVisibility(ESlateVisibility::Hidden);
	}

	for (UTextBlock* CooldownText : SkillCoolTimes)
	{
		if (CooldownText)
			CooldownText->SetVisibility(ESlateVisibility::Hidden);
	}

	return true;
}

void UMyHUD::NativeTick(const FGeometry& _MyGeometry, float _InDeltaTime)
{
	Super::NativeTick(_MyGeometry, _InDeltaTime);
}

void UMyHUD::UpdateHPBar(float _CurHP, float _MaxHP)
{
	if (_MaxHP <= 0.f)
		return;

	float Per = 0.f;

	if (_MaxHP < _CurHP)
		Per = 1.f;
	else if (_CurHP > 0.f && _MaxHP >= _CurHP)
		Per = _CurHP / _MaxHP;
	else if (_CurHP <= 0.f)
		Per = 0.f;

	FText InText = FText::Format(INVTEXT("{0} / {1}"), _CurHP, _MaxHP);

	HPText->SetText(InText);
	HPBar->SetPercent(Per);
}

void UMyHUD::UpdateMPBar(float _CurMP, float _MaxMP)
{
	if (_MaxMP <= 0.f)
		return;

	float Per = 0.f;

	if (_MaxMP < _CurMP)
		Per = 1.f;
	else if (_CurMP > 0.f && _MaxMP >= _CurMP)
		Per = _CurMP / _MaxMP;
	else if (_CurMP <= 0.f)
		Per = 0.f;

	FText InText = FText::Format(INVTEXT("{0} / {1}"), _CurMP, _MaxMP);

	MPText->SetText(InText);
	MPBar->SetPercent(Per);
}

void UMyHUD::UpdateCooldown(int32 _SlotIdx, float _RemainTime, float _BaseCooldown)
{
	if (!CooldownOverlayMIDs.IsValidIndex(_SlotIdx))
		return;

	float RemainTime = FMath::Clamp(_RemainTime, 0.f, _BaseCooldown);
	float Percent = _BaseCooldown > 0.f ? RemainTime / _BaseCooldown : 0.f;

	if (UMaterialInstanceDynamic* MID = CooldownOverlayMIDs[_SlotIdx])
		MID->SetScalarParameterValue("Percent", Percent);

	const bool bCoolingDown = RemainTime > 0.f;

	if (CooldownOverlays.IsValidIndex(_SlotIdx) && CooldownOverlays[_SlotIdx])
	{
		if (bCoolingDown)
			CooldownOverlays[_SlotIdx]->SetVisibility(ESlateVisibility::HitTestInvisible);
		else
			CooldownOverlays[_SlotIdx]->SetVisibility(ESlateVisibility::Hidden);
	}

	if (SkillCoolTimes.IsValidIndex(_SlotIdx) && SkillCoolTimes[_SlotIdx])
	{
		if (bCoolingDown)
		{
			SkillCoolTimes[_SlotIdx]->SetVisibility(ESlateVisibility::HitTestInvisible);
			SkillCoolTimes[_SlotIdx]->SetText(FText::AsNumber(FMath::CeilToInt(RemainTime)));
		}
		else
		{
			SkillCoolTimes[_SlotIdx]->SetVisibility(ESlateVisibility::Hidden);
		}
	}
}

void UMyHUD::SetCharacterData(UCharacterDefinition* _Data)
{
	if (_Data)
	{
		if (_Data->PortaitCircleImage)
		{
			PortraitImage->SetBrushFromTexture(_Data->PortaitCircleImage);
			PortraitImage->SetColorAndOpacity(FLinearColor(1.f, 1.f, 1.f, 1.f));
		}

		// Skill Data 에 스킬 담당 이미지까지 등록 후 해당 값 가져오는게 더 안전할 것 같음
		if (_Data->SkillImage_1)
		{
			SkillImage_1->SetBrushFromTexture(_Data->SkillImage_1);
			SkillImage_1->SetColorAndOpacity(FLinearColor(1.f, 1.f, 1.f, 1.f));
		}

		if (_Data->SkillImage_2)
		{
			SkillImage_2->SetBrushFromTexture(_Data->SkillImage_2);
			SkillImage_2->SetColorAndOpacity(FLinearColor(1.f, 1.f, 1.f, 1.f));
		}

		if (_Data->SkillImage_3)
		{
			SkillImage_3->SetBrushFromTexture(_Data->SkillImage_3);
			SkillImage_3->SetColorAndOpacity(FLinearColor(1.f, 1.f, 1.f, 1.f));
		}

		if (_Data->SkillImage_4)
		{
			SkillImage_4->SetBrushFromTexture(_Data->SkillImage_4);
			SkillImage_4->SetColorAndOpacity(FLinearColor(1.f, 1.f, 1.f, 1.f));
		}
	}

}
