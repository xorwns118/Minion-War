// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MyHUD.generated.h"

UCLASS()
class DIANA_ACTION_GAME_API UMyHUD : public UUserWidget
{
	GENERATED_BODY()

protected:
	UPROPERTY(meta = (BindWidget))
	class UImage* PortraitImage;

	UPROPERTY(meta = (BindWidget))
	class UImage* SkillImage_1;

	UPROPERTY(meta = (BindWidget))
	class UImage* SkillImage_2;

	UPROPERTY(meta = (BindWidget))
	class UImage* SkillImage_3;

	UPROPERTY(meta = (BindWidget))
	class UImage* SkillImage_4;

	UPROPERTY(meta = (BindWidget))
	class UImage* CooldownOverlay_1;

	UPROPERTY(meta = (BindWidget))
	class UImage* CooldownOverlay_2;

	UPROPERTY(meta = (BindWidget))
	class UImage* CooldownOverlay_3;

	UPROPERTY(meta = (BindWidget))
	class UImage* CooldownOverlay_4;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* Cooldown_1;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* Cooldown_2;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* Cooldown_3;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* Cooldown_4;

	UPROPERTY(meta = (BindWidget))
	class UProgressBar* HPBar;

	UPROPERTY(meta = (BindWidget))
	class UProgressBar* MPBar;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* HPText;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* MPText;

	UPROPERTY()
	TArray<class UImage*> CooldownOverlays;

	UPROPERTY()
	TArray<class UTextBlock*> SkillCoolTimes;

	UPROPERTY()
	TArray<class UMaterialInstanceDynamic*> CooldownOverlayMIDs;

public:
	void UpdateHPBar(float _CurHP, float _MaxHP);
	void UpdateMPBar(float _CurMP, float _MaxMP);
	void UpdateCooldown(int32 _SlotIdx, float _RemainTime, float _BaseCooldown);
	void SetCharacterData(class UCharacterDefinition* _Data);

protected:
	virtual bool Initialize() override;
	virtual void NativeTick(const FGeometry& _MyGeometry, float _InDeltaTime) override;
};
