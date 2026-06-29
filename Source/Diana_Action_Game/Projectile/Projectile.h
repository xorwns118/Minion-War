// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Projectile.generated.h"

UCLASS()
class DIANA_ACTION_GAME_API AProjectile : public AActor
{
	GENERATED_BODY()
	
protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Component", DisplayName = "SphereComponent")
	class USphereComponent*				CollisionCom;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Component", DisplayName = "ProjectilMovementComponent")
	class UProjectileMovementComponent* ProjMovementCom;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Component", DisplayName = "NiagaraComponent")
	class UNiagaraComponent*			NiagaraCom;

	APawn*								Spawner;
	AController*						SkillInstigator;
	TSet<TWeakObjectPtr<AActor>>		HitActors;

public:
	void SetSpawner(APawn* _Spawner) { Spawner = _Spawner; }

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;

public:
	AProjectile();
};
