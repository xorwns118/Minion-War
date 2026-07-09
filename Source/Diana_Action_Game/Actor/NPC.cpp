// Fill out your copyright notice in the Description page of Project Settings.


#include "NPC.h"

#include "GameFramework/CharacterMovementComponent.h"
#include "Components/CapsuleComponent.h"

#include "../Component/SkillComponent.h"
#include "../Component/BuffComponent.h"
#include "../Component/StatComponent.h"

ANPC::ANPC()
{
	PrimaryActorTick.bCanEverTick = true;

	PrimaryActorTick.bCanEverTick = true;

	GetMesh()->SetRelativeRotation(FRotator(0.f, -90.f, 0.f));
	GetMesh()->SetRelativeLocation(FVector(0.f, 0.f, -95.f));

	SkillCom = CreateDefaultSubobject<USkillComponent>(TEXT("SkillComponent"));
	BuffCom = CreateDefaultSubobject<UBuffComponent>(TEXT("BuffComponent"));

	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 300.0f, 0.0f);
}

void ANPC::BeginPlay()
{
	Super::BeginPlay();
	
}

void ANPC::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

ETeamAttitude::Type ANPC::GetTeamAttitudeTowards(const AActor& _Other) const
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

float ANPC::TakeDamage(float _DamageAmount, FDamageEvent const& _DamageEvent, AController* _EventInstigator, AActor* _DamageCauser)
{
	Super::TakeDamage(_DamageAmount, _DamageEvent, _EventInstigator, _DamageCauser);

	float Damage = _DamageAmount;
	float ApplyDamage = Damage;

	UStatComponent* StatCom = GetComponentByClass<UStatComponent>();

	if (StatCom)
		ApplyDamage = StatCom->ApplyDamage(Damage);

	UE_LOG(LogTemp, Warning, TEXT("%s\nOrigin Damge : %f\nApplyDamage : %f"), *GetActorNameOrLabel(), Damage, ApplyDamage);
	return Damage;
}