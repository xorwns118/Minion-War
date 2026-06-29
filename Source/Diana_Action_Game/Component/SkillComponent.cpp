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

#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"

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

	SkeletalMeshCom = GetOwner()->GetComponentByClass<USkeletalMeshComponent>();

	ACharacter* Character = Cast<ACharacter>(GetOwner());
	if (Character != nullptr)
		BaseMoveSpeed = Character->GetCharacterMovement()->MaxWalkSpeed;
	else
		UE_LOG(LogTemp, Error, TEXT("Owner does not character!! please check class"));
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
			if (SkillSlots[_SlotIdx].SkillData->CanUseSkill(Cast<APawn>(GetOwner()), this))
			{
				// 콤보 초기화
				IsComboWindowOn = false;
				CurComboIdx = 0;

				// 현재 사용중인 스킬 등록
				CurSkillData = SkillSlots[_SlotIdx].SkillData;

				// 스킬 실행
				CurSkillData->OnExecuteSkill(Cast<APawn>(GetOwner()), this);

				return true;
			}
			else
				return false;
		}
		else if (SkillSlots[_SlotIdx].SkillData == CurSkillData)
		{
			if (CurSkillData->CanCombo && IsComboWindowOn)
			{
				++CurComboIdx;

				FName NextSection = *FString::Printf(TEXT("Attack%d"), CurComboIdx);

				if (SkeletalMeshCom && SkeletalMeshCom->GetAnimInstance() && SkeletalMeshCom->GetAnimInstance()->GetCurrentActiveMontage())
				{
					if (SkeletalMeshCom->GetAnimInstance()->GetCurrentActiveMontage()->GetSectionIndex(NextSection) != INDEX_NONE)
					{
						SkeletalMeshCom->GetAnimInstance()->Montage_JumpToSection(NextSection);
						IsComboWindowOn = false;

						return true;
					}
					else
					{
						UE_LOG(LogTemp, Error, TEXT("Invalid montage section : %s"), *NextSection.ToString());
						return false;
					}
				}
			}
		}
	}

	return false;
}

void USkillComponent::SkillStart()
{
	if (CurSkillData == nullptr)
		return;

	ACharacter* Character = Cast<ACharacter>(GetOwner());

	if (Character == nullptr)
		return;

	Character->GetCharacterMovement()->MaxWalkSpeed *= CurSkillData->MoveSpeedScale;

	/*if (CurSkillData->TrailEffect != nullptr)
	{
		FVector CurSocketLocation = SkeletalMeshCom->GetSocketLocation(CurSkillData->SocketName);
		FRotator BoxRotation = SkeletalMeshCom->GetSocketRotation(CurSkillData->SocketName);

		UNiagaraFunctionLibrary::SpawnSystemAtLocation(
			GetWorld(),
			CurSkillData->TrailEffect,
			CurSocketLocation,
			BoxRotation
		);
	}*/
}

void USkillComponent::SkillEnd()
{
	if (CurSkillData == nullptr)
		return;

	ACharacter* Character = Cast<ACharacter>(GetOwner());

	if (Character == nullptr)
		return;

	Character->GetCharacterMovement()->MaxWalkSpeed = BaseMoveSpeed;

	CurSkillData = nullptr;
}

void USkillComponent::OpenComboWindow()
{
	IsComboWindowOn = true;
}

void USkillComponent::CloseComboWindow()
{
	IsComboWindowOn = false;
}

void USkillComponent::HitCheckStart()
{
	if (CurSkillData == nullptr)
		return;

	if (SkeletalMeshCom == nullptr)
		return;

	if (CurSkillData->SkillType == ESkillType::BUFF)
		return;

	PrevSocketLocation = SkeletalMeshCom->GetSocketLocation(CurSkillData->SocketName);
}

void USkillComponent::HitCheck()
{
	if (CurSkillData == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("Skill Data does not exist."));
		return;
	}

	if (SkeletalMeshCom == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("Skeletal Mesh Component does not exist."));
		return;
	}

	APawn* SkillUser = Cast<APawn>(GetOwner());
	if (SkillUser == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("Skill User does not exist."));
		return;
	}

	if (!SkeletalMeshCom->DoesSocketExist(CurSkillData->SocketName))
	{
		FString SocketName = CurSkillData->SocketName.ToString();
		UE_LOG(LogTemp, Error, TEXT("Socket(%s) does not exist."), *SocketName);
		return;
	}

	FVector CurSocketLocation = SkeletalMeshCom->GetSocketLocation(CurSkillData->SocketName);
	FQuat BoxQuat = SkeletalMeshCom->GetSocketQuaternion(CurSkillData->SocketName);
	TArray<FHitResult> HitResults;

	FCollisionQueryParams Params;
	Params.AddIgnoredActor(GetOwner());

	FCollisionShape Shape = FCollisionShape::MakeBox(CurSkillData->BoxSize);

	bool bIsHit = GetWorld()->SweepMultiByChannel(
		HitResults,
		PrevSocketLocation,
		CurSocketLocation,
		BoxQuat,
		ECC_WorldDynamic,
		Shape,
		Params
	);

	if (bIsHit)
	{
		for (const FHitResult& Result : HitResults)
		{
			AActor* HitActor = Result.GetActor();

			if (HitActor && !HitActors.Contains(HitActor))
			{
				HitActors.Add(HitActor);

				// 같은 팀인지 확인하는 과정 추가 필요 - 테스트 후 구현
				UGameplayStatics::ApplyDamage(
					Result.GetActor(),
					CurSkillData->Damage,
					SkillUser->GetController(),
					SkillUser,
					UDamageType::StaticClass()
				);

				if (CurSkillData->HitEffect)
				{
					UNiagaraFunctionLibrary::SpawnSystemAtLocation(
						GetWorld(),
						CurSkillData->HitEffect,
						Result.ImpactPoint,
						Result.ImpactNormal.Rotation()
					);
				}
			}
		}
	}

	DrawDebugBox(GetWorld(), CurSocketLocation, CurSkillData->BoxSize, FColor::Red);

	PrevSocketLocation = CurSocketLocation;
}

void USkillComponent::HitCheckEnd()
{
	HitActors.Empty();
}

void USkillComponent::SpawnProjectile()
{
}



//================
// 비동기 에셋 로딩
//================

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
