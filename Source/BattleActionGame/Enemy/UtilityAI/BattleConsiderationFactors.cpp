#include "BattleConsiderationFactors.h"

#include "AbilitySystemComponent.h"
#include "BattleUtilityAIData.h"
#include "BattleUtilityAxis.h"
#include "NavigationSystem.h"
#include "BattleActionGame/BattleGameplayTags.h"
#include "BattleActionGame/Character/BattleCharacter.h"
#include "BattleActionGame/Character/BattleCharacterBase.h"
#include "BattleActionGame/Character/BattleHealthComponent.h"
#include "BattleActionGame/Navigation/NavArea/BattleNavArea.h"
#include "BattleActionGame/Physics/BattleCollisionChannels.h"
#include "NavMesh/RecastNavMesh.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(BattleConsiderationFactors)

UConsiderationFactors::UConsiderationFactors()
{
}


/*
 *	평가값 계산 함수
 *	Single - Simple 
 */

// [0 ~ 1] 0 => Hp가 0, 1 => Hp가 최대치
float UConsiderationFactors::GetMyHp() const
{
	return MyHp;
}

// [0 or 1] 0 => 선택된 Target이 없음, 1 => 선택된 Target이 존재
float UConsiderationFactors::GetHasTarget() const
{
	return SelectedTarget.IsValid() ? 1.0f : 0.0f;
}

// [0 or 1] 0 => 선택된 Target이 왼쪽에 있음, 1 => 선택된 Target이 오른쪽에 있음
float UConsiderationFactors::GetTargetHorizontalDirection() const
{
	ABattleCharacterBase* Target = SelectedTarget.Get();
	if (!Target)
	{
		return 0.5f;
	}

	const FVector& RightVector = MyCharacter->GetActorRightVector();
	const FVector& ToTargetVector = Target->GetActorLocation() - MyCharacter->GetActorLocation();
	
	float DotProduct = FVector::DotProduct(RightVector, ToTargetVector);
	
	return DotProduct > 0 ? 1.0f : 0.0f;
	
}

// [0 or 1] 0 => 선택된 Target이 후방에 있음, 1 => 선택된 Target이 전방에 있음
float UConsiderationFactors::GetTargetForwardDirection() const
{
	ABattleCharacterBase* Target = SelectedTarget.Get();
	if (!Target)
	{
		return 0.5f;
	}

	const FVector& ForwardVector = MyCharacter->GetActorForwardVector();
	const FVector& ToTargetVector = Target->GetActorLocation() - MyCharacter->GetActorLocation();
	
	float DotProduct = FVector::DotProduct(ForwardVector, ToTargetVector);
	
	return DotProduct > 0 ? 1.0f : 0.0f;

}

// [0 ~ 1] 0 => 선택된 Target이 바로 근처에 있음, 1 => 선택된 Target이 먼 거리에 있음 
float UConsiderationFactors::GetIsFarFromTarget() const
{
	return SelectedTargetDistance;
}

// [0 ~ 1] 0 => 선택된 Target과 나 사이의 장애물이 있음, 1 => 선택된 Target과 나 사이에 장애물이 없음.
float UConsiderationFactors::GetTargetSightClear() const
{
	return bIsSelectedTargetSightClear;
}

// [0 ~ 1] 0 =>  주변에 가까운 적이 없음, 1 => 주변에 가까운 적이 많음
float UConsiderationFactors::GetNearbyEnemyPressure() const
{
	// 적들이 나한테 가까이 있는지 판정
	// 거리는 40%정도면 가까운 것으로 판정하고, 가까운 적이 ThreatCharacterNum보다 적은지 많은지 판단
	if (TargetDistances.Num() == 0)
	{
		return 0.f;
	}

	int32 Count =0;
	
	for (int32 Idx = 0; Idx < TargetActors.Num(); ++Idx)
	{
		if (TargetActors[Idx].IsValid() &&TargetDistances[Idx] < 0.4f)
		{
			++Count;
		}
	}
	
	float ReturnValue = Count / Settings.ThreatCharacterNum;
	ReturnValue = FMath::Clamp(ReturnValue, 0.0f, 1.0f);

	return ReturnValue;
}

// [0 ~ 1] 0 => 비전투, 0.5 => 적정 전투 시간, 1 => 너무 오랫동안 전투 중
float UConsiderationFactors::GetCombatDuration() const
{
	if (!bIsInCombat)
	{
		return 0.0f;
	}
	else
	{
		float CombatTime = CurTime - CombatStartTime;
		CombatTime = FMath::Clamp(CombatTime/Settings.BestCombatTime *0.5, 0.0f, 1.0f);

		return CombatTime;
	}
}

// [0 or 1] 0 => 비전투 중, 1 => 전투 중
float UConsiderationFactors::GetIsInCombat() const
{
	return bIsInCombat;
}

// [0 or 1] 0 => 움직일 수 없음, 1 => 움직일 수 있음
float UConsiderationFactors::GetCanMove() const
{
	if (!MyCharacter->GetAbilitySystemComponent())
	{
		return 1.0f;
	}
	if (MyCharacter->GetAbilitySystemComponent()->HasMatchingGameplayTag(FBattleGameplayTags::Get().Status_Groggy))
	{
		return 0.0f;
	}
	
	return 1.0f;
}

// [0 or 1] 0 => 공격할 수 없음, 1 => 공격할 수 있음
float UConsiderationFactors::GetCanAttack() const
{
	if (!MyCharacter->GetAbilitySystemComponent())
	{
		return 1.0f;
	}
	if (MyCharacter->GetAbilitySystemComponent()->HasMatchingGameplayTag(FBattleGameplayTags::Get().Status_Groggy))
	{
		return 0.0f;
	}
	
	return 1.0f;
}

// [0 ~ 1] 0 => 적들이 밀집해 있음, 1 => 적들이 흩어져 있음
float UConsiderationFactors::GetEnemySpread() const
{
	return EnemySpread;
}

// [0 ~ 1] 0 => 적들과의 평균 거리가 가까움, 1 => 적들과의 평균 거리가 멈
float UConsiderationFactors::GetEnemyAverageDistance() const
{
	return EnemyAverageDistance;
}

// [0 or 1] 0 => 주변에 Cave가 없음, 1 => 주변에 Cave가 있음
float UConsiderationFactors::GetNearByCave() const
{
	if ((TotalAreaFlags & AREA_Cave) != 0 )
	{
		return 1.0f;
	}
	return 0.0f;
}

// [0 or 1] 0 => 주변에 Desert가 없음, 1 => 주변에 Desert가 있음
float UConsiderationFactors::GetNearByDesert() const
{
	if ((TotalAreaFlags & AREA_Desert) != 0 )
	{
		return 1.0f;
	}
	return 0.0f;
}

// [0 or 1] 0 => 주변에 Ruin이 없음, 1 => 주변에 Ruin이 있음
float UConsiderationFactors::GetNearByRuin() const
{
	if ((TotalAreaFlags & AREA_Ruin) != 0 )
	{
		return 1.0f;
	}
	return 0.0f;
}

// [0 or 1] 0 => 주변에 좋은 Spot이 없음, 1 => 주변에 좋은 Spot이 있음
float UConsiderationFactors::GetNearByBestCombatSpot() const
{
	if ((TotalAreaFlags & AREA_Corner) != 0  || (TotalAreaFlags & AREA_HighArea) != 0)
	{
		return 1.0f;
	}
	return 0.0f;
}

// [0 ~ 1] 0 => 적들에게 둘러 쌓여있지 않음, 1 => 적들에게 사방으로 둘러 쌓여있음
float UConsiderationFactors::GetSurroundedRisk() const
{
	FVector ActorLocation = MyCharacter->GetActorLocation();
	FVector ForwardVector = MyCharacter->GetActorForwardVector();

	bool bHasForwardTarget = false;
	bool bHasRightTarget = false;
	bool bHasLeftTarget = false;
	bool bHasBackTarget = false;
	int32 ResultCount = 0;
	
	for (const TWeakObjectPtr<ABattleCharacterBase>& TargetPtr : TargetActors)
	{
		ABattleCharacterBase* TargetActor = TargetPtr.Get();
		if (!TargetActor)
		{
			continue;
		}
		FVector DirectionToTarget = (TargetActor->GetActorLocation() - ActorLocation).GetSafeNormal2D();
		if (DirectionToTarget.IsNearlyZero())
		{
			continue;
		}

		// arc cos로 각도를 구함 (0~180)
		float AngleRad = FMath::Acos(FVector::DotProduct(ForwardVector, DirectionToTarget));
		float AngleDeg = FMath::RadiansToDegrees(AngleRad);

		// 방향은 외적을 통해 구함
		float Cross = FVector::CrossProduct(ForwardVector, DirectionToTarget).Z;
		float FinalAngle = (Cross >= 0.f) ? AngleDeg : -AngleDeg;

		if (!bHasForwardTarget && FinalAngle >= -45.f && FinalAngle <= 45.f)
		{
			bHasForwardTarget = true;
			ResultCount++;
		}
		else if (!bHasRightTarget && FinalAngle > 45.f && FinalAngle <= 135.f)
		{
			bHasRightTarget = true;
			ResultCount++;
		}
		else if (!bHasLeftTarget && FinalAngle < -45.f && FinalAngle >= -135.f)
		{
			bHasLeftTarget = true;
			ResultCount++;
		}
		else if (!bHasBackTarget && (FinalAngle < -135.f || FinalAngle > 135.f))
		{
			bHasBackTarget = true;
			ResultCount++;
		}
		if (ResultCount == 4)
		{
			break;
		}
	}

	return ResultCount * 0.25f;	
}

// [0 or 1] 0 => 현재 자리가 좋음, 1 => 현재 자리가 별로임
float UConsiderationFactors::GetPositionalDisadvantage() const
{
	if ((MyCharacterAreaFlag & AREA_Corner) != 0 || (MyCharacterAreaFlag & AREA_HighArea) != 0)
	{
		return 0.0f;
	}

	return 1.0f;
}

// 미구현 => 내가 독을 얼마나 가지고 있는지
float UConsiderationFactors::GetPoisonAmount() const
{
	return PoisonAmount;
}

// 미구현 => 내가 전기를 얼마나 가지고 있는지
float UConsiderationFactors::GetElectricityAmount() const
{
	return ElectricAmount;
}

/*
 *	평가값 계산 함수
 *	Single - Complex
 */

// [0 ~ 1] 0 => 전투할 여력이 없음, 1 => 만전의 상태
// 전투 가능한 상태인지를 나타냄
float UConsiderationFactors::GetMyCombatPotential() const
{
	// 기본값은 체력 기반
	float Result = GetMyHp();
	
	Result *= FMath::Lerp(1.15f, 0.8f, GetCombatDuration());
	
	Result *= GetPositionalDisadvantage() ? 0.9f : 1.1f;
	
	if (GetPoisonAmount() > 0.8)
	{
		Result *= 1.1f;
	}
	else if (GetPoisonAmount() < 0.2f)
	{
		Result *= 0.8;
	}
	if (GetElectricityAmount() > 0.8)
	{
		Result *= 1.1f;
	}
	else if (GetElectricityAmount() < 0.2f)
	{
		Result *= 0.8;
	}
	
	Result = FMath::Clamp(Result, 0.0f, 1.0f);
	
	return Result;
	
}

// [0 ~ 1] 0 => 위협이 전혀 없음, 1 => 많이 위험한 상태
// 내가 받고 있는 위협 정도를 의미
float UConsiderationFactors::GetThreatScore() const
{
	// 현재 내 상태가 위험한지 판단.
	// 1 => 위험한 상태
	// 기본값은 주변에 가까운 적들이 얼마나 있는지를 기반으로 함
	float Result = GetNearbyEnemyPressure();
	
	Result *= FMath::Lerp(0.9f, 1.1f, GetCombatDuration());
	
	Result *= GetPositionalDisadvantage() ? 1.2f : 0.8f;
	
	Result *= GetTargetSightClear() ? 0.9f : 1.0f;
	
	float SurroundedRisk = GetSurroundedRisk();
	
	if (SurroundedRisk == 1.0f)
	{
		Result *= 1.25f;
	}
	else if (SurroundedRisk == 0.75f)
	{
		Result *= 1.1f;
	}
	else if (SurroundedRisk == 0.25f)
	{
		Result *= 0.8f;
	}
	else if (SurroundedRisk == 0.0f)
	{
		Result *= 0.6f;
	}
	
	Result = FMath::Clamp(Result, 0.0f, 1.0f);
	
	return Result;
}

// [0 ~ 1] 0 => 현재 자리가 가장 좋음, 1 => 현재 자리가 매우 안좋음
// 전투 자리 재조정이 필요한지를 의미
float UConsiderationFactors::GetNeedCombatReposition() const
{
	float PositionDisadvantage = GetPositionalDisadvantage();

	if (PositionDisadvantage == 0.0f)
	{
		return 0.0f;
	}
	float SurroundedValue = GetSurroundedRisk();
	
	SurroundedValue = (SurroundedValue >= 0.75f) ? SurroundedValue*1.25f : SurroundedValue;
	
	float Result = 0.2f + SurroundedValue * 0.8f;
	
	
	if (GetNearbyEnemyPressure() >= 0.5f)
	{
		Result *=1.1f; 
	}

	Result = FMath::Clamp(Result, 0.0f,1.0f);

	return Result;	
}

// [0 ~ 1] 0 => 지역을 이동할 필요가 없음, 1 => 지역 이동을 해야함
// 지역 이동이 필요한지를 의미
float UConsiderationFactors::GetNeedCombatAreaChange() const
{
	float CombatDuration = GetCombatDuration();
	
	if (CombatDuration >= 0.8f)
	{
		CombatDuration *= 1.2f;
	}
	else if (CombatDuration >= 0.5f)
	{
		CombatDuration *= 1.1f;
	}
	
	float Result = CombatDuration;
	
	float MyStateScore = GetThreatScore() *0.3f + (1-GetMyCombatPotential())*0.7f; 

	Result *= FMath::Lerp(0.7f,1.3f,MyStateScore);
	
	if (GetPoisonAmount() < 0.2f || GetElectricityAmount() < 0.2f)
	{
		Result *= 1.1f;
	}

	Result = FMath::Clamp(Result, 0.0f,1.0f);

	return Result;
}

/*
 *	여러 후보별 평가값을 계산하는 함수
 *	Multi - Target - Simple
 *	
 *	주변 적들에 대한 것.
 */


// [0 ~ 1] 0 => 타겟과의 거리가 가까움, 1 => 타겟과의 거리가 멈 
TArray<float> UConsiderationFactors::GetTargetDistance() const
{
	return TargetDistances;
}

// [0 ~ 1] 0 => 타겟의 Hp가 낮음, 1 => 타겟의 Hp가 높음
TArray<float> UConsiderationFactors::GetTargetHp() const
{
	return TargetHps;
}

// [0 ~ 1] 0 => 독에 안걸린 상태, 1 => 독에 걸린 상태
TArray<float> UConsiderationFactors::GetTargetPoisonedState() const
{
	// 0 => 독안걸림 1 => 걸림
	TArray<float> ResultArray;
	ResultArray.Reserve(TargetActors.Num());

	for (const TWeakObjectPtr<ABattleCharacterBase>& TargetPtr : TargetActors)
	{
		ABattleCharacterBase* TargetActor = TargetPtr.Get();
		
		if (!TargetActor || !TargetActor->GetAbilitySystemComponent())
		{
			ResultArray.Add(0.0f);
			continue;
		}
		
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

/*
 *	여러 후보별 평가값을 계산하는 함수
 *	Multi - Target - Complex
 *	
 *	주변 적들에 대한 것.
 */

// [0 ~ 1] 0 => 타겟의 우선순위가 낮음, 1 => Target의 우선순위가 높음
// 타겟들에 대한 우선순위를 나타냄
TArray<float> UConsiderationFactors::GetTargetPriority() const
{
	// 0 => 중요치 않음, 1 => 중요함
	// Character면 높게, NPC면 낮게
	// Distance가 멀면 낮게, 가까우면 높게
	// 추후에는 데미지를 얼마나 넣었는지 같은 AI 입장에서 거슬리는 것들 (뭉쳐있는것도)

	TArray<float> ResultArray;
	ResultArray.Reserve(TargetActors.Num());
	
	for (int32 Idx =0; Idx<TargetActors.Num();Idx++)
	{
		ABattleCharacterBase* TargetActor = TargetActors[Idx].Get();
		if (!TargetActor)
		{
			ResultArray.Add(0.0f);
			continue;
		}
		
		float Result = (1 - TargetDistances[Idx]);

		if (TotalDamage > 0.0f)
		{
			if (const float* Damage = DamageStack.Find(TargetActor))
			{
				float DamageRate = *Damage / TotalDamage;
				Result *= FMath::Lerp(1.0f, 1.5f, DamageRate);
			}
		}

		if (TargetHps[Idx] <= 0.3f)
		{
			Result *= 1.2f;
		}
		
		Result = FMath::Clamp(Result, 0.0f, 1.0f);
		
		ResultArray.Add(Result);
	}

	return ResultArray;
}

// [0 ~ 1] 0 => 타겟이 건강함, 1 => 타겟이 약해져있음
// 타겟들의 상태가 어떤지를 나타냄
TArray<float> UConsiderationFactors::GetTargetWeakness() const
{
	TArray<float> ResultArray;
	ResultArray.Reserve(TargetActors.Num());

	for (int32 Idx =0; Idx<TargetActors.Num();Idx++)
	{
		ABattleCharacterBase* TargetActor = TargetActors[Idx].Get();
		
		if (!TargetActor)
		{
			ResultArray.Add(0.0f);
			continue;
		}
		
		float Result = (1.0f - TargetHps[Idx]);

		UAbilitySystemComponent* ASC = TargetActor->GetAbilitySystemComponent();

		if (ASC != nullptr && ASC->HasMatchingGameplayTag(FBattleGameplayTags::Get().Status_Poisoned))
		{
			Result *= 1.2f;
		}
		
		if (TargetDistances[Idx] >= 0.8f)
		{
			Result *= 0.8f;
		}
		
		Result = FMath::Clamp(Result, 0.0f, 1.0f);
		
		ResultArray.Add(Result);
	}
	
	return ResultArray;
}



TFunction<float()> UConsiderationFactors::GetConsiderFunction(EBattleConsiderType ConsiderType) const
{
	switch (ConsiderType)
	{
	case EBattleConsiderType::HasTarget:
		return [this]() -> float
		{
			return GetHasTarget();
		};
	case EBattleConsiderType::MyHp:
		return [this]() -> float
		{
			return GetMyHp();
		};
	case EBattleConsiderType::MyCombatPotential:
		return [this]() -> float
		{
			return GetMyCombatPotential();
		};
	case EBattleConsiderType::TargetHorizontalDirection:
		return [this]() -> float
		{
			return GetTargetHorizontalDirection();
		};
	case EBattleConsiderType::TargetForwardDirection:
		return [this]() -> float
		{
			return GetTargetForwardDirection();
		};
	case EBattleConsiderType::NearbyEnemyPressure:
		return [this]() -> float
		{
			return GetNearbyEnemyPressure();
		};
	case EBattleConsiderType::ThreatScore:
		return [this]() -> float
		{
			return GetThreatScore();
		};
	case EBattleConsiderType::CombatDuration:
		return [this]() -> float
		{
			return GetCombatDuration();
		};
	case EBattleConsiderType::IsInCombat:
		return [this]() -> float
		{
			return GetIsInCombat();
		};
	case EBattleConsiderType::CanMove:
		return [this]() -> float
		{
			return GetCanMove();
		};
	case EBattleConsiderType::EnemySpread:
		return [this]() -> float
		{
			return GetEnemySpread();
		};
	case EBattleConsiderType::EnemyAverageDistance:
		return [this]() -> float
		{
			return GetEnemyAverageDistance();
		};
	case EBattleConsiderType::IsFarFromTarget:
		return [this]() -> float
		{
			return GetIsFarFromTarget();
		};
	case EBattleConsiderType::PoisonAmount:
		return [this]() -> float
		{
			return GetPoisonAmount();
		};
	case EBattleConsiderType::ElectricityAmount:
		return [this]() -> float
		{
			return GetElectricityAmount();
		};
	case EBattleConsiderType::NearByCave:
		return [this]() -> float
		{
			return GetNearByCave();
		};
	case EBattleConsiderType::NearByDesert:
		return [this]() -> float
		{
			return GetNearByDesert();
		};
	case EBattleConsiderType::NearByRuin:
		return [this]() -> float
		{
			return GetNearByRuin();
		};
	case EBattleConsiderType::NearByBestCombatSpot:
		return [this]() -> float
		{
			return GetNearByBestCombatSpot();
		};
	case EBattleConsiderType::NeedCombatAreaChange:
		return [this]() -> float
		{
			return GetNeedCombatAreaChange();
		};
	case EBattleConsiderType::NeedCombatReposition:
		return [this]() -> float
		{
			return GetNeedCombatReposition();
		};
	case EBattleConsiderType::SurroundedRisk:
		return [this]() -> float
		{
			return GetSurroundedRisk();
		};
	case EBattleConsiderType::PositionalDisadvantage:
		return [this]() -> float
		{
			return GetPositionalDisadvantage();
		};
	case EBattleConsiderType::CanAttack:
		return [this]() -> float
		{
			return GetCanAttack();
		};
	case EBattleConsiderType::TargetSightClear:
		return [this]() -> float
		{
			return GetTargetSightClear();
		};
	default:
		return [this]() -> float
		{
			return 0.0f;
		};		
	}
}


TFunction<TArray<float>()> UConsiderationFactors::GetMultiConsiderFunction(EBattleConsiderType ConsiderType) const
{
	switch (ConsiderType)
	{
	case EBattleConsiderType::TargetDistance:
		return [this]() -> TArray<float>
		{
			return GetTargetDistance();
		};
	case EBattleConsiderType::TargetHp:
		return [this]() -> TArray<float>
		{
			return GetTargetHp();
		};
	case EBattleConsiderType::TargetPriority:
		return [this]() -> TArray<float>
		{
			return GetTargetPriority();
		};
	case EBattleConsiderType::TargetWeakness:
		return [this]() -> TArray<float>
		{
			return GetTargetWeakness();
		};
	case EBattleConsiderType::TargetPoisonedState:
		return [this]() -> TArray<float>
		{
			return GetTargetPoisonedState();
		};
	default:
		return [this]() -> TArray<float>
		{
			return {0.0f};
		};		
	}
}

void UConsiderationFactors::SetSelectedTarget(ABattleCharacterBase* InSelectedTarget)
{
	SelectedTarget = InSelectedTarget;
}

EAxisType UConsiderationFactors::GetAxisType(EBattleConsiderType ConsiderType)
{
	switch (ConsiderType)
	{
	case EBattleConsiderType::TargetHp:
	case EBattleConsiderType::TargetDistance:
	case EBattleConsiderType::TargetPriority:
	case EBattleConsiderType::TargetWeakness:
	case EBattleConsiderType::TargetPoisonedState:
		return EAxisType::Target;
	default:
		return EAxisType::Single;
	}
}

void UConsiderationFactors::InitConsiderFunction(const UBattleUtilityAIData* InUtilityAIData, AActor* InOwner, const FConsiderationSettings& InSettings)
{
	if (InUtilityAIData == nullptr || InOwner == nullptr)
	{
		return;
	}
	
	// UtilityAIData에 등록된 ConsiderType에 대해서만 Factor를 만들어둠
	for (EBattleConsiderType ConsiderType: InUtilityAIData->Consider)
	{
		if (GetAxisType(ConsiderType) == EAxisType::Single)
		{
			FactorCalculators.Add(ConsiderType,GetConsiderFunction(ConsiderType));
			Factors.Add(ConsiderType, 0.0f);
		}
		else
		{
			MultiFactorCalculators.Add(ConsiderType,GetMultiConsiderFunction(ConsiderType));
			MultiFactors.Add(ConsiderType,{0.0f});
		}
	}
	
	Settings = InSettings;
	
	MyCharacter = Cast<ABattleCharacterBase>(InOwner);
	
	if (MyCharacter == nullptr)
	{
		return;
	}
	
	if (UBattleHealthComponent* HealthComponent = MyCharacter->GetHealthComponent())
	{
		HealthComponent->OnHealthChanged.AddDynamic(this, &ThisClass::OnCharacterHealthChanged);
	}
}

void UConsiderationFactors::ClearNearActorsData()
{
	TargetHps.Reset();
	TargetDistances.Reset();
	TargetActors.Reset();
	
	EnemySpread = 0.0f;
	EnemyAverageDistance = 0.0f;
	SelectedTargetDistance = 0.0f;
	bIsSelectedTargetSightClear = false;
}

void UConsiderationFactors::CalcConsiderationFactors()
{
	// EBattleConsiderType: 체력, 전투 지속 시간 등 평가 항목의 종류
	// FactorCalculators: 평가 항목별 계산 함수, Factors: 계산한 평가값 저장
	
	// 각 Factor별로 저장된 계산 함수를 호출하여 값 계산 후 저장
	for (TTuple<EBattleConsiderType, float>& Factor : Factors)
	{
		Factor.Value = FactorCalculators[Factor.Key]();
	}

	// 후보별 평가값을 배열로 계산하여 저장
	for (TTuple<EBattleConsiderType, TArray<float>>& MultiFactor : MultiFactors)
	{
		MultiFactor.Value = MultiFactorCalculators[MultiFactor.Key]();
	}
}

float UConsiderationFactors::GetFactorValue(EBattleConsiderType ConsiderType) const
{
	const float* Value = Factors.Find(ConsiderType);
	return Value ? *Value : 0.0f;
}

const TArray<float>* UConsiderationFactors::GetMultiFactorValues(EBattleConsiderType ConsiderType) const
{
	return MultiFactors.Find(ConsiderType);
}

void UConsiderationFactors::CollectConsiderationData()
{
	if (!IsValid(MyCharacter))
	{
		return;
	}
	MyHp = MyCharacter->GetHealthComponent()->GetHealthNormalized();
	CurTime = GetWorld()->GetTimeSeconds();
	
	// 주변 Actor에 대한 정보 수집
	SearchNearActors();

	// 주변 Spot에 대한 정보 수집
	SearchNearSpots();
}

void UConsiderationFactors::SearchNearSpots()
{
	TotalAreaFlags = 0;
	MyCharacterAreaFlag = 0;
	BestSpotLocation = FVector(NAN, NAN, NAN);
	
	UNavigationSystemV1* NavSystem = FNavigationSystem::GetCurrent<UNavigationSystemV1>(GetWorld());
	
	if (!NavSystem)
	{
		return;
	}
	
	ARecastNavMesh* NavMesh = Cast<ARecastNavMesh>(NavSystem->GetDefaultNavDataInstance());
	
	if (!NavMesh)
	{
		return;
	}
	
	// 폴리곤의 Area ID에 해당하는 NavArea 클래스 CDO를 반환
	const auto FindArea = [NavMesh](NavNodeRef PolyRef) -> const UNavArea*
	{
		if (PolyRef == 0)
		{
			return nullptr;
		}

		const UClass* AreaClass = NavMesh->GetAreaClass(NavMesh->GetPolyAreaID(PolyRef));

		return AreaClass ? Cast<UNavArea>(AreaClass->GetDefaultObject()) : nullptr;
	};
	
	FVector CharacterLocation = MyCharacter->GetActorLocation();
	const FBox SearchBox(CharacterLocation - Settings.SpotSearchExtent, CharacterLocation + Settings.SpotSearchExtent);
	
	// 주변의 NavMesh 폴리곤 수집
	TArray<FNavPoly> NearbyPolys;
	NavMesh->GetPolysInBox(SearchBox, NearbyPolys);
	
	FVector BestLocation = FVector(NAN,NAN,NAN);
	FVector AlternativeLocation = FVector(NAN,NAN,NAN);
	bool bFindBestLocation = false;
	
	for (const FNavPoly& Poly : NearbyPolys)
	{
		const UNavArea* Area = FindArea(Poly.Ref);
		if (!Area)
		{
			continue;
		}
		
		const int32 Flags = Area->GetAreaFlags();
		TotalAreaFlags |= Flags;
		
		if (bFindBestLocation)
		{
			continue;
		}

		// 처음 발견한 코너나 높은 지형을 선택함
		if ((Flags & (AREA_Corner | AREA_HighArea)) != 0)
		{
			BestLocation = Poly.Center;
			bFindBestLocation = true;
		}
		else if ((Flags & AREA_LowArea) == 0)
		{
			// 낮은 지형이 아닌 곳을 대안으로 저장
			AlternativeLocation = Poly.Center;
		}
	}
	
	BestSpotLocation = bFindBestLocation ? BestLocation : AlternativeLocation;
	
	// 캐릭터 주변의 가장 가까운 폴리곤으로 지형 특성 가져옴
	const NavNodeRef NearestPoly = NavMesh->FindNearestPoly(CharacterLocation, FVector(1000.f));
	const UNavArea* CurrentArea = FindArea(NearestPoly);
	MyCharacterAreaFlag = CurrentArea ? CurrentArea->GetAreaFlags() : 0;
}

void UConsiderationFactors::SearchNearActors()
{
	FVector MyLocation = MyCharacter->GetActorLocation();
	
	ClearNearActorsData();
	
	// 주변의 PlayerCharacter를 가져옴 
	TArray<FOverlapResult> OutOverlaps;
	GetWorld()->OverlapMultiByChannel(OutOverlaps, MyLocation, FQuat::Identity, Battle_TraceChannel_AttackToCharacter, FCollisionShape::MakeSphere(Settings.ActorSearchRadius));
	
	for (FOverlapResult& Overlap : OutOverlaps)
	{
		ABattleCharacter* Target = Cast<ABattleCharacter>(Overlap.GetActor());
		
		if (!IsValid(Target) || Target == MyCharacter)
		{
			continue;
		}
		TargetActors.AddUnique(Target);
	}
	
	// 주변에 적이 없으면 비전투로 전환
	if (TargetActors.IsEmpty())
	{
		bIsInCombat = false;
		SelectedTarget = nullptr;
		return;
	}
	
	// 비전투인데 적을 발견하면 전투로 전환
	if (!bIsInCombat)
	{
		CombatStartTime = CurTime;
		bIsInCombat = true;
	}
	
	const int32 TargetCount = TargetActors.Num();
	
	TargetDistances.Reserve(TargetCount);
	TargetHps.Reserve(TargetCount);
	
	TArray<float> TargetAngles;
	TargetAngles.Reserve(TargetCount);

	float TotalDistance = 0.0f;
	float DirectionSumX = 0.0f;
	float DirectionSumY = 0.0f;
	
	for (const TWeakObjectPtr<ABattleCharacterBase>& Target : TargetActors)
	{
		ABattleCharacterBase* TargetActor = Target.Get();
		if (!TargetActor)
		{
			TargetDistances.Add(0.0f);
			TargetHps.Add(0.0f);
			continue;
		}
		
		const FVector Offset = TargetActor->GetActorLocation() - MyLocation;
		const float Distance = FMath::Clamp(FMath::GetRangePct(0.0f, Settings.MaxTargetDistance, Offset.Size()), 0.0f, 1.0f);
		
		TotalDistance += Distance;
		
		TargetDistances.Add(Distance);
		TargetHps.Add(TargetActor->GetHealthComponent()->GetHealthNormalized());
		
		if (!Offset.IsNearlyZero())
		{
			// 적의 방향각을 구해둠
			const float Angle = FMath::Atan2(Offset.Y, Offset.X);
			TargetAngles.Add(Angle);
		
			// 평균 방향각을 측정하기 위함
			DirectionSumX += FMath::Cos(Angle);
			DirectionSumY += FMath::Sin(Angle);
		}
	}
	
	// 평균 방향각 계산
	const float AverageAngles = FMath::Atan2(DirectionSumY, DirectionSumX);
	
	// 편차를 가중 평균하여 방향 분포값 계산
	float WeightedSpreadSum = 0.0f;
	float TotalWeight = 0.0f;
	
	for (const float Angle : TargetAngles)
	{
		// 편차를 원하는 범위에 맞춤
		const float DirectionDeviation = 1.0f - FMath::Cos(Angle - AverageAngles);
			
		// 편차가 큰 대상의 영향을 줄임
		const float DeviationWeight = 1.0f / (1.0f + DirectionDeviation);
		WeightedSpreadSum += DeviationWeight * DirectionDeviation;
		TotalWeight += DeviationWeight;
	}
	
	EnemySpread = TotalWeight > 0.0f ? WeightedSpreadSum / TotalWeight : 1.0f;
	EnemyAverageDistance = TotalDistance / TargetCount;
	
	if (ABattleCharacterBase* TargetActor = SelectedTarget.Get())
	{
		const FVector TargetLocation = TargetActor->GetActorLocation();
		
		float Distance = FVector::Dist(TargetLocation, MyLocation);
		SelectedTargetDistance = FMath::Clamp(FMath::GetRangePct(0.0f, Settings.MaxTargetDistance, Distance), 0.0f, 1.0f);
	
		FHitResult HitResult;
	
		FCollisionQueryParams Params;
		Params.AddIgnoredActor(MyCharacter);
		bIsSelectedTargetSightClear = !GetWorld()->LineTraceSingleByChannel(HitResult, MyLocation, TargetLocation, Battle_TraceChannel_WorldVisible,Params);
	}
	
}



void UConsiderationFactors::OnCharacterHealthChanged(UBattleHealthComponent* HealthComponent, float OldValue,
	float NewValue, AActor* Instigator)
{
	if (OldValue <= NewValue)
	{
		return;
	}
	
	ABattleCharacterBase* InstigatorCharacter = Cast<ABattleCharacterBase>(Instigator);
	
	if (InstigatorCharacter == nullptr)
	{
		return;
	}
	
	const float Damage = OldValue - NewValue;
	
	if (DamageStack.Contains(InstigatorCharacter))
	{
		DamageStack[InstigatorCharacter] += Damage;
	}
	else
	{
		DamageStack.Add(InstigatorCharacter, Damage);
	}
	
	TotalDamage += Damage;
}

AActor* UConsiderationFactors::GetCandidatePtr(EAxisType InAxisType, int32 Index) const
{
	if (EAxisType::Target == InAxisType)
	{
		if (TargetActors.IsValidIndex(Index))
		{
			return TargetActors[Index].Get();
		}
	}
	return nullptr;
}

int32 UConsiderationFactors::GetCandidateArrNum(EAxisType InAxisType) const
{
	if (EAxisType::Target == InAxisType)
	{
		return TargetActors.Num();
	}
	
	return 0;
}

