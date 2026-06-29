// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "ANS_SpawnTrail.generated.h"

UCLASS()
class DIANA_ACTION_GAME_API UANS_SpawnTrail : public UAnimNotifyState
{
	GENERATED_BODY()

protected:
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Trail")
    class UNiagaraSystem* TrailSystem;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Trail")
    FName StartSocketName;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Trail")
    FName EndSocketName;

private:
    UPROPERTY()
    class UNiagaraComponent* NiagaraCom;

public:
    virtual void NotifyBegin(USkeletalMeshComponent* _MeshCom, UAnimSequenceBase* _Animation, float _TotalDuration, const FAnimNotifyEventReference& _EventReference) override;
    virtual void NotifyEnd(USkeletalMeshComponent* _MeshCom, UAnimSequenceBase* _Animation, const FAnimNotifyEventReference& _EventReference) override;
};