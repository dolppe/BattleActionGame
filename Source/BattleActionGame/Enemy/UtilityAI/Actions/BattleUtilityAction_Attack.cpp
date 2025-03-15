#include "BattleUtilityAction_Attack.h"

#include "AbilitySystemComponent.h"
#include "BattleActionGame/BattleGameplayTags.h"
#include "BattleActionGame/Character/BattleCharacterBase.h"
#include "BattleActionGame/Combat/BattleCombatManagerComponent.h"
#include "BattleActionGame/Combat/BattleGameplayAbility_Attack_Parent.h"
#include "BattleActionGame/Combat/BattleGameplayAbility_BasicAttack.h"
#include "BattleActionGame/Combat/BattleGameplayAbility_TargetedAttack.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(BattleUtilityAction_Attack)


/*
 *
 *
 *	UBattleUtilityAction_Attack
 *
 *
 */


UBattleUtilityAction_Attack::UBattleUtilityAction_Attack()
{
}

void UBattleUtilityAction_Attack::StartAction()
{
	Super::StartAction();

	ASC = CachedAIComponent->ConsiderList->MyCharacter->GetAbilitySystemComponent();

	if (AgeRate >= 0.0f)
	{
		ScoreMultiplier = ScoreMultiplier * AgeRate;
	}
	
	StartAgeTimer();

	StartAttack();
	
}

bool UBattleUtilityAction_Attack::TickAction(float DeltaTime)
{
	if (ASC)
	{
		if (!ASC->HasMatchingGameplayTag(FBattleGameplayTags::Get().Status_Attack_Attacking))
		{
			return true;
		}
		return false;
	}

	return true;
}

void UBattleUtilityAction_Attack::StartAgeTimer()
{
	FTimerManager& TimerManager = GetWorld()->GetTimerManager();
	
	if (TimerManager.IsTimerActive(TimerHandle))
	{
		float RemainTime = TimerManager.GetTimerRemaining(TimerHandle);

		TimerManager.ClearTimer(TimerHandle);
		TimerManager.SetTimer(TimerHandle, this, &ThisClass::UpdateAge, AgeCycleTime + RemainTime, false);
	}
	else
	{
		TimerManager.SetTimer(TimerHandle, this, &ThisClass::UpdateAge, AgeCycleTime, false);
	}
}

void UBattleUtilityAction_Attack::StartAttack()
{
	
}

void UBattleUtilityAction_Attack::UpdateAge()
{
	ScoreMultiplier = 1.0f;
}


/*
 *
 *
 *	UBattleUtilityAction_AttackSingle
 *
 *
 */





UBattleUtilityAction_AttackSingle::UBattleUtilityAction_AttackSingle()
{
	Priority = 3;
}

void UBattleUtilityAction_AttackSingle::StartAction()
{
	Super::StartAction();
	

	
}

bool UBattleUtilityAction_AttackSingle::TickAction(float DeltaTime)
{
	return Super::TickAction(DeltaTime);
}

void UBattleUtilityAction_AttackSingle::StartAttack()
{
	if (ASC)
	{
		ASC->TryActivateAbilityByClass(GA_Attack);
	}
}

void UBattleUtilityAction_AttackSingle::StartAgeTimer()
{
	Super::StartAgeTimer();
}

void UBattleUtilityAction_AttackSingle::UpdateAge()
{
	ScoreMultiplier = 1.0f;
}


/*
 *
 *
 *	UBattleUtilityAction_AreaAttack
 *
 *
 */





UBattleUtilityAction_AttackArea::UBattleUtilityAction_AttackArea()
{
	Priority = 3;
}


void UBattleUtilityAction_AttackArea::StartAction()
{
	Super::StartAction();
}

bool UBattleUtilityAction_AttackArea::TickAction(float DeltaTime)
{
	return Super::TickAction(DeltaTime);
}

void UBattleUtilityAction_AttackArea::StartAttack()
{
	if (!IsSetAreaData)
	{
		GetAreaData();
		IsSetAreaData = true;
	}
	
	if (ASC)
	{
		if (FGameplayAbilitySpec* AbilitySpec = ASC->FindAbilitySpecFromClass(GA_Attack))
		{
			
			// GA 하나 만들기 (Spot을 담고 있어야하며 전달 받은것으로 처리, Instancing은 Actor마다 있어야함.
			// Target Spot 찾는 함수
			TArray<FVector> AttackAreaData = GetBestSpots();

			if (AttackAreaData.IsEmpty())
			{
				AttackAreaData = GetTargetSpots();
				
				if (AttackAreaData.IsEmpty())
				{
					EndAction();
					return;
				}
			}
			
			// GA->SetTargetData(Target) => 해당 함수 만들어서 Spot 넘겨주기
			if (UGameplayAbility* InstancedAbility = AbilitySpec->GetPrimaryInstance())
			{
				if (UBattleGameplayAbility_TargetedAttack* InstancedGA = Cast<UBattleGameplayAbility_TargetedAttack>(InstancedAbility))
				{
					InstancedGA->SetAreaCenterData(AttackAreaData);
					ASC->TryActivateAbility(AbilitySpec->Handle);
				}
			}
		}
	}
}

void UBattleUtilityAction_AttackArea::StartAgeTimer()
{
	Super::StartAgeTimer();
}


void UBattleUtilityAction_AttackArea::UpdateAge()
{
	ScoreMultiplier = 1.0f;
}

void UBattleUtilityAction_AttackArea::GetAreaData()
{
	if (ASC)
	{
		if (FGameplayAbilitySpec* AbilitySpec = ASC->FindAbilitySpecFromClass(GA_Attack))
		{
			UBattleGameplayAbility_Attack_Parent* GA = Cast<UBattleGameplayAbility_Attack_Parent>(AbilitySpec->Ability);
			int AttackIdx = GA->GetAttackMode();

			if (UBattleCombatManagerComponent* CombatManagerComponent = Cast<UBattleCombatManagerComponent>(CachedAIComponent->ConsiderList->MyCharacter->GetComponentByClass(UBattleCombatManagerComponent::StaticClass())))
			{
				UBattleCombatData* CombatData = CombatManagerComponent->GetAttackData();
				if (CombatData->TargetedAttacks.IsValidIndex(AttackIdx))
				{
					if (UAttackCollisionData_CircularAOE* CircularAoe = Cast<UAttackCollisionData_CircularAOE>(CombatData->TargetedAttacks[AttackIdx].CollisionMethod))
					{
						AreaNum = CircularAoe->AttackNum;
						AreaRadius = CircularAoe->AttackRadius;
					}
				}
			}
		}
	}
}


TArray<FVector> UBattleUtilityAction_AttackArea::GetBestSpots() const
{
	TArray<FVector> Locations;
	
	for (ABattleCharacterBase* Character : CachedAIComponent->ConsiderList->TargetActors)
	{
		FVector CharacterLocation = Character->GetActorLocation();
		CharacterLocation.Z = 0.0f;
		Locations.Add(CharacterLocation);
	}


	if (Locations.Num() <2)
	{
		return TArray<FVector>();
	}
	
	// 점 2개 선택 (조건에 맞는)
	TArray<TPair<FVector,FVector>> SelectedLocations;

	for (int i = 0; i<Locations.Num();i++)
	{
		FVector& StartLocation = Locations[i];
		for (int j =i+1; j <Locations.Num();j++)
		{
			FVector& EndLocation = Locations[j];
			
			float Distance = FVector::Dist(StartLocation, EndLocation);

			if (Distance <= AreaRadius * 2)
			{
				SelectedLocations.Add({StartLocation, EndLocation});
			}
		}
	}

	if (SelectedLocations.IsEmpty())
	{
		return TArray<FVector>();
	}

	ABattleCharacterBase* Character = Cast<ABattleCharacterBase>(CachedAIComponent->GetOwner());
	UBattleCombatManagerComponent* CombatManagerComponent = Cast<UBattleCombatManagerComponent>(Character->GetComponentByClass(UBattleCombatManagerComponent::StaticClass()));

	FAttackData AttackData;
	
	// 선택된 점 세트마다 원 2개 만들기
	TSet<FVector> Centers;

	for (TPair<FVector, FVector>& LocationPair : SelectedLocations)
	{
		FVector& StartVector = LocationPair.Key;
		FVector& EndVector = LocationPair.Value;

		FVector MidVector = (StartVector + EndVector) * 0.5f;

		float HalfDistance = (StartVector - EndVector).Size2D() * 0.5f;

		float MidToCenterDistance = FMath::Sqrt((AreaRadius * AreaRadius) - (HalfDistance * HalfDistance));

		FVector MidUnit = (MidVector - StartVector).GetSafeNormal2D();

		float CenterX1 = MidVector.X + MidToCenterDistance*MidUnit.Y;
		float CenterY1 = MidVector.Y - MidToCenterDistance*MidUnit.X;
		
		float CenterX2 = MidVector.X - MidToCenterDistance*MidUnit.Y;
		float CenterY2 = MidVector.Y + MidToCenterDistance*MidUnit.X;

		FVector NewCenter = FVector(CenterX1, CenterY1, 0.0f);
		FVector NewCenter2 = FVector(CenterX2, CenterY2, 0.0f);

		Centers.Add(NewCenter);
		Centers.Add(NewCenter2);
	}
	
	// 원에 포함되는 점 개수 구하기
	

	const float AreaRadiusSquared = AreaRadius * AreaRadius;

	TArray<TPair<int, FVector>> CalcLocations; 
	int MaxCount = 0;
	
	for (FVector& Center : Centers)
	{
		int Count = 0;
		for (FVector Location : Locations)
		{
			if (FVector::DistSquared2D(Location, Center) <= AreaRadiusSquared)
			{
				Count++;
			}
		}

		CalcLocations.Add({Count, Center});
	}

	CalcLocations.Sort([](const TPair<int, FVector>& A, const TPair<int, FVector>& B)
	{
		return A.Key > B.Key;
	});

	TArray<FVector> Result;
	
	for (int Idx =0;Idx<CalcLocations.Num();Idx++)
	{
		if (Idx >= AreaNum)
		{
			break;
		}
		FVector AreaData = CalcLocations[Idx].Value;
		
		Result.Add(AreaData);
	}

	return Result;
	
}

TArray<FVector> UBattleUtilityAction_AttackArea::GetTargetSpots() const
{
	TArray<FVector> Locations;
	
	for (ABattleCharacterBase* Character : CachedAIComponent->ConsiderList->TargetActors)
	{
		FVector CharacterLocation = Character->GetActorLocation();
		CharacterLocation.Z = 0.0f;
		Locations.Add(CharacterLocation);
	}

	if (Locations.IsEmpty())
	{
		return TArray<FVector>();
	}

	TArray<FVector> Result;

	for (int Idx = 0; Idx<Locations.Num();Idx++)
	{
		if (Idx >= AreaNum)
		{
			break;
		}
		FVector AreaData = Locations[Idx];

		Result.Add(AreaData);
	}

	return Result;

}
