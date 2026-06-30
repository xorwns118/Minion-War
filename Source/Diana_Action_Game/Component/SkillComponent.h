// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Engine/StreamableManager.h"
#include "SkillComponent.generated.h"

UENUM(BlueprintType)
enum class ESkillSlot : uint8
{
	LeftClick	UMETA(DisplayName = "Left Click"),
	RightClick	UMETA(DisplayName = "Right Click"),
	Skill_1		UMETA(DisplayName = "Skill 1"),
	Skill_2		UMETA(DisplayName = "Skill 2"),
	Skill_3		UMETA(DisplayName = "Skill 3"),
	Skill_4		UMETA(DisplayName = "Skill 4"),

	End UMETA(Hidden)
};

USTRUCT(BlueprintType)
struct FSkillSlotInfo
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	ESkillSlot								SlotType;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TSoftObjectPtr<class USkillDataBase>	SkillData;

};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class DIANA_ACTION_GAME_API USkillComponent : public UActorComponent
{
	GENERATED_BODY()

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Skills", meta = (TitleProperty = "SlotType"))
	TArray<FSkillSlotInfo>					SkillSlots;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TSoftObjectPtr<class USkillDataBase>	CurSkillData;

	UPROPERTY()
	class USkeletalMeshComponent*			SkeletalMeshCom;

private:
	TMap<FPrimaryAssetId, float>	mapSkillCoolTime;

	bool							IsComboWindowOn;
	int32							CurComboIdx;
	
	TSet<TWeakObjectPtr<AActor>>	HitActors;
	TMap<FName, FVector>			PrevSocketLocation;
	
	float							BaseMoveSpeed;

protected:
	virtual void BeginPlay() override;

public:	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

public:
	void UseSkill(int32 _SlotIdx) { TryExecuteSkill(_SlotIdx); }
	bool TryExecuteSkill(int32 _SlotIdx);

	const TSoftObjectPtr<class USkillDataBase>& GetCurSkillData() { return CurSkillData; }

	void Bind(UEnhancedInputComponent* _EIC, class UInputContainer* _InputContainer);

	void AddSkillUseTime(FPrimaryAssetId _Id) { mapSkillCoolTime.Add(_Id, GetWorld()->GetTimeSeconds()); }

	bool IsSkillCoolTime(FPrimaryAssetId _Id, float _MaxCoolTime)
	{
		if (!mapSkillCoolTime.Contains(_Id))
			return false;

		if (GetWorld()->GetTimeSeconds() - mapSkillCoolTime[_Id] < _MaxCoolTime)
			return true;

		return false;
	}

	float GetSkillRemainCoolTime(FPrimaryAssetId _Id, float _MaxCoolTime)
	{
		if (!mapSkillCoolTime.Contains(_Id))
			return 0.f;

		return mapSkillCoolTime[_Id] + _MaxCoolTime - GetWorld()->GetTimeSeconds();
	}


private:
	bool SkillLoaded = false;
	TSharedPtr<FStreamableHandle> AsyncLoadHandle;

	void LoadSkill();
	void OnSkillLoaded();

public:
	void SkillStart();
	void SkillEnd();

	void OpenComboWindow();
	void CloseComboWindow();

	void HitCheckStart();
	void HitCheck();
	void HitCheckEnd();

	void HitTraceBySocketName(FName _SocketName);

	void SpawnProjectile();

public:
	USkillComponent();

};
