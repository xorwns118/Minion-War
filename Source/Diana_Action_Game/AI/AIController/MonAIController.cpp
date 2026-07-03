// Fill out your copyright notice in the Description page of Project Settings.


#include "MonAIController.h"

#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISenseConfig_Sight.h"
#include "Perception/AISenseConfig_Damage.h"
#include "Perception/AISense_Damage.h"

#include "GenericTeamAgentInterface.h"

#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/BlackboardData.h"

#include "../../Actor/Monster/Monster.h"
#include "../../GlobalEnum.h"
#include "../../Component/StatComponent.h"

AMonAIController::AMonAIController()
{
	PerceptionCom = CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("PerceptionComponent"));
	SetPerceptionComponent(*PerceptionCom);

	SightConfig = CreateDefaultSubobject<UAISenseConfig_Sight>(TEXT("Sight"));

	if (SightConfig)
	{
		SightConfig->SightRadius = 3000.f;
		SightConfig->LoseSightRadius = 3500.f;
		SightConfig->PeripheralVisionAngleDegrees = 60.f;
		SightConfig->DetectionByAffiliation.bDetectEnemies = true;
		SightConfig->DetectionByAffiliation.bDetectFriendlies = false;
		SightConfig->DetectionByAffiliation.bDetectNeutrals = false;

		PerceptionCom->ConfigureSense(*SightConfig);
		PerceptionCom->SetDominantSense(SightConfig->GetSenseImplementation());
	}

	DamageConfig = CreateDefaultSubobject<UAISenseConfig_Damage>(TEXT("Damage"));

	if (DamageConfig)
		PerceptionCom->ConfigureSense(*DamageConfig);

	PerceptionCom->OnTargetPerceptionUpdated.AddDynamic(this, &AMonAIController::OnTargetDetected);
}

FSensedTargetInfo& AMonAIController::AddSensedTarget(AActor* _Target)
{
	FSensedTargetInfo Info;
	Info.Target = _Target;

	return SensedTargets.Add_GetRef(Info); // 추가된 아이템 레퍼런스를 리턴
}

FSensedTargetInfo* AMonAIController::FindSensedTarget(const AActor* _Target)
{
	for (FSensedTargetInfo& Info : SensedTargets)
	{
		if (Info.Target == _Target)
			return &Info;
	}

	return nullptr;
}

void AMonAIController::ClearSensedTarget(float _LimitTime)
{
	float CurTime = GetWorld()->GetTimeSeconds();

	for (auto iter = SensedTargets.CreateIterator(); iter; ++iter)
	{
		bool Remove = false;

		// 감지 대상이 삭제된 경우
		if (!iter->Target.IsValid())
		{
			Remove = true;
		}
		else if (!iter->IsSensed)
		{
			// 감지 못한 시간이 LimitTime 을 넘어서면
			if (CurTime - iter->LoseTime > _LimitTime)
			{
				Remove = true;
			}
		}

		if (Remove)
			iter.RemoveCurrent();
	}
}

void AMonAIController::OnTargetDetected(AActor* _Target, FAIStimulus _Stimulus)
{
	// 자극의 근원지 추적 불가능
	if (_Target == nullptr)
		return;

	AMonster* Monster = Cast<AMonster>(GetPawn());
	if (Monster == nullptr)
		return;

	ETeamAttitude::Type Type = Monster->GetTeamAttitudeTowards(*_Target);

	if (Type == ETeamAttitude::Hostile)
	{
		FSensedTargetInfo* Info = FindSensedTarget(_Target);

		if (Info == nullptr && _Stimulus.WasSuccessfullySensed())
			Info = &AddSensedTarget(_Target);

		Info->IsSensed = _Stimulus.WasSuccessfullySensed();	// 감지 여부 기록

		if (Info && !Info->IsSensed)
		{
			Info->LosePos = _Stimulus.StimulusLocation;		// 놓친 시점의 위치
			Info->LoseTime = GetWorld()->GetTimeSeconds();	// 놓친 시간값 기록
		}

		if (Info == nullptr && !_Stimulus.WasSuccessfullySensed())
		{
			UE_LOG(LogTemp, Error, TEXT("SkillComponent::OnTargetDeceted() : ????????"));
			return;
		}

		static FAISenseID SightId = UAISense::GetSenseID<UAISense_Sight>();
		static FAISenseID DamageId = UAISense::GetSenseID<UAISense_Damage>();

		if (Info->IsSensed && _Stimulus.Type == SightId)
			Info->AggroValue += 10.f;	// 시야 인지 어그로 10
		else if (_Stimulus.Type == DamageId)
			Info->AggroValue += 20.f;	// 데미지 인지 어그로 20
	}
}

void AMonAIController::OnPossess(APawn* _Pawn)
{
	Super::OnPossess(_Pawn);

	const IGenericTeamAgentInterface* PawnTeam = Cast<IGenericTeamAgentInterface>(_Pawn);

	if (PawnTeam)
		SetGenericTeamId(PawnTeam->GetGenericTeamId());
	else
		SetGenericTeamId((uint8)ETeamType::Neutral);

	UStatComponent* StatCom = _Pawn->FindComponentByClass<UStatComponent>();

	SightConfig->SightRadius = StatCom->GetStat(TEXT("DetectRange"));
	SightConfig->LoseSightRadius = StatCom->GetStat(TEXT("LoseDetectRange"));

	PerceptionCom->ConfigureSense(*SightConfig);
	PerceptionCom->ConfigureSense(*DamageConfig);
	PerceptionCom->RequestStimuliListenerUpdate();

	if (BTAsset && BBAsset)
	{
		UBlackboardComponent* BBCom = Blackboard;

		if (UseBlackboard(BBAsset, BBCom))
		{
			Blackboard = BBCom;

			RunBehaviorTree(BTAsset);
		}
	}
}