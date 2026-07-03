// Fill out your copyright notice in the Description page of Project Settings.


#include "ANS_AoE_ExecuteScope.h"

#include "GameFramework/Character.h"
#include "../Component/SkillComponent.h"

void UANS_AoE_ExecuteScope::NotifyBegin(USkeletalMeshComponent* _MeshCom, UAnimSequenceBase* _Animation, float _TotalDuration, const FAnimNotifyEventReference& _EventReference)
{
	Super::NotifyBegin(_MeshCom, _Animation, _TotalDuration, _EventReference);

	ACharacter* Character = Cast<ACharacter>(_MeshCom->GetOwner());
	if (Character == nullptr)
		return;

	USkillComponent* SkillCom = Character->GetComponentByClass<USkillComponent>();
	if (SkillCom == nullptr)
		return;

	SkillCom->DianaUltimate_Start();
}

void UANS_AoE_ExecuteScope::NotifyTick(USkeletalMeshComponent* _MeshCom, UAnimSequenceBase* _Animation, float _FrameDeltaTime, const FAnimNotifyEventReference& _EventReference)
{
	Super::NotifyTick(_MeshCom, _Animation, _FrameDeltaTime, _EventReference);
}

void UANS_AoE_ExecuteScope::NotifyEnd(USkeletalMeshComponent* _MeshCom, UAnimSequenceBase* _Animation, const FAnimNotifyEventReference& _EventReference)
{
	Super::NotifyEnd(_MeshCom, _Animation, _EventReference);

	ACharacter* Character = Cast<ACharacter>(_MeshCom->GetOwner());
	if (Character == nullptr)
		return;

	USkillComponent* SkillCom = Character->GetComponentByClass<USkillComponent>();
	if (SkillCom == nullptr)
		return;

	SkillCom->DianaUltimate_End();
}
