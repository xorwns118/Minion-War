// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "ANS_ComboWindow.generated.h"

UCLASS()
class DIANA_ACTION_GAME_API UANS_ComboWindow : public UAnimNotifyState
{
	GENERATED_BODY()
	
public:
	virtual void NotifyBegin(USkeletalMeshComponent* _MeshCom, UAnimSequenceBase* _Animation, float _TotalDuration, const FAnimNotifyEventReference& _EventReference) override;
	virtual void NotifyTick(USkeletalMeshComponent* _MeshCom, UAnimSequenceBase* _Animation, float _FrameDeltaTime, const FAnimNotifyEventReference& _EventReference) override;
	virtual void NotifyEnd(USkeletalMeshComponent* _MeshCom, UAnimSequenceBase* _Animation, const FAnimNotifyEventReference& _EventReference) override;
};
