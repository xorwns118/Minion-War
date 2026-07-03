// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "Task_RandomMove.generated.h"

UCLASS()
class DIANA_ACTION_GAME_API UTask_RandomMove : public UBTTaskNode
{
	GENERATED_BODY()
	
public:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& _OwnerCom, uint8* _NodeMemory) override;
	virtual void TickTask(UBehaviorTreeComponent& _OwnerCom, uint8* _NodeMemory, float _DeltaSeconds) override;

	virtual uint16 GetInstanceMemorySize() const override
	{
		return sizeof(FVector);
	}

public:
	UTask_RandomMove();
};
