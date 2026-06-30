// Fill out your copyright notice in the Description page of Project Settings.


#include "Monster.h"

#include "GameFramework/CharacterMovementComponent.h"
#include "Components/CapsuleComponent.h"

AMonster::AMonster()
{
	PrimaryActorTick.bCanEverTick = true;

	GetCharacterMovement()->MaxWalkSpeed = 420.f;
}

void AMonster::BeginPlay()
{
	Super::BeginPlay();

	SetGenericTeamId(1);
}

void AMonster::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}