// Fill out your copyright notice in the Description page of Project Settings.


#include "Projectile.h"

#include "Components/SphereComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "NiagaraComponent.h"

AProjectile::AProjectile()
{
	PrimaryActorTick.bCanEverTick = true;

	CollisionCom = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComponent"));
	RootComponent = CollisionCom;

	ProjMovementCom = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectilMovementComponent"));
	ProjMovementCom->UpdatedComponent = CollisionCom;

	NiagaraCom = CreateDefaultSubobject<UNiagaraComponent>(TEXT("NiagaraComponent"));

}

void AProjectile::BeginPlay()
{
	Super::BeginPlay();
	
}

void AProjectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

