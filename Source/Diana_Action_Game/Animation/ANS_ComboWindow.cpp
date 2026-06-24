// Fill out your copyright notice in the Description page of Project Settings.


#include "ANS_ComboWindow.h"

#include "GameFramework/Character.h"
#include "../Component/SkillComponent.h"

void UANS_ComboWindow::NotifyBegin(USkeletalMeshComponent* _MeshCom, UAnimSequenceBase* _Animation, float _TotalDuration, const FAnimNotifyEventReference& _EventReference)
{
	Super::NotifyBegin(_MeshCom, _Animation, _TotalDuration, _EventReference);

	ACharacter* Character = Cast<ACharacter>(_MeshCom->GetOwner());
	if (Character == nullptr)
		return;

	USkillComponent* SkillCom = Character->FindComponentByClass<USkillComponent>();
	if (SkillCom == nullptr)
		return;

	SkillCom->OpenComboWindow();
}

void UANS_ComboWindow::NotifyTick(USkeletalMeshComponent* _MeshCom, UAnimSequenceBase* _Animation, float _FrameDeltaTime, const FAnimNotifyEventReference& _EventReference)
{
}

void UANS_ComboWindow::NotifyEnd(USkeletalMeshComponent* _MeshCom, UAnimSequenceBase* _Animation, const FAnimNotifyEventReference& _EventReference)
{
	Super::NotifyEnd(_MeshCom, _Animation, _EventReference);

	ACharacter* Character = Cast<ACharacter>(_MeshCom->GetOwner());
	if (Character == nullptr)
		return;

	USkillComponent* SkillCom = Character->FindComponentByClass<USkillComponent>();
	if (SkillCom == nullptr)
		return;

	SkillCom->CloseComboWindow();
}
