// Fill out your copyright notice in the Description page of Project Settings.


#include "MyPlayer.h"

#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/PlayerController.h"

#include "Components/CapsuleComponent.h"
#include "Camera/CameraComponent.h"

#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"

#include "../Container/InputContainer.h"
#include "../Component/SkillComponent.h"
#include "../Component/StatComponent.h"
#include "../Component/BuffComponent.h"
#include "../Component/PlayerStatComponent.h"

#include "../GlobalEnum.h"

AMyPlayer::AMyPlayer()
{
	PrimaryActorTick.bCanEverTick = true;

	GetCapsuleComponent()->SetCapsuleHalfHeight(95.f);

	GetMesh()->SetRelativeRotation(FRotator(0.f, -90.f, 0.f));
	GetMesh()->SetRelativeLocation(FVector(0.f, 0.f, -95.f));

	SpringArmCom = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArmCom->SetupAttachment(GetRootComponent());
	SpringArmCom->TargetArmLength = 250.f;
	SpringArmCom->SetRelativeLocation(FVector(0.f, 0.f, 70.f));
	SpringArmCom->bUsePawnControlRotation = true;

	CameraCom = CreateDefaultSubobject<UCameraComponent>(TEXT("MainCamera"));
	CameraCom->SetupAttachment(SpringArmCom);
	CameraCom->bUsePawnControlRotation = false;

	SkillCom = CreateDefaultSubobject<USkillComponent>(TEXT("SkillComponent"));

	StatCom = CreateDefaultSubobject<UPlayerStatComponent>(TEXT("StatComponent"));

	BuffCom = CreateDefaultSubobject<UBuffComponent>(TEXT("BuffComponent"));

	InputContainer = CreateDefaultSubobject<UInputContainer>(TEXT("InputContainer"));

	GetCharacterMovement()->MaxWalkSpeed = 600.f;
	GetCharacterMovement()->JumpZVelocity = 500.f;

	BaseMoveSpeed = GetCharacterMovement()->MaxWalkSpeed;

	bUseControllerRotationPitch = false;
	bUseControllerRotationRoll = false;
	bUseControllerRotationYaw = false;

	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 300.0f, 0.0f);

	CamInterpSpeed = 12.f;
}

void AMyPlayer::BeginPlay()
{
	Super::BeginPlay();

	SetGenericTeamId((uint8)ETeamType::Player);
}

void AMyPlayer::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (IsCamRecenter)
	{
		if (GetController())
		{
			const FRotator CurRot = GetController()->GetControlRotation();
			const FRotator ActorRot = GetActorRotation();

			// 현재 Control Rot 상하 유지, Actor Yaw 로 카메라 조정
			const FRotator TargetRot = FRotator(CurRot.Pitch, ActorRot.Yaw, 0.f);

			// 끊기듯 회전하지 않게 빠른 속도로 목표로 회전
			const FRotator NewRot = FMath::RInterpTo(
				CurRot,
				TargetRot,
				GetWorld()->GetDeltaSeconds(),
				CamInterpSpeed
			);

			GetController()->SetControlRotation(NewRot);

			if (FMath::Abs(FMath::FindDeltaAngleDegrees(NewRot.Yaw, TargetRot.Yaw)) < 1.f)
				IsCamRecenter = false;
		}
		else
		{
			IsCamRecenter = false;
		}
	}
}

void AMyPlayer::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	InputContainer->Init(GetController());

	if (UEnhancedInputComponent* EIC = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		if (const UInputAction* IA = InputContainer->FindIAByName("IA_Move"))
			EIC->BindAction(IA, ETriggerEvent::Triggered, this, &AMyPlayer::MoveAction);

		if (const UInputAction* IA = InputContainer->FindIAByName("IA_Look"))
			EIC->BindAction(IA, ETriggerEvent::Triggered, this, &AMyPlayer::LookAction);

		if (const UInputAction* IA = InputContainer->FindIAByName("IA_RightClick"))
			EIC->BindAction(IA, ETriggerEvent::Started, this, &AMyPlayer::AimingAction);

		SkillCom->Bind(EIC, InputContainer);
	}
}

float AMyPlayer::TakeDamage(float _DamageAmount, FDamageEvent const& _DamageEvent, AController* _EventInstigator, AActor* _DamageCauser)
{
	Super::TakeDamage(_DamageAmount, _DamageEvent, _EventInstigator, _DamageCauser);

	float Damage = _DamageAmount;
	float ApplyDamage = Damage;

	if (StatCom)
		ApplyDamage = StatCom->ApplyDamage(Damage);

	UE_LOG(LogTemp, Warning, TEXT("%s\nOrigin Damge : %f\nApplyDamage : %f"), *GetActorNameOrLabel(), Damage, ApplyDamage);
	return Damage;
}

void AMyPlayer::MoveAction(const FInputActionValue& _Value)
{
	FVector2D vMovement = _Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// 카메라가 바라보는 컨트롤러 회전값의 Yaw만 추출
		const FRotator Rot = Controller->GetControlRotation();
		const FRotator YawRot = FRotator(0, Rot.Yaw, 0);

		// 카메라 기준 전방 및 우측 단위 벡터 계산
		const FVector ForwardDir = FRotationMatrix(YawRot).GetUnitAxis(EAxis::X);
		const FVector RightDir = FRotationMatrix(YawRot).GetUnitAxis(EAxis::Y);

		// 전방 입력과 우측 입력을 더해 최종 대각선 방향을 하나의 벡터로 만듦
		FVector DesiredDir = (ForwardDir * vMovement.X) + (RightDir * vMovement.Y);

		// 벡터의 길이를 정규화하여 대각선 이동 시 빨라지는 현상 방지
		if (!DesiredDir.IsNearlyZero())
		{
			DesiredDir.Normalize();

			// 하나의 합산된 방향 벡터와 입력의 총 크기 전달
			// 대각선 입력 시 크기 보정
			AddMovementInput(DesiredDir, vMovement.Size());
		}
	}
}

void AMyPlayer::LookAction(const FInputActionValue& _Value)
{
	FVector2D LookAxisVector = _Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		AddControllerYawInput(LookAxisVector.X);
		AddControllerPitchInput(LookAxisVector.Y);
	}
}

void AMyPlayer::AimingAction(const FInputActionValue& _Value)
{
	IsCamRecenter = true;
}

ETeamAttitude::Type AMyPlayer::GetTeamAttitudeTowards(const AActor& _Other) const
{
	const IGenericTeamAgentInterface* TeamAgent = Cast<IGenericTeamAgentInterface>(&_Other);

	if (TeamAgent)
	{
		FGenericTeamId OtherId = TeamAgent->GetGenericTeamId();

		if (GetGenericTeamId() == OtherId)
			return ETeamAttitude::Friendly;
		else
			return ETeamAttitude::Hostile;
	}

	return ETeamAttitude::Neutral;
}
