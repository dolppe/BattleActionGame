#include "BattleUtilityAIComponent.h"

#include "BattleActionGame/Enemy/UtilityAI/Actions/BattleUtilityAction.h"
#include "BattleUtilityAIData.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(BattleUtilityAIComponent)

UBattleUtilityAIComponent::UBattleUtilityAIComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	PrimaryComponentTick.bCanEverTick = true;
	bActionComplete = true;

	ActiveAction = nullptr;
}

void UBattleUtilityAIComponent::BeginPlay()
{
	Super::BeginPlay();

	for (FActionConfig ActionConfig : UtilityAIData->ActionConfigs)
	{
		UBattleUtilityAction* NewAction = NewObject<UBattleUtilityAction>(this, ActionConfig.ActionClass);
		NewAction->InitAxis(ActionConfig.AxisConfigs);

		InstancedActions.Add(NewAction);
	}
	
	
	CollectConsiderFactors();

	StartTimer();

}

void UBattleUtilityAIComponent::CollectConsiderFactors()
{
	
	
}

void UBattleUtilityAIComponent::SelectBestAction()
{

	CollectConsiderFactors();
	
	float BestScore = 0.0f;
	UBattleUtilityAction* BestAction = nullptr;

	for (UBattleUtilityAction* Action : InstancedActions)
	{
		float CurScore = Action->EvaluateScore(ConsiderList);
		if (BestScore < CurScore)
		{
			BestScore = CurScore;
			BestAction = Action;
		}
	}

	if (bActionComplete)
	{
		// 액션 수행
		ActiveAction->EndAction();

		ActiveAction = BestAction;
		ActiveAction->StartAction();
		bActionComplete = false;

	}
	else if (BestAction != ActiveAction && BestAction->GetPriority() < ActiveAction->GetPriority())
	{
		// 액션 수행
		ActiveAction->EndAction();
		
		ActiveAction = BestAction;
		ActiveAction->StartAction();
		bActionComplete = false;
		
	}
	
	StartTimer();
	

}

void UBattleUtilityAIComponent::StartTimer()
{
	GetWorld()->GetTimerManager().SetTimer(TimerHandle, this, &UBattleUtilityAIComponent::SelectBestAction, UpdatePeriod, false);
}

void UBattleUtilityAIComponent::TickComponent(float DeltaTime, ELevelTick TickType,
	FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (ActiveAction)
	{
		bActionComplete = ActiveAction->TickAction(DeltaTime);

		if (bActionComplete)
		{
			SelectBestAction();
		}
	}
}
