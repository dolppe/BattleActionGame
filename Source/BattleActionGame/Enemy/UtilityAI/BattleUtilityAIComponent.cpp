#include "BattleUtilityAIComponent.h"

#include "BattleActionGame/Enemy/UtilityAI/Actions/BattleUtilityAction.h"
#include "BattleUtilityAIData.h"
#include "BattleActionGame/BattleLogChannels.h"
#include "BattleActionGame/Character/BattleCharacter.h"
#include "BattleActionGame/Character/BattleCharacterBase.h"
#include "BattleActionGame/Character/BattleHealthComponent.h"
#include "BattleActionGame/Physics/BattleCollisionChannels.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(BattleUtilityAIComponent)

DEFINE_ENUM_TO_STRING(EBattleConsiderType,"/Script/BattleActionGame")

UConsiderationFactors::UConsiderationFactors()
{
};

float UConsiderationFactors::GetTarget()
{
	// 1 => 존재, 0 => 없음
	if (SelectedTarget == nullptr)
	{
		return 0.0f;
	}
	else
	{
		return 1.0f;
	}
}

float UConsiderationFactors::GetMyCombatPotential()
{
	// 전투 가능한 상태인지 0 => 전투 불가능 도주해야함, 1 => 전투 가능
	// 내 Hp
	// 추후에 궁극기 유무, 나한테 디버프 있는지, 주변의 적 수,
	
	return MyHp;
	
}

TArray<float> UConsiderationFactors::GetTargetDistanceNearly()
{
	// 1 => 엄청 멈, 0 => 가까움
	return TargetDistances;
}

TArray<float> UConsiderationFactors::GetTargetHp()
{
	return TargetHps;
}

TArray<float> UConsiderationFactors::GetTargetPriority()
{
	// 0 => 중요치 않음, 1 => 중요함
	// Character면 높게, NPC면 낮게
	// Distance가 멀면 낮게, 가까우면 높게
	// 추후에는 데미지를 얼마나 넣었는지 같은 AI 입장에서 거슬리는 것들 (뭉쳐있는것도)

	TArray<float> ResultArray;
	
	for (int Idx =0; Idx<TargetActors.Num();Idx++)
	{
		float Result = 1.0f;

		if (!TargetActors[Idx].IsA(ABattleCharacter::StaticClass()))
		{
			Result *= 0.5f;
		}
		
		Result *= (1 - TargetDistances[Idx]);

		ResultArray.Add(Result);
	}

	return ResultArray;
}

TArray<float> UConsiderationFactors::GetTargetWeakness()
{
	// WeakNess 0 => 건강함, 1 => 약해진 상태
	// Hp가 얼마나 없는지, Distance는 얼마나 가까운지
	// 추후 상태이상에 걸렸는지, Enemy가 주변에 있는지
	// 
	TArray<float> ResultArray;

	for (int Idx =0; Idx<TargetActors.Num();Idx++)
	{
		float Result = 1.0f;
		Result *= (1 - TargetHps[Idx]);
		Result *= (1 - TargetDistances[Idx]);

		ResultArray.Add(Result);
	}
	
	return ResultArray;
}

float UConsiderationFactors::GetMyHp()
{
	return MyHp;
}

TFunction<float()> UConsiderationFactors::GetConsiderFunction(EBattleConsiderType ConsiderType)
{
	switch (ConsiderType)
	{
	case EBattleConsiderType::HasTarget:
		return [this]() -> float
		{
			return GetTarget();
		};
		break;
	case EBattleConsiderType::MyHp:
		return [this]() -> float
		{
			return GetMyHp();
		};
		break;
	case EBattleConsiderType::MyCombatPotential:
		return [this]() -> float
		{
			return GetMyCombatPotential();
		};
		break;
	default:
		return [this]() -> float
		{
			return 0.0f;
		};		
	}
}

TFunction<TArray<float>()> UConsiderationFactors::GetArrayConsiderFunction(EBattleConsiderType ConsiderType)
{
	switch (ConsiderType)
	{
	case EBattleConsiderType::TargetDistanceNearly:
		return [this]() -> TArray<float>
		{
			return GetTargetDistanceNearly();
		};
		break;
	case EBattleConsiderType::TargetHp:
		return [this]() -> TArray<float>
		{
			return GetTargetHp();
		};
		break;
	case EBattleConsiderType::TargetPriority:
		return [this]() -> TArray<float>
		{
			return GetTargetPriority();
		};
		break;
	case EBattleConsiderType::TargetWeakness:
		return [this]() -> TArray<float>
		{
			return GetTargetWeakness();
		};
		break;
	default:
		return [this]() -> TArray<float>
		{
			return {0.0f};
		};		
	}
}

EAxisType UConsiderationFactors::GetAxisType(EBattleConsiderType ConsiderType)
{
	switch (ConsiderType)
	{
	case EBattleConsiderType::TargetHp:
	case EBattleConsiderType::TargetDistanceNearly:
	case EBattleConsiderType::TargetPriority:
	case EBattleConsiderType::TargetWeakness:
		return EAxisType::Target;
	case EBattleConsiderType::HasTarget:
	case EBattleConsiderType::MyHp:
	case EBattleConsiderType::MyCombatPotential:
		return EAxisType::Single;
	}
	return EAxisType::Single;
}

void UConsiderationFactors::InitConsiderFunction(const UBattleUtilityAIData* UtilityAIData)
{
	for (EBattleConsiderType ConsiderType: UtilityAIData->Consider)
	{
		if (GetAxisType(ConsiderType) == EAxisType::Single)
		{
			Functions.Add(ConsiderType,GetConsiderFunction(ConsiderType));
			Factors.Add(ConsiderType) = 0.0f;
		}
		else
		{
			ArrayFunctions.Add(ConsiderType,GetArrayConsiderFunction(ConsiderType));
			ArrayFactors.Add(ConsiderType,{0.0f});
		}
		
	}

	UtilityAIComponent = Cast<UBattleUtilityAIComponent>(GetOuter());
	MyCharacter = Cast<ABattleCharacterBase>(UtilityAIComponent->GetOwner());
}

void UConsiderationFactors::ClearConsiderFactors()
{
	TargetHps.Empty();
	TargetDistances.Empty();
	TargetActors.Empty();
}

void UConsiderationFactors::GetConsiderListData()
{
	SearchNearActors();

	MyHp = MyCharacter->GetHealthComponent()->GetHealthNormalized();
}

void UConsiderationFactors::SearchNearActors()
{
	FVector Center = MyCharacter->GetActorLocation();
	float Radius = 5000.f;
	TArray<FOverlapResult> OutOverlaps;

	// 이걸로 Target 찾아서 지정하기
	GetWorld()->OverlapMultiByChannel(OutOverlaps, Center, FQuat::Identity, Battle_TraceChannel_AttackToCharacter, FCollisionShape::MakeSphere(Radius));

	if (!OutOverlaps.IsEmpty())
	{
		ClearConsiderFactors();
		for (FOverlapResult& OverlapResult : OutOverlaps)
		{
			AActor* OverlappedActor = OverlapResult.GetActor();
			if (OverlappedActor)
			{
				if (OverlappedActor == MyCharacter)
				{
					continue;
				}
				if (OverlappedActor->IsA<ABattleCharacterBase>())
				{
					TargetActors.Add(Cast<ABattleCharacterBase>(OverlappedActor));
				}
			}
		}

	}

	float MaxTargetDistance = UtilityAIComponent->MaxTargetDistance;

	
	
	// Target 나온 것을 토대로
	for (ABattleCharacterBase* Character : TargetActors)
	{
		float Distance = FMath::Clamp(FVector::Dist(Character->GetActorLocation(), Center), 0.0f, MaxTargetDistance);
		float TargetHp = Character->GetHealthComponent()->GetHealthNormalized();
		Distance = FMath::GetRangePct(0.0f, MaxTargetDistance, Distance);
		TargetDistances.Add(Distance);
		TargetHps.Add(TargetHp);
	}
}

AActor* UConsiderationFactors::GetTargetPtr(EAxisType InAxisType, int Index) const
{
	if (EAxisType::Target == InAxisType)
	{
		if (TargetActors.IsValidIndex(Index))
		{
			return TargetActors[Index];
		}
	}
	return nullptr;
}

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
		NewAction->InitAxis(ActionConfig.AxisConfigs, this);
		NewAction->SetWeight(ActionConfig.Weight);

		InstancedActions.Add(NewAction);
	}


	ConsiderList->InitConsiderFunction(UtilityAIData);
	
	CollectConsiderFactors();

	StartTimer();

}

void UBattleUtilityAIComponent::PostInitProperties()
{
	Super::PostInitProperties();
	ConsiderList = NewObject<UConsiderationFactors>(this, UConsiderationFactors::StaticClass());
}

void UBattleUtilityAIComponent::CollectConsiderFactors()
{
	ConsiderList->GetConsiderListData();

	UE_LOG(LogBattle, Log, TEXT("Consider Factor"));
	
	for (TTuple<EBattleConsiderType, float>& Factor :ConsiderList->Factors)
	{
		Factor.Value = ConsiderList->Functions[Factor.Key]();
		UE_LOG(LogBattle, Log, TEXT("%s Factor Value: %f"), *EnumToString(Factor.Key), Factor.Value);
	}

	for (TTuple<EBattleConsiderType, TArray<float>>& ArrayFactor : ConsiderList->ArrayFactors)
	{
		ArrayFactor.Value = ConsiderList->ArrayFunctions[ArrayFactor.Key]();
		for (float DebugValue : ArrayFactor.Value)
		{
			UE_LOG(LogBattle, Log, TEXT("%s Factor Value: %f"), *EnumToString(ArrayFactor.Key), DebugValue);
		}
	}
}

PRAGMA_DISABLE_OPTIMIZATION

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

	if (ActiveAction == nullptr)
	{
		ActiveAction = BestAction;
		ActiveAction->StartAction();
		bActionComplete = false;
	}
	else if (bActionComplete)
	{
		// 액션 수행
		ActiveAction->EndAction();

		ActiveAction = BestAction;
		ActiveAction->StartAction();
		bActionComplete = false;

	}
	else if (BestAction != ActiveAction && BestAction->GetPriority() > ActiveAction->GetPriority())
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
	GetWorld()->GetTimerManager().ClearTimer(TimerHandle);
	GetWorld()->GetTimerManager().SetTimer(TimerHandle, this, &UBattleUtilityAIComponent::SelectBestAction, UpdatePeriod, false);
}

PRAGMA_ENABLE_OPTIMIZATION

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
