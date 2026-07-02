// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "Perception/AIPerceptionTypes.h"
#include "MonAIController.generated.h"

USTRUCT(BlueprintType)
struct FSensedTargetInfo
{
	GENERATED_BODY()

public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TWeakObjectPtr<AActor>	Target;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	float					AggroValue = 0.f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	bool					IsSensed;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	FVector					LosePos;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	float					LoseTime;
};

UCLASS()
class DIANA_ACTION_GAME_API AMonAIController : public AAIController
{
	GENERATED_BODY()
	
protected:
	UPROPERTY(VisibleAnywhere, Category = "AI")
	class UAIPerceptionComponent*	PerceptionCom;

	UPROPERTY(VisibleAnywhere, Category = "AI")
	class UAISenseConfig_Sight*		SightConfig;

	UPROPERTY(VisibleAnywhere, Category = "AI")
	class UAISenseConfig_Damage*	DamageConfig;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	class UBehaviorTree*			BTAsset;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	class UBlackboardData*			BBAsset;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI")
	TArray<FSensedTargetInfo>		SensedTargets;

public:
	const TArray<FSensedTargetInfo>& GetSensedTargets() { return SensedTargets; }

	FSensedTargetInfo& AddSensedTarget(AActor* _Target);
	FSensedTargetInfo* FindSensedTarget(const AActor* _Target);

	void ClearSensedTarget(float _LimitTime);

public:
	UFUNCTION()
	void OnTargetDetected(AActor* _Target, FAIStimulus _Stimulus);

protected:
	virtual void OnPossess(APawn* _Pawn) override;

public:
	AMonAIController();
};
