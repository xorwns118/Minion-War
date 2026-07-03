// Fill out your copyright notice in the Description page of Project Settings.


#include "Task_RandomMove.h"

#include "AIController.h"
#include "NavigationSystem.h"

#include "GameFramework/CharacterMovementComponent.h"
#include "Navigation/PathFollowingComponent.h"

#include "../../Actor/Monster/Monster.h"

UTask_RandomMove::UTask_RandomMove()
{
    bCreateNodeInstance = false;
    bNotifyTick = true;
}

EBTNodeResult::Type UTask_RandomMove::ExecuteTask(UBehaviorTreeComponent& _OwnerCom, uint8* _NodeMemory)
{
    AAIController* AIController = _OwnerCom.GetAIOwner();
    if (AIController == nullptr)
        return EBTNodeResult::Failed;

    APawn* Pawn = AIController->GetPawn();
    if (Pawn == nullptr)
        return EBTNodeResult::Failed;

    UNavigationSystemV1* NavSys = FNavigationSystem::GetCurrent<UNavigationSystemV1>(_OwnerCom.GetWorld());
    if (NavSys == nullptr)
        return EBTNodeResult::Failed;

    FNavLocation RandomLocation;
    bool IsTargetAreaSet = NavSys->GetRandomReachablePointInRadius(Pawn->GetActorLocation(), 2000.0f, RandomLocation);

    if (IsTargetAreaSet)
    {
        const EPathFollowingRequestResult::Type Result = AIController->MoveToLocation(RandomLocation.Location, 50.f);

        switch (Result)
        {
        case EPathFollowingRequestResult::AlreadyAtGoal:
            return EBTNodeResult::Succeeded;
        case EPathFollowingRequestResult::Failed:
            return EBTNodeResult::Failed;
        case EPathFollowingRequestResult::RequestSuccessful:
            *reinterpret_cast<FVector*>(_NodeMemory) = RandomLocation.Location;
            return EBTNodeResult::InProgress;
        default:
            return EBTNodeResult::Failed;
        }
    }

    return EBTNodeResult::Failed;
}

void UTask_RandomMove::TickTask(UBehaviorTreeComponent& _OwnerCom, uint8* _NodeMemory, float _DeltaSeconds)
{
    AAIController* AIController = _OwnerCom.GetAIOwner();
    if (AIController == nullptr)
    {
        FinishLatentTask(_OwnerCom, EBTNodeResult::Failed);
        return;
    }

    APawn* Pawn = AIController->GetPawn();
    if (Pawn == nullptr)
    {
        FinishLatentTask(_OwnerCom, EBTNodeResult::Failed);
        return;
    }

    ACharacter* Char = Cast<ACharacter>(Pawn);

    if (Char && Char->GetCharacterMovement()->IsFalling())
    {
        AIController->StopMovement();
        FinishLatentTask(_OwnerCom, EBTNodeResult::Succeeded);
        return;
    }

    FVector TargetLocation = *reinterpret_cast<FVector*>(_NodeMemory);

    if (FVector::Dist(Pawn->GetActorLocation(), TargetLocation) <= 50.f)
    {
        FinishLatentTask(_OwnerCom, EBTNodeResult::Succeeded);
        return;
    }

    // 목표지점으로 향하다가 중간에 막힌 경우
    if (AIController->GetMoveStatus() == EPathFollowingStatus::Idle)
    {
        FinishLatentTask(_OwnerCom, EBTNodeResult::Failed);
    }
}