// Fill out your copyright notice in the Description page of Project Settings.


#include "Projectile.h"

#include "Components/SphereComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Engine/OverlapResult.h"

#include "../Component/SkillComponent.h"
#include "../Data/SkillData_Projectile.h"

AProjectile::AProjectile()
{
	PrimaryActorTick.bCanEverTick = true;

	CollisionCom = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComponent"));
	RootComponent = CollisionCom;

	ProjMovementCom = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectilMovementComponent"));
	ProjMovementCom->UpdatedComponent = CollisionCom;

    TrailCom = CreateDefaultSubobject<UNiagaraComponent>(TEXT("TrailComponent"));
    if (TrailCom)
        TrailCom->SetupAttachment(RootComponent);
}

void AProjectile::BeginPlay()
{
    Super::BeginPlay();

    if (CollisionCom)
        CollisionCom->OnComponentBeginOverlap.AddDynamic(this, &AProjectile::OnProjectileOverlap);

    // 이펙트 효과 조절과 자연스러운 소멸을 위한 타이머 핸들
    if (LifeTime > 0.f)
    {
        GetWorldTimerManager().SetTimer(
            DestroyTimerHandle,
            this,
            &AProjectile::ExplodeAndHide,
            LifeTime,
            false
        );
    }
}

void AProjectile::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

}

void AProjectile::ProjHit(AActor* _DamagedActor)
{
    FVector SpawnLocation = GetActorLocation();
    FRotator SpawnRotation = GetActorRotation();

    if (ProjData == nullptr)
        return;

    if (ProjData->HitEffect != nullptr)
    {
        UNiagaraFunctionLibrary::SpawnSystemAtLocation(
            GetWorld(),
            ProjData->HitEffect,
            SpawnLocation,
            SpawnRotation
        );
    }

    HitActors.Add(this);
    HitActors.Add(GetOwner());
    HitActors.Add(_DamagedActor);

    UGameplayStatics::ApplyDamage(
        _DamagedActor,
        ProjData->Damage,
        GetInstigatorController(),
        this,
        UDamageType::StaticClass()
    );

    DrawDebugSphere(GetWorld(), GetActorLocation(), CollisionCom->GetScaledSphereRadius(), 10, FColor::Red);
}

void AProjectile::OnProjectileOverlap(UPrimitiveComponent* _OverlappedCom, AActor* _OtherActor, UPrimitiveComponent* _OtherCom, int32 _OtherBodyIndex, bool _bFromSweep, const FHitResult& _SweepResult)
{
    if (_OtherActor && _OtherActor != this && _OtherActor != GetOwner())
        ProjHit(_OtherActor);
}

void AProjectile::ExplodeAndHide()
{
    SetLifeSpan(Overtime); // 파티클이 사라지지 않도록 제거시간 조정

    if (CollisionCom)
        CollisionCom->SetCollisionEnabled(ECollisionEnabled::NoCollision);

    if (TrailCom)
        TrailCom->Deactivate(); // 새로운 파티클 발사 중단 - 이미 공중에 그려진 잔상들만 남게 됨

    if (ProjData == nullptr)
        return;

    if (ProjData->EndEffect)
    {
        UNiagaraFunctionLibrary::SpawnSystemAtLocation(
            GetWorld(),
            ProjData->EndEffect,
            GetActorLocation(),
            FRotator()
        );
    }

    if (!ProjData->IsHitExplosion)
        return;

    TArray<FOverlapResult> OverlapResults;
    FCollisionQueryParams Params;
    Params.AddIgnoredActor(this);
    Params.AddIgnoredActor(GetOwner());

    FCollisionShape Shape = FCollisionShape::MakeSphere(ProjData->DamageRadius);

    bool bHit = GetWorld()->OverlapMultiByChannel(
        OverlapResults,
        GetActorLocation(),
        FQuat::Identity,
        ECollisionChannel::ECC_Pawn,
        Shape,
        Params
    );

    if (bHit)
    {
        for (const FOverlapResult& Result : OverlapResults)
        {
            AActor* HitActor = Result.GetActor();
            if (HitActor && !HitActors.Contains(HitActor))
            {
                HitActors.Add(HitActor);

                UGameplayStatics::ApplyRadialDamage(
                    GetWorld(),
                    ProjData->Damage,
                    GetActorLocation(),
                    ProjData->DamageRadius,
                    UDamageType::StaticClass(),
                    HitActors,
                    this,
                    GetInstigatorController(),
                    true
                );

                if (ProjData->HitEffect != nullptr)
                {
                    UNiagaraFunctionLibrary::SpawnSystemAtLocation(
                        GetWorld(),
                        ProjData->HitEffect,
                        HitActor->GetActorLocation(),
                        HitActor->GetActorRotation()
                    );
                }
            }
        }
    }

    DrawDebugSphere(GetWorld(), GetActorLocation(), ProjData->DamageRadius, 10, FColor::Red);
}