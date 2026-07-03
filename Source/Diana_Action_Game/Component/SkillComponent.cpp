// Fill out your copyright notice in the Description page of Project Settings.

#include "SkillComponent.h"

#include "Engine/AssetManager.h"

#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"

#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"

#include "NiagaraFunctionLibrary.h"
#include "GenericTeamAgentInterface.h"

#include "Components/SkeletalMeshComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"

#include "Engine/OverlapResult.h"

#include "../Container/InputContainer.h"
#include "../Projectile/Projectile.h"
#include "../Data/SkillDataBase.h"
#include "../Data/SkillData_Projectile.h"
#include "../Data/SkillData_AoEAttack.h"

#include "../Actor/Monster/Monster.h"

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
	{
		BaseMoveSpeed = Character->GetCharacterMovement()->MaxWalkSpeed;
		BaseYawRotateSpeed = Character->GetCharacterMovement()->RotationRate.Yaw;
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Owner does not character!! please check class"));
	}
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
		return false;

	if (!IsValid(SkillSlots[_SlotIdx].SkillData.Get()))
		return false;

	APawn* OwnerPawn = Cast<APawn>(GetOwner());
	if (OwnerPawn == nullptr)
		return false;

	if (SkillSlots[_SlotIdx].SkillData->CanUseSkill(OwnerPawn, this))
	{
		if (CurSkillData == nullptr)
		{
			if (SkillSlots[_SlotIdx].SkillData->CanUseSkill(Cast<APawn>(GetOwner()), this))
			{
				IsComboWindowOn = false;
				CurComboIdx = 0;

				CurSkillData = SkillSlots[_SlotIdx].SkillData;

				CurSkillData->OnExecuteSkill(Cast<APawn>(GetOwner()), this);

				return true;
			}
			else
			{
				return false;
			}
		}
		else if (SkillSlots[_SlotIdx].SkillData == CurSkillData)
		{
			if (CurSkillData->CanCombo && IsComboWindowOn)
			{
				++CurComboIdx;

				FName NextSection = *FString::Printf(TEXT("Attack%d"), CurComboIdx);

				if (SkeletalMeshCom == nullptr)
				{
					UE_LOG(LogTemp, Error, TEXT("Invalid Skeletal Mesh"));
					return false;
				}

				if (SkeletalMeshCom->GetAnimInstance() == nullptr)
				{
					UE_LOG(LogTemp, Error, TEXT("Invalid Anim Instance"));
					return false;
				}

				if (SkeletalMeshCom->GetAnimInstance()->GetCurrentActiveMontage() == nullptr)
				{
					UE_LOG(LogTemp, Error, TEXT("Invalid Current Active Montage : %s"), *CurSkillData->Montage->GetName());
					return false;
				}

				if (SkeletalMeshCom->GetAnimInstance()->GetCurrentActiveMontage()->GetSectionIndex(NextSection) != INDEX_NONE)
				{
					SkeletalMeshCom->GetAnimInstance()->Montage_JumpToSection(NextSection);
					IsComboWindowOn = false;

					return true;
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

	if (CurSkillData->StartEffect != nullptr)
	{
		FVector SpawnLocation = FVector();
		FRotator SpawnRotation = FRotator();

		if (!SkeletalMeshCom->DoesSocketExist(CurSkillData->StartSocketName)) // Socket 없을 시 캐릭터의 위치에 이펙트 소환
		{
			SpawnLocation = GetOwner()->GetActorLocation();
			SpawnLocation.Z = 0.f; // 바닥 유지
		}
		else
		{
			SpawnLocation = SkeletalMeshCom->GetSocketLocation(CurSkillData->StartSocketName);
			SpawnRotation = SkeletalMeshCom->GetSocketRotation(CurSkillData->StartSocketName);
		}

		UNiagaraFunctionLibrary::SpawnSystemAtLocation(
			GetWorld(),
			CurSkillData->StartEffect,
			SpawnLocation,
			FRotator()
		);
	}
}

void USkillComponent::SkillEnd()
{
	if (CurSkillData == nullptr)
		return;

	APawn* OwnerPawn = Cast<APawn>(GetOwner());

	if (OwnerPawn)
		CurSkillData->OnEndSkill(OwnerPawn, this);

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

	switch (CurSkillData->HitZone)
	{
	case EWeaponHitZone::MainWeapon:
		PrevSocketLocation.Add(CurSkillData->MainHitSocketName, SkeletalMeshCom->GetSocketLocation(CurSkillData->MainHitSocketName));
	break;
	case EWeaponHitZone::AssistWeapon:
		PrevSocketLocation.Add(CurSkillData->AssistHitSocketName, SkeletalMeshCom->GetSocketLocation(CurSkillData->AssistHitSocketName));
	break;
	case EWeaponHitZone::DualWeapon:
		PrevSocketLocation.Add(CurSkillData->MainHitSocketName, SkeletalMeshCom->GetSocketLocation(CurSkillData->MainHitSocketName));
		PrevSocketLocation.Add(CurSkillData->AssistHitSocketName, SkeletalMeshCom->GetSocketLocation(CurSkillData->AssistHitSocketName));
	break;
	default:
		break;
	}
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

	switch (CurSkillData->HitZone)
	{
	case EWeaponHitZone::MainWeapon:
		HitTraceBySocketName(CurSkillData->MainHitSocketName);
		break;
	case EWeaponHitZone::AssistWeapon:
		HitTraceBySocketName(CurSkillData->AssistHitSocketName);
		break;
	case EWeaponHitZone::DualWeapon:
		HitTraceBySocketName(CurSkillData->MainHitSocketName);
		HitTraceBySocketName(CurSkillData->AssistHitSocketName);
		break;
	default:
		break;
	}
}

void USkillComponent::HitCheckEnd()
{
	HitActors.Empty();
	PrevSocketLocation.Empty();
}

void USkillComponent::HitTraceBySocketName(FName _SocketName)
{
	if (GetOwner() == nullptr)
		return;

	APawn* SkillUser = Cast<APawn>(GetOwner());
	if (SkillUser == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("Skill User does not exist."));
		return;
	}

	if (!PrevSocketLocation.Contains(_SocketName))
	{
		UE_LOG(LogTemp, Error, TEXT("Map does not contain socket(%s)."), *_SocketName.ToString());
		return;
	}

	if (!IsValidSocket(SkeletalMeshCom, _SocketName))
		return;

	FVector CurSocketLocation = SkeletalMeshCom->GetSocketLocation(_SocketName);
	FQuat BoxQuat = SkeletalMeshCom->GetSocketQuaternion(_SocketName);
	TArray<FHitResult> HitResults;

	FCollisionQueryParams Params;
	Params.AddIgnoredActor(GetOwner());

	FCollisionObjectQueryParams ObjectParams;
	ObjectParams.AddObjectTypesToQuery(ECC_Pawn);

	FCollisionShape Shape = FCollisionShape::MakeBox(CurSkillData->BoxSize);

	bool bIsHit = GetWorld()->SweepMultiByObjectType(
		HitResults,
		PrevSocketLocation[_SocketName],
		CurSocketLocation,
		BoxQuat,
		ObjectParams,
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
				if (Cast<APawn>(HitActor) == nullptr)
					continue;

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

	PrevSocketLocation[_SocketName] = CurSocketLocation;
}

void USkillComponent::SpawnProjectile()
{
	USkillData_Projectile* ProjData = Cast<USkillData_Projectile>(CurSkillData.Get());
	if (ProjData == nullptr)
		return;

	TSubclassOf<AProjectile> Proj = ProjData->ProjectileClass;
	if (Proj == nullptr)
		return;

	if (!IsValidSocket(SkeletalMeshCom, ProjData->SpawnSocketName))
		return;
	
	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
	SpawnParams.Owner = GetOwner();
	SpawnParams.Instigator = Cast<APawn>(GetOwner());

	FVector SpawnSocketLocation = SkeletalMeshCom->GetSocketLocation(ProjData->SpawnSocketName);
	FRotator SpawnSocketRotation = FRotator(0, GetOwner()->GetActorRotation().Yaw, 0);

	AProjectile* SpawnedProjectile = GetWorld()->SpawnActor<AProjectile>(
		Proj,
		SpawnSocketLocation,
		SpawnSocketRotation,
		SpawnParams
	);

	SpawnedProjectile->SetData(ProjData);
}

void USkillComponent::DianaUltimate_Start()
{
	if (CurSkillData == nullptr)
		return;

	USkillData_AoEAttack* AoEData = Cast<USkillData_AoEAttack>(CurSkillData.Get());
	if (AoEData == nullptr)
		return;

	TArray<FOverlapResult> OverlapResults;
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(GetOwner());

	FCollisionObjectQueryParams ObjectParams;
	ObjectParams.AddObjectTypesToQuery(ECC_Pawn);

	FVector Center = Cast<APawn>(GetOwner())->GetActorLocation() + AoEData->LocationOffset;
	// 캐릭터의 발 아래 위치로
	Center.Z -= Cast<ACharacter>(GetOwner())->GetCapsuleComponent()->GetScaledCapsuleHalfHeight();

	FCollisionShape Shape = FCollisionShape::MakeSphere(AoEData->HitRadius);

	bool bFind = GetWorld()->OverlapMultiByObjectType(
		OverlapResults,
		Center,
		FQuat::Identity,
		ObjectParams,
		Shape,
		Params
	);

	TArray<AActor*> Targets;

	if (bFind)
	{
		for (const FOverlapResult& Result : OverlapResults)
		{
			AActor* HitActor = Result.GetActor();
			if (HitActor && !Targets.Contains(HitActor))
				Targets.Add(HitActor);
		}
	}

	if (AoEData->IsPullToCenter)
	{
		for (const auto& Target : Targets)
		{
			ACharacter* Char = Cast<ACharacter>(Target);
			if (Char == nullptr)
				continue;

			FVector TargetLocation = Target->GetActorLocation();

			// 평면 벡터, 정규화
			FVector DirToCenter = Center - TargetLocation;
			DirToCenter.Z = 0.0f;
			
			// 겹쳐있는 적 무시
			float CurrentDistance = DirToCenter.Size();
			if (CurrentDistance <= 0.f)
				continue;

			DirToCenter.Normalize();

			// 멀리 있을수록 가하는 힘 증가
			float DynamicForce = AoEData->Force * FMath::Clamp(CurrentDistance / AoEData->HitRadius, 0.0f, 1.0f);

			// 가할 힘 + 에어본 수치
			FVector LaunchVelocity = DirToCenter * DynamicForce;
			LaunchVelocity.Z = AoEData->AirborneValue;

			// 상대의 속도를 완전히 없애 강제로 끌어옴
			Char->GetCharacterMovement()->StopMovementImmediately();

			Char->LaunchCharacter(LaunchVelocity, true, true);
		}
	}
	else if (AoEData->IsPushFromCenter)
	{
		UE_LOG(LogTemp, Warning, TEXT("Push From Center"));
	}

	// DrawDebugSphere(GetWorld(), Center, AoEData->HitRadius, 10, FColor::Red, false, 2.5f);
}

void USkillComponent::DianaUltimate_End()
{
	if (CurSkillData == nullptr)
		return;

	USkillData_AoEAttack* AoEData = Cast<USkillData_AoEAttack>(CurSkillData.Get());
	if (AoEData == nullptr)
		return;

	FVector Center = GetOwner()->GetActorLocation() + AoEData->LocationOffset;

	// 중심점 기준 정해진 반경 안의 Target 모음
	TArray<FOverlapResult> OverlapResults;
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(GetOwner());

	TArray<AActor*> Targets;

	FCollisionShape Shape = FCollisionShape::MakeSphere(AoEData->HitRadius);

	bool bFind = GetWorld()->OverlapMultiByChannel(
		OverlapResults,
		Center,
		FQuat::Identity,
		ECollisionChannel::ECC_Pawn,
		Shape,
		Params
	);

	APawn* OwnerPawn = Cast<APawn>(GetOwner());

	// 찾은 대상들에게 이펙트 남김, (각각의 대상에 대한 처리가 필요없다면 제거 가능)
	if (bFind)
	{
		for (const FOverlapResult& Result : OverlapResults)
		{
			AActor* HitActor = Result.GetActor();
			if (HitActor && !Targets.Contains(HitActor))
			{
				Targets.Add(HitActor);

				if (AoEData->HitEffect)
				{
					UNiagaraFunctionLibrary::SpawnSystemAtLocation(
						GetWorld(),
						CurSkillData->HitEffect,
						HitActor->GetActorLocation(),
						FRotator()
					);
				}
			}
		}

		TArray<AActor*> IgnoreActors;
		IgnoreActors.Add(GetOwner());
		// 같은 팀도 무시 할 수 있도록 추가 로직 필요

		UGameplayStatics::ApplyRadialDamage(
			GetWorld(),
			AoEData->Damage,
			Center,
			AoEData->HitRadius,
			UDamageType::StaticClass(),
			IgnoreActors,
			GetOwner(),
			OwnerPawn->GetController(),
			true
		);
	}

	// DrawDebugSphere(GetWorld(), Center, AoEData->HitRadius, 10, FColor::Red, false, 2.5);
}

bool USkillComponent::IsValidSocket(USkeletalMeshComponent* _SkeletalMeshCom, FName _Name)
{
	if (_SkeletalMeshCom == nullptr || _Name.IsNone())
		return false;

	if (_SkeletalMeshCom->DoesSocketExist(_Name))
		return true;
	
	UE_LOG(LogTemp, Error, TEXT("Socket(%s) does not exist."), *_Name.ToString());
	return false;
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
