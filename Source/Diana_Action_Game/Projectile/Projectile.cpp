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

    UGameplayStatics::ApplyDamage(
        _DamagedActor,
        ProjData->Damage,
        GetInstigatorController(),
        this,
        UDamageType::StaticClass()
    );

    HitActors.Add(_DamagedActor);

    DrawDebugSphere(GetWorld(), GetActorLocation(), CollisionCom->GetScaledSphereRadius(), 10, FColor::Red);
}

void AProjectile::OnProjectileOverlap(UPrimitiveComponent* _OverlappedCom, AActor* _OtherActor, UPrimitiveComponent* _OtherCom, int32 _OtherBodyIndex, bool _bFromSweep, const FHitResult& _SweepResult)
{
    if (_OtherActor && Cast<APawn>(_OtherActor) && _OtherActor != this && _OtherActor != GetOwner())
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

    if (ProjData->ExplosionEffect)
    {
        UNiagaraFunctionLibrary::SpawnSystemAtLocation(
            GetWorld(),
            ProjData->ExplosionEffect,
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

    FCollisionObjectQueryParams ObjectParams;
    ObjectParams.AddObjectTypesToQuery(ECC_Pawn);

    FCollisionShape Shape = FCollisionShape::MakeSphere(ProjData->DamageRadius);

    bool bFind = GetWorld()->OverlapMultiByObjectType(
        OverlapResults,
        GetActorLocation(),
        FQuat::Identity,
        ObjectParams,
        Shape,
        Params
    );

    TArray<AActor*> ExplosionTargets;

    if (bFind)
    {
        for (const FOverlapResult& Result : OverlapResults)
        {
            AActor* HitActor = Result.GetActor();

            // 투사체가 지나가면서 이미 맞은 적 제외 + Pawn 타입이 아니면 무시
            if (HitActor && Cast<APawn>(HitActor) && !HitActors.Contains(HitActor) && !ExplosionTargets.Contains(HitActor))
            {
                ExplosionTargets.Add(HitActor);

                if (ProjData->HitEffect != nullptr)
                {
                    UNiagaraFunctionLibrary::SpawnSystemAtLocation(
                        GetWorld(),
                        ProjData->HitEffect,
                        HitActor->GetActorLocation(),
                        HitActor->GetActorRotation()
                    );

                    UE_LOG(LogTemp, Warning, TEXT("Spawn Niagara : X : %f, Y %f: , Z : %f - HitActorName : %s"),
                        HitActor->GetActorLocation().X,
                        HitActor->GetActorLocation().Y,
                        HitActor->GetActorLocation().Z,
                        *HitActor->GetActorNameOrLabel()
                    );
                }
            }
        }
    }

    TArray<AActor*> IgnoreActors;
    IgnoreActors.Add(this);
    IgnoreActors.Add(GetOwner());

    UGameplayStatics::ApplyRadialDamage(
        GetWorld(),
        ProjData->Damage,
        GetActorLocation(),
        ProjData->DamageRadius,
        UDamageType::StaticClass(),
        IgnoreActors,
        this,
        GetInstigatorController(),
        true
    );

    DrawDebugSphere(GetWorld(), GetActorLocation(), ProjData->DamageRadius, 10, FColor::Red);
}