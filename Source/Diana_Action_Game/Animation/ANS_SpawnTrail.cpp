// Fill out your copyright notice in the Description page of Project Settings.


#include "ANS_SpawnTrail.h"

#include "NiagaraFunctionLibrary.h"
#include "NiagaraComponent.h"

void UANS_SpawnTrail::NotifyBegin(USkeletalMeshComponent* _MeshCom, UAnimSequenceBase* _Animation, float _TotalDuration, const FAnimNotifyEventReference& _EventReference)
{
	Super::NotifyBegin(_MeshCom, _Animation, _TotalDuration, _EventReference);

    if (!TrailSystem)
        return;

    NiagaraCom = UNiagaraFunctionLibrary::SpawnSystemAttached(
        TrailSystem,
        _MeshCom,
        StartSocketName,
        FVector::ZeroVector,
        FRotator::ZeroRotator,
        EAttachLocation::SnapToTarget,
        true
    );
}

void UANS_SpawnTrail::NotifyEnd(USkeletalMeshComponent* _MeshCom, UAnimSequenceBase* _Animation, const FAnimNotifyEventReference& _EventReference)
{
	Super::NotifyEnd(_MeshCom, _Animation, _EventReference);
}
