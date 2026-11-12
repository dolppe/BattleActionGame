#include "BattleUtilityAIComponent.h"

#include "BattleActionGame/Enemy/UtilityAI/Actions/BattleUtilityAction.h"
#include "BattleUtilityAIData.h"
#include "BattleActionGame/Character/BattleCharacterBase.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(BattleUtilityAIComponent)

UBattleUtilityAIComponent::UBattleUtilityAIComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UBattleUtilityAIComponent::BeginPlay()
{
	Super::BeginPlay();

	if (GetWorld()->GetNetMode() == NM_Client)
	{
		return;
	}
	
	if (UtilityAIData == nullptr)
	{
		return;
	}
	
	// ConsiderationFactors를 생성하고 초기화
	ConsiderList = NewObject<UConsiderationFactors>(GetOwner(), UConsiderationFactors::StaticClass());
	ConsiderList->InitConsiderFunction(UtilityAIData, GetOwner(), Settings);

	// 설정된 Action을 생성하고 초기화
	for (const FActionConfig& ActionConfig : UtilityAIData->ActionConfigs)
	{
		UBattleUtilityAction* NewAction = NewObject<UBattleUtilityAction>(this, ActionConfig.ActionClass);
		NewAction->InitAction(ActionConfig, ConsiderList);

		InstancedActions.Add(NewAction);
		LastActiveTimes.Add(-1000.0f);
	}


}

void UBattleUtilityAIComponent::TickComponent(float DeltaTime, ELevelTick TickType,
                                              FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (!HasAuthority())
	{
		return;
	}
	
	// 현재 활성화된 Action이 있으면 Tick을 진행
	if (ActiveAction)
	{
		ActiveAction->TickAction(DeltaTime);

		// 활성화된 Action이 완료되면 End로 끝냄
		if (ActiveAction->IsCompleteAction())
		{
			ActiveAction->EndAction();
			ActiveAction = nullptr;
			
			// Action이 종료되었기에 실행할 행동을 찾음
			SelectBestAction();
		}
	}
	else
	{
		// 현재 활성화된 Action이 없기에 실행할 행동을 찾음
		SelectBestAction();
	}
}


void UBattleUtilityAIComponent::CollectConsiderFactors()
{
	// 평가에 필요한 현재 상황 정보 수집
	ConsiderList->CollectConsiderationData();
	
	// 평가 항목별 계산 함수를 실행하고, 값을 저장하도록 함
	ConsiderList->CalcConsiderationFactors();
}


void UBattleUtilityAIComponent::SelectBestAction()
{
	if (ConsiderList == nullptr)
	{
		return;
	}
	if (ActiveAction != nullptr)
	{
		return;
	}
	
	// 현재 상황 정보를 수집하고, 평가값을 계산하도록 요청
	CollectConsiderFactors();
	
	// 조건에 맞고, 점수가 가장 높은 Action의 Index를 구함
	int32 SelectedActionIdx = FindBestActionIndex();
	
	UBattleUtilityAction* BestAction = nullptr;
	
	if (InstancedActions.IsValidIndex(SelectedActionIdx))
	{
		BestAction = InstancedActions[SelectedActionIdx];
	}
	
	// 선택된 Action 활성화
	if (BestAction != nullptr)
	{
		ActiveAction = BestAction;
		LastActiveTimes[SelectedActionIdx] = GetWorld()->GetTimeSeconds();
		ActiveAction->SetStartAction();
		ActiveAction->StartAction();
	}
}

int32 UBattleUtilityAIComponent::FindBestActionIndex()
{
	float BestScore = 0.0f;
	int32 SelectedActionIdx = INDEX_NONE;
	
	const float WorldTime = GetWorld()->GetTimeSeconds();
	
	for (int32 Idx = 0; Idx < InstancedActions.Num(); ++Idx)
	{
		UBattleUtilityAction* Action = InstancedActions[Idx];
		
		if (!IsValid(Action))
		{
			continue;
		}
		
		// UtilityAIData에서 활성화가 안됐으면 건너뜀
		if (UtilityAIData && UtilityAIData->ActionConfigs.Num() == InstancedActions.Num() && !UtilityAIData->ActionConfigs[Idx].bIsActive)
		{
			continue;
		}
		
		const float LastActiveTime = LastActiveTimes[Idx];
		const float TimeSinceLastActive = WorldTime - LastActiveTime;
		float CurScore = 0.0f;
		
		// CoolTime, Distance 조건에 만족하면 점수 계산
		if (TimeSinceLastActive >= Action->GetCoolTime() && CheckActionDistance(Action))
		{
			CurScore = Action->EvaluateScore();
		}
		

		
		const float AgeTime = Action->GetAgeTime();
		// AgeTime만큼 안지났으면 AgeRate를 적용해서 점수를 작게 만듦
		if (AgeTime > 0.0f && TimeSinceLastActive < AgeTime)
		{
			float AgeAlpha = TimeSinceLastActive / AgeTime;
			CurScore *= FMath::Lerp(Action->GetAgeRate(), 1.0f, AgeAlpha);
		}
		
		// 가장 큰 점수를 가진 ActionIdx을 구함
		if (BestScore < CurScore)
		{
			BestScore = CurScore;
			SelectedActionIdx = Idx;
		}
	}
	
	return SelectedActionIdx;
	
}

bool UBattleUtilityAIComponent::GetDistanceSourceLocation(EDistanceSourceType DistanceSourceType, FVector& OutLocation) const
{
	// Source의 Location을 담는데 성공하면 true 반환
	switch (DistanceSourceType)
	{
	case EDistanceSourceType::SelectedTarget:
		{
			const ABattleCharacterBase* SelectedTarget = ConsiderList->GetSelectedTarget();
			if (!IsValid(SelectedTarget))
			{
				return false;
			}
			OutLocation = SelectedTarget->GetActorLocation();
			return true;	
		}
	default:
		return false;
	}
}

bool UBattleUtilityAIComponent::CheckActionDistance(UBattleUtilityAction* UtilityAction) const
{
	const EDistanceSourceType SourceType = UtilityAction->GetDistanceSourceType();
	
	if (SourceType == EDistanceSourceType::None)
	{
		return true;
	}
	
	FVector SourceLocation;
	
	if (!GetDistanceSourceLocation(SourceType, SourceLocation))
	{
		// DistanceSource의 Location을 가져오는데 실패했기 때문에 false 리턴
		return false;
	}
	
	const float Distance = FVector::Dist(GetOwner()->GetActorLocation(), SourceLocation);
	
	return UtilityAction->CanAllowedRange(Distance);
}



