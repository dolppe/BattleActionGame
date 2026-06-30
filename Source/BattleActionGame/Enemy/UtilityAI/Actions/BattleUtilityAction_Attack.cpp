#include "BattleUtilityAction_Attack.h"

#include "AbilitySystemComponent.h"
#include "NavigationSystem.h"
#include "BattleActionGame/BattleGameplayTags.h"
#include "BattleActionGame/Character/BattleCharacterBase.h"
#include "BattleActionGame/Combat/BattleCombatManagerComponent.h"
#include "BattleActionGame/Combat/BattleGameplayAbility_Attack_Parent.h"
#include "BattleActionGame/Combat/BattleGameplayAbility_BasicAttack.h"
#include "BattleActionGame/Combat/BattleGameplayAbility_Special_Spawn.h"
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
	
	bAbilityStart = false;

	ASC = CachedAIComponent->ConsiderList->MyCharacter->GetAbilitySystemComponent();

	if (CachedAIComponent->ConsiderList->SelectedTarget != nullptr)
	{
		if (UBattleCombatManagerComponent* CombatManager = Cast<UBattleCombatManagerComponent>(CachedAIComponent->ConsiderList->MyCharacter->GetComponentByClass(UBattleCombatManagerComponent::StaticClass())))
		{
			CombatManager->SetCurrentTargetActor(CachedAIComponent->ConsiderList->SelectedTarget);
		}
	}

	StartAttack();
	
	bAbilityStart = true;
	
}

void UBattleUtilityAction_Attack::TickAction(float DeltaTime)
{
	if (!bAbilityStart)
	{
		return;
	}
	if (ASC)
	{
		if (AbilitySpec->IsActive())
		{
			return;
		}
	}
	bIsCompletedAction = true;
}


void UBattleUtilityAction_Attack::StartAttack()
{
	
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
	bAbilityStart = false;
			
	if (CachedAIComponent->ConsiderList->SelectedTarget != nullptr)
	{
		FVector ToTargetVector = (CachedAIComponent->ConsiderList->SelectedTarget->GetActorLocation() - CachedAIComponent->ConsiderList->MyCharacter->GetActorLocation()).GetSafeNormal2D();
		CachedAIComponent->ConsiderList->MyCharacter->SetActorRotation(ToTargetVector.Rotation());
	}
	
	Super::StartAction();
	
}

void UBattleUtilityAction_AttackSingle::TickAction(float DeltaTime)
{
	if (!bAbilityStart)
	{
		return;
	}
	if (ASC && AbilitySpec)
	{
		if (AbilitySpec->IsActive())
		{
			return;
		}
	}
	bIsCompletedAction = true;
}

void UBattleUtilityAction_AttackSingle::StartAttack()
{
	if (ASC)
	{
		AbilitySpec = ASC->FindAbilitySpecFromClass(GA_Attack);
		
		if (AbilitySpec)
		{
			ASC->TryActivateAbility(AbilitySpec->Handle);
		}
	}
}

UBattleUtilityAction_TargetChangeAttack::UBattleUtilityAction_TargetChangeAttack()
{
}

void UBattleUtilityAction_TargetChangeAttack::StartAction()
{
	// BestTarget 대상 설정 및 TurnToTarget이후에 진행
	bAbilityStart = false;
	if (!BestTargets.IsEmpty())
	{
		ABattleCharacterBase* SelectedTarget = nullptr;
		for (AActor* BestTarget : BestTargets)
		{
			if (BestTarget != nullptr)
			{
				SelectedTarget = Cast<ABattleCharacterBase>(BestTarget);
				if (SelectedTarget != nullptr)
				{
					break;	
				}
			}
		}
		
		if (SelectedTarget != nullptr)
		{
			CachedAIComponent->ConsiderList->SelectedTarget = SelectedTarget;
			
			FVector ToTargetVector = (SelectedTarget->GetActorLocation() - CachedAIComponent->ConsiderList->MyCharacter->GetActorLocation()).GetSafeNormal2D();
			CachedAIComponent->ConsiderList->MyCharacter->SetActorRotation(ToTargetVector.Rotation());
		}
	}
	
	Super::StartAction();
}

void UBattleUtilityAction_TargetChangeAttack::TickAction(float DeltaTime)
{
	if (!bAbilityStart)
	{
		return;
	}
	if (ASC && AbilitySpec)
	{
		if (AbilitySpec->IsActive())
		{
			return;
		}
	}
	bIsCompletedAction = true;
}

void UBattleUtilityAction_TargetChangeAttack::StartAttack()
{
	if (ASC)
	{
		AbilitySpec = ASC->FindAbilitySpecFromClass(GA_Attack);
		
		if (AbilitySpec)
		{
			ASC->TryActivateAbility(AbilitySpec->Handle);
		}
	}
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

void UBattleUtilityAction_AttackArea::TickAction(float DeltaTime)
{
	if (!bAbilityStart)
	{
		return;
	}
	if (ASC && AbilitySpec)
	{
		if (AbilitySpec->IsActive())
		{
			return;
		}
	}
	bIsCompletedAction = true;
}

void UBattleUtilityAction_AttackArea::StartAttack()
{
	if (ASC)
	{
		AbilitySpec = ASC->FindAbilitySpecFromClass(GA_Attack);
		if (AbilitySpec)
		{
			
			// GA 하나 만들기 (Spot을 담고 있어야하며 전달 받은것으로 처리, Instancing은 Actor마다 있어야함.
			// Target Spot 찾는 함수
			TArray<FVector> TargetLocations;
			for (ABattleCharacterBase* Character : CachedAIComponent->ConsiderList->TargetActors)
			{
				FVector CharacterLocation = Character->GetActorLocation();
				CharacterLocation.Z = 0.0f;
				TargetLocations.Add(CharacterLocation);
			}
			
			UNavigationSystemV1* NavSystem = UNavigationSystemV1::GetCurrent(GetWorld());
			TArray<FVector> AttackAreaData = BattleUtilityActionHelper::FindBestSpotsWithGreedy(NavSystem, TargetLocations, AreaRadius, AreaNum);

			if (AttackAreaData.IsEmpty())
			{
				AttackAreaData = GetTargetSpots();
				
				if (AttackAreaData.IsEmpty())
				{
					EndAction();
					return;
				}
			}
			
			if (UBattleCombatManagerComponent* CombatManagerComponent = Cast<UBattleCombatManagerComponent>(CachedAIComponent->ConsiderList->MyCharacter->GetComponentByClass(UBattleCombatManagerComponent::StaticClass())))
			{
				CombatManagerComponent->SetAreaCenterData(AttackAreaData);
				ASC->TryActivateAbility(AbilitySpec->Handle);
				
			}
		}
	}
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

UBattleUtilityAction_SpawnActor::UBattleUtilityAction_SpawnActor()
{
	Priority = 3;
}

void UBattleUtilityAction_SpawnActor::StartAction()
{
	Super::StartAction();
}

void UBattleUtilityAction_SpawnActor::TickAction(float DeltaTime)
{
	if (!bAbilityStart)
	{
		return;
	}
	if (ASC && AbilitySpec)
	{
		if (AbilitySpec->IsActive())
		{
			return;
		}
	}
	bIsCompletedAction = true;
}


void UBattleUtilityAction_SpawnActor::StartAttack()
{
	if (ASC)
	{
		AbilitySpec = ASC->FindAbilitySpecFromClass(GA_Spawn);
		
		TArray<FVector> TargetLocations;
		for (ABattleCharacterBase* Character : CachedAIComponent->ConsiderList->TargetActors)
		{
			FVector CharacterLocation = Character->GetActorLocation();
			CharacterLocation.Z = 0.0f;
			TargetLocations.Add(CharacterLocation);
		}
			
		UNavigationSystemV1* NavSystem = UNavigationSystemV1::GetCurrent(GetWorld());
		TArray<FVector> SpawnCenterData = BattleUtilityActionHelper::FindBestSpotsWithGreedy(NavSystem, TargetLocations, SpawnAreaRadius, SpawnActorNum);

		if (SpawnCenterData.IsEmpty())
		{
			SpawnCenterData = GetTargetSpots();
			
			if (SpawnCenterData.IsEmpty())
			{
				EndAction();
				return;
			}
		}
		
		// GA->SetTargetData(Target) => 해당 함수 만들어서 Spot 넘겨주기
		if (UGameplayAbility* InstancedAbility = AbilitySpec->GetPrimaryInstance())
		{
			if (UBattleGameplayAbility_Special_Spawn* InstancedGA = Cast<UBattleGameplayAbility_Special_Spawn>(InstancedAbility))
			{
				InstancedGA->SetSpawnCenterDAta(SpawnCenterData);
				ASC->TryActivateAbility(AbilitySpec->Handle);
			}
		}
		
	}
	
}



TArray<FVector> UBattleUtilityAction_SpawnActor::GetTargetSpots() const
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
		if (Idx >= SpawnActorNum)
		{
			break;
		}
		FVector AreaData = Locations[Idx];

		Result.Add(AreaData);
	}

	return Result;

}