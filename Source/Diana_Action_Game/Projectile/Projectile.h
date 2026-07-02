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

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Component", DisplayName = "SceneComponent")
	class USceneComponent*				SceneCom;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Component", DisplayName = "ProjectilMovementComponent")
	class UProjectileMovementComponent* ProjMovementCom;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Component", DisplayName = "TrailComponent")
	class UNiagaraComponent*			TrailCom;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HitEvent")
	bool								HitExplosion;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Life")
	float								LifeTime;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Life")
	float								Overtime;

	FTimerHandle						DestroyTimerHandle;
	bool								IsExplosion;

	TArray<AActor*>						HitActors;

	TSoftObjectPtr<class USkillData_Projectile> ProjData;

public:
	void SetData(TSoftObjectPtr<class USkillData_Projectile> _Data) { ProjData = _Data; }

	void ProjHit(AActor* _DamagedActor);

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;

public:
	UFUNCTION()
	void OnProjectileOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,UPrimitiveComponent* OtherComp,
							int32 OtherBodyIndex,bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION(BlueprintCallable)
	void ExplodeAndHide();

public:
	AProjectile();
};
