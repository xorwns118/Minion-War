// Fill out your copyright notice in the Description page of Project Settings.


#include "Monster.h"

#include "GameFramework/CharacterMovementComponent.h"
#include "Components/CapsuleComponent.h"

#include "../../AI/AIController/MonAIController.h"
#include "../../Component/MonStatComponent.h"
#include "../../GlobalEnum.h"

AMonster::AMonster()
{
	PrimaryActorTick.bCanEverTick = true;

	GetCharacterMovement()->MaxWalkSpeed = 420.f;

	StatCom = CreateDefaultSubobject<UMonStatComponent>(TEXT("StatComponent"));
}

void AMonster::BeginPlay()
{
	Super::BeginPlay();

	SetGenericTeamId((uint8)ETeamType::Enemy);
}

void AMonster::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}