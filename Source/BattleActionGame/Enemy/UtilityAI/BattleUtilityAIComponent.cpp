#include "BattleUtilityAIComponent.h"

#include "AbilitySystemComponent.h"
#include "BattleActionGame/Enemy/UtilityAI/Actions/BattleUtilityAction.h"
#include "BattleUtilityAIData.h"
#include "Actions/BattleUtilityAction_Attack.h"
#include "BattleActionGame/BattleGameplayTags.h"
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

float UConsiderationFactors::GetIsTargetInSight()
{
	// 0.5 정면에 딱 맞게 있는것
	// 0.0 왼쪽에 있는 것
	// 1.0 오른쪽에 있는 것
	if (SelectedTarget == nullptr)
	{
		return 0.5f;
	}

	FVector CharacterFwdVector = MyCharacter->GetActorForwardVector();
	FVector ToTargetVector = (SelectedTarget->GetActorLocation() - MyCharacter->GetActorLocation()).GetSafeNormal();

	float CrossProductZ = FVector::CrossProduct(CharacterFwdVector, ToTargetVector).Z;

	if (FMath::IsNearlyZero(CrossProductZ))
	{
		return 0.5f;
	}

	float DotProduct = FVector::DotProduct(CharacterFwdVector, ToTargetVector);

	if (CrossProductZ > 0.0f)
	{
		return 0.5f + (1.0f - DotProduct) * 0.5f;
	}
	else
	{
		return 0.5f - (1.0f - DotProduct) * 0.5f;
	}
	
}

float UConsiderationFactors::GetNearbyEnemyCount()
{
	// 0.4 정도면 가까운 것으로 침 =>

	if (TargetDistances.Num() == 0)
	{
		return 0.f;
	}

	float Count =0.0f;
	
	for (float TargetActorDistance : TargetDistances)
	{
		if (TargetActorDistance < 0.4f)
		{
			Count++;
		}
	}
	
	
	float ReturnValue = Count / ThreatCharacterNum;
	ReturnValue = FMath::Clamp(ReturnValue, 0.0f, 1.0f);

	return ReturnValue;
}

float UConsiderationFactors::GetThreatScore()
{
	// 1 => 위험한 상태
	// NearbyEnemyCount가 높으면 위협이 큼, 싸울만 한 상태가 아니면 위협이 큼
	
	float Result = 1.0f;
	Result *= GetNearbyEnemyCount();
	Result *= 1.0f - GetMyCombatPotential();
	
	return Result;
}

float UConsiderationFactors::GetCombatDuration()
{
	// 전투 초기 혹은 비전투 => 0, BestCombat => 0.5, 
	
	if (!bIsInCombat)
	{
		return 0.0f;
	}
	else
	{
		float CombatTime = GetWorld()->GetTimeSeconds() - CombatStartTime;
		CombatTime = FMath::Clamp(CombatTime/BestCombatTime *0.5, 0.0f, 1.0f);

		return CombatTime;
	}
}

float UConsiderationFactors::GetNearbyWaterAmount()
{
	// 주변의 물이 아예 없으면 0, 주변의 물의 퍼센트에 따라서 값 결정
	return 0.0f;
}

float UConsiderationFactors::GetEnemyDirection()
{
	// 0.5면 중앙, 0.0에 가까울수록 왼쪽에 더 많은 것이고, 1.0으로 가까워질수록 오른쪽에 더 많은 것임
	// 대신 뒤에 있는 Actor는 신경을 안씀
	if (TargetActors.IsEmpty())
	{
		return 0.5f;
	}
	
	FVector ForwardVector = MyCharacter->GetActorForwardVector();
	FVector RightVector = MyCharacter->GetActorRightVector();
	FVector CharacterLocation = MyCharacter->GetActorLocation();

	float RightCount = 0.0f;
	float TotalCount = 0.0f;
	for (ABattleCharacterBase* Target : TargetActors)
	{
		FVector ToTargetVector = (Target->GetActorLocation() - CharacterLocation).GetSafeNormal();

		float DotProduct = FVector::DotProduct(ForwardVector, ToTargetVector);
		if (DotProduct <= 0.0f)
		{
			continue;
		}
		else
		{
			TotalCount++;
			float LeftRight = FVector::DotProduct(RightVector, ToTargetVector);
			if (LeftRight > 0.0f)
			{
				RightCount++;
			}
		}
	}
	if (TotalCount == 0.0f)
		return 0.5f;

	return RightCount / TotalCount;
	
}

float UConsiderationFactors::GetIsAlone()
{
	// 0 => 혼자 아님 1=> 혼자

	return !bIsInCombat;
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
	// Hp가 얼마나 없는지
	// 추후 상태이상에 걸렸는지, Enemy가 주변에 있는지
	// 독에 걸렸으면 1.0f 곱하고, 안걸렸으면 0.7f
	TArray<float> ResultArray;

	for (int Idx =0; Idx<TargetActors.Num();Idx++)
	{
		float Result = 1.0f;
		Result *= (1 - TargetHps[Idx]);

		UAbilitySystemComponent* ASC = TargetActors[Idx]->GetAbilitySystemComponent();

		if (ASC != nullptr && ASC->HasMatchingGameplayTag(FBattleGameplayTags::Get().Status_Poisoned))
		{
			Result *= 0.7f;
		}
		
		ResultArray.Add(Result);
	}
	
	return ResultArray;
}

TArray<float> UConsiderationFactors::GetTargetPoisonedState()
{
	// 0 => 독안걸림 1 => 걸림
	TArray<float> ResultArray;

	for (ABattleCharacterBase* TargetActor : TargetActors)
	{
		if (TargetActor->GetAbilitySystemComponent()->HasMatchingGameplayTag(FBattleGameplayTags::Get().Status_Poisoned))
		{
			ResultArray.Add(1.0f);
		}
		else
		{
			ResultArray.Add(0.0f);
		}
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
	case EBattleConsiderType::IsTargetInSight:
		return [this]() -> float
		{
			return GetIsTargetInSight();
		};
		break;
	case EBattleConsiderType::NearbyEnemyCount:
		return [this]() -> float
		{
			return GetNearbyEnemyCount();
		};
		break;
	case EBattleConsiderType::ThreatScore:
		return [this]() -> float
		{
			return GetThreatScore();
		};
		break;
	case EBattleConsiderType::CombatDuration:
		return [this]() -> float
		{
			return GetCombatDuration();
		};
		break;
	case EBattleConsiderType::NearbyWaterAmount:
		return [this]() -> float
		{
			return GetNearbyWaterAmount();
		};
		break;
	case EBattleConsiderType::EnemyDirection:
		return [this]() -> float
		{
			return GetEnemyDirection();
		};
		break;
	case EBattleConsiderType::IsAlone:
		return [this]() -> float
		{
			return GetIsAlone();
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
	case EBattleConsiderType::TargetPoisonedState:
		return [this]() -> TArray<float>
		{
			return GetTargetPoisonedState();
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
	case EBattleConsiderType::TargetPoisonedState:
		return EAxisType::Target;
	default:
		return EAxisType::Single;
	}
	return EAxisType::Single;
}

void UConsiderationFactors::InitConsiderFunction(const UBattleUtilityAIData* UtilityAIData, UBattleUtilityAIComponent* InUtilityAIComponent)
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

	UtilityAIComponent = InUtilityAIComponent;
	MyCharacter = Cast<ABattleCharacterBase>(UtilityAIComponent->GetOwner());
	BestCombatTime = UtilityAIComponent->BestCombatTime;
	ThreatCharacterNum = UtilityAIComponent->ThreatCharacterNum;
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

	ClearConsiderFactors();
	
	if (!OutOverlaps.IsEmpty())
	{
		
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

	if (TargetActors.IsEmpty() && bIsInCombat == true)
	{
		bIsInCombat = false;
		SelectedTarget = nullptr;
	}
	else if (!TargetActors.IsEmpty() && bIsInCombat == false)
	{
		CombatStartTime = GetWorld()->GetTimeSeconds();
		bIsInCombat = true;
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

	if (GetWorld()->GetNetMode() == NM_Client)
	{
		return;
	}

	for (FActionConfig ActionConfig : UtilityAIData->ActionConfigs)
	{
		UBattleUtilityAction* NewAction = NewObject<UBattleUtilityAction>(this, ActionConfig.ActionClass);
		NewAction->InitAxis(ActionConfig.AxisConfigs, this);
		NewAction->SetWeight(ActionConfig.Weight);

		InstancedActions.Add(NewAction);
	}

	ConsiderList = NewObject<UConsiderationFactors>(GetOwner(), UConsiderationFactors::StaticClass());

	ConsiderList->InitConsiderFunction(UtilityAIData, this);
	
	CollectConsiderFactors();

	StartTimer();

}

void UBattleUtilityAIComponent::PostInitProperties()
{
	Super::PostInitProperties();

}

void UBattleUtilityAIComponent::CollectConsiderFactors()
{
	ConsiderList->GetConsiderListData();
	
	for (TTuple<EBattleConsiderType, float>& Factor :ConsiderList->Factors)
	{
		Factor.Value = ConsiderList->Functions[Factor.Key]();
		//UE_LOG(LogBattle, Log, TEXT("%s Factor Value: %f"), *EnumToString(Factor.Key), Factor.Value);
	}

	for (TTuple<EBattleConsiderType, TArray<float>>& ArrayFactor : ConsiderList->ArrayFactors)
	{
		ArrayFactor.Value = ConsiderList->ArrayFunctions[ArrayFactor.Key]();
		for (float DebugValue : ArrayFactor.Value)
		{
			//UE_LOG(LogBattle, Log, TEXT("%s Factor Value: %f"), *EnumToString(ArrayFactor.Key), DebugValue);
		}
	}
}

PRAGMA_DISABLE_OPTIMIZATION

void UBattleUtilityAIComponent::SelectBestAction()
{

	CollectConsiderFactors();

	UE_LOG(LogBattle, Log, TEXT("SelectBestAction Start"));
	
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
	if (BestAction != nullptr)
	{
		UE_LOG(LogBattle, Log, TEXT("Best Action Is : %s"), *BestAction->GetName());
	}
	
	if (ActiveAction == nullptr)
	{
		
		ActiveAction = BestAction;
		ActiveAction->StartAction();
		bActionComplete = false;
		UE_LOG(LogBattle, Log, TEXT("ActiveAction Is Nullptr => %s Start"), *ActiveAction->GetName());
	}
	else if (bActionComplete)
	{
		// 액션 수행
		ActiveAction->EndAction();

		ActiveAction = BestAction;
		ActiveAction->StartAction();
		bActionComplete = false;

		UE_LOG(LogBattle, Log, TEXT("bActionComplete Is True => %s Start"), *ActiveAction->GetName());
	}
	else if (BestAction != ActiveAction && BestAction->GetPriority() > ActiveAction->GetPriority())
	{
		// 액션 수행
		ActiveAction->EndAction();
		
		ActiveAction = BestAction;
		ActiveAction->StartAction();
		bActionComplete = false;

		UE_LOG(LogBattle, Log, TEXT("Priority Change => %s Start"), *ActiveAction->GetName());
		
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
