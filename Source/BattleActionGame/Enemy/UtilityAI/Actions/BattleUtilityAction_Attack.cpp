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
#include "BattleActionGame/Enemy/UtilityAI/BattleConsiderationFactors.h"
#include "BattleActionGame/Enemy/UtilityAI/BattleUtilityAxis.h"

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
	
	UConsiderationFactors* ConsiderList = ConsiderationFactors.Get();
	
	if (!IsValid(ConsiderList))
	{
		bIsCompletedAction = true;
		return;
	}
	
	ABattleCharacterBase* MyCharacter = ConsiderList->GetMyCharacter();
	ABattleCharacterBase* SelectedTarget = ConsiderList->GetSelectedTarget();
	
	if (!IsValid(MyCharacter) || !IsValid(SelectedTarget))
	{
		bIsCompletedAction = true;
		return;
	}
	
	bAbilityStart = false;

	ASC = MyCharacter->GetAbilitySystemComponent();
	
	if (!IsValid(ASC))
	{
		bIsCompletedAction = true;
		return;
	}

	if (SelectedTarget != nullptr)
	{
		if (UBattleCombatManagerComponent* CombatManager = Cast<UBattleCombatManagerComponent>(MyCharacter->GetComponentByClass(UBattleCombatManagerComponent::StaticClass())))
		{
			CombatManager->SetCurrentTargetActor(SelectedTarget);
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
}

void UBattleUtilityAction_AttackSingle::StartAction()
{
	UConsiderationFactors* ConsiderList = ConsiderationFactors.Get();
	
	if (!IsValid(ConsiderList))
	{
		bIsCompletedAction = true;
		return;
	}
	
	ABattleCharacterBase* MyCharacter = ConsiderList->GetMyCharacter();
	ABattleCharacterBase* SelectedTarget = ConsiderList->GetSelectedTarget();
	
	if (!IsValid(MyCharacter) || ! IsValid(SelectedTarget))
	{
		bIsCompletedAction = true;
		return;
	}
	
	bAbilityStart = false;
	
	FVector ToTargetVector = (SelectedTarget->GetActorLocation() - MyCharacter->GetActorLocation()).GetSafeNormal2D();
	MyCharacter->SetActorRotation(ToTargetVector.Rotation());
	
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
	UConsiderationFactors* ConsiderList = ConsiderationFactors.Get();
	
	if (!IsValid(ConsiderList))
	{
		bIsCompletedAction = true;
		return;
	}
	
	ABattleCharacterBase* MyCharacter = ConsiderList->GetMyCharacter();
	
	if (!IsValid(MyCharacter))
	{
		bIsCompletedAction = true;
		return;
	}
	// BestTarget 대상 설정 및 TurnToTarget이후에 진행
	bAbilityStart = false;
	
	AActor* SelectedTarget = BestTargets[EAxisType::Target];
	
	if (!IsValid(SelectedTarget))
	{
		bIsCompletedAction = true;
		return;
	}
	if (ABattleCharacterBase* SelectedCharacter = Cast<ABattleCharacterBase>(SelectedTarget))
	{
		ConsiderList->SetSelectedTarget(SelectedCharacter);	
	}
	
	FVector ToTargetVector = (SelectedTarget->GetActorLocation() - MyCharacter->GetActorLocation()).GetSafeNormal2D();
	MyCharacter->SetActorRotation(ToTargetVector.Rotation());
	
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
	UConsiderationFactors* ConsiderList = ConsiderationFactors.Get();
	
	if (!IsValid(ConsiderList))
	{
		bIsCompletedAction = true;
		return;
	}
	
	ABattleCharacterBase* MyCharacter = ConsiderList->GetMyCharacter();
	
	if (!IsValid(MyCharacter))
	{
		bIsCompletedAction = true;
		return;
	}
	
	if (ASC)
	{
		AbilitySpec = ASC->FindAbilitySpecFromClass(GA_Attack);
		if (AbilitySpec)
		{
			
			// GA 하나 만들기 (Spot을 담고 있어야하며 전달 받은것으로 처리, Instancing은 Actor마다 있어야함.
			// Target Spot 찾는 함수
			TArray<FVector> TargetLocations;
			
			for (int32 Idx = 0; Idx < ConsiderList->GetCandidateArrNum(EAxisType::Target); ++Idx)
			{
				AActor* TargetActor = ConsiderList->GetCandidatePtr(EAxisType::Target, Idx);
				if (!IsValid(TargetActor))
				{
					continue;
				}
				TargetLocations.Add(TargetActor->GetActorLocation());
			}
			
			TArray<FVector> AttackAreaData = BattleUtilityActionHelper::FindBestSpotsWithGreedy(TargetLocations, AreaRadius, AreaNum);
			
			UNavigationSystemV1* NavSystem = UNavigationSystemV1::GetCurrent(GetWorld());
			
			if (AttackAreaData.IsEmpty())
			{
				bIsCompletedAction = true;
				return;
			}
			
			for (FVector& AreaData : AttackAreaData)
			{
				FNavLocation ProjectedNavLocation;
				
				if (NavSystem->ProjectPointToNavigation(AreaData, ProjectedNavLocation, FVector(300.f,300.f, 6000.f)))
				{
					AreaData = ProjectedNavLocation.Location;
				}	
			}
			
			if (UBattleCombatManagerComponent* CombatManagerComponent = Cast<UBattleCombatManagerComponent>(MyCharacter->GetComponentByClass(UBattleCombatManagerComponent::StaticClass())))
			{
				CombatManagerComponent->SetAreaCenterData(AttackAreaData);
				ASC->TryActivateAbility(AbilitySpec->Handle);
			}
		}
	}
}



UBattleUtilityAction_SpawnActor::UBattleUtilityAction_SpawnActor()
{
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
	UConsiderationFactors* ConsiderList = ConsiderationFactors.Get();
	
	if (!IsValid(ConsiderList))
	{
		bIsCompletedAction = true;
		return;
	}
	
	ABattleCharacterBase* MyCharacter = ConsiderList->GetMyCharacter();
	
	if (!IsValid(MyCharacter))
	{
		bIsCompletedAction = true;
		return;
	}
	
	if (ASC)
	{
		AbilitySpec = ASC->FindAbilitySpecFromClass(GA_Spawn);
		if (AbilitySpec)
		{
			
			// GA 하나 만들기 (Spot을 담고 있어야하며 전달 받은것으로 처리, Instancing은 Actor마다 있어야함.
			// Target Spot 찾는 함수
			TArray<FVector> TargetLocations;
			
			for (int32 Idx = 0; Idx < ConsiderList->GetCandidateArrNum(EAxisType::Target); ++Idx)
			{
				AActor* TargetActor = ConsiderList->GetCandidatePtr(EAxisType::Target, Idx);
				if (!IsValid(TargetActor))
				{
					continue;
				}
				TargetLocations.Add(TargetActor->GetActorLocation());
			}
			
			TArray<FVector> SpawnAreaData = BattleUtilityActionHelper::FindBestSpotsWithGreedy(TargetLocations, SpawnAreaRadius, SpawnActorNum);
			
			UNavigationSystemV1* NavSystem = UNavigationSystemV1::GetCurrent(GetWorld());
			
			if (SpawnAreaData.IsEmpty())
			{
				bIsCompletedAction = true;
				return;
			}
			
			for (FVector& AreaData : SpawnAreaData)
			{
				FNavLocation ProjectedNavLocation;
				
				if (NavSystem->ProjectPointToNavigation(AreaData, ProjectedNavLocation, FVector(300.f,300.f, 6000.f)))
				{
					AreaData = ProjectedNavLocation.Location;
				}	
			}
		
			// GA->SetTargetData(Target) => 해당 함수 만들어서 Spot 넘겨주기
			if (UGameplayAbility* InstancedAbility = AbilitySpec->GetPrimaryInstance())
			{
				if (UBattleGameplayAbility_Special_Spawn* InstancedGA = Cast<UBattleGameplayAbility_Special_Spawn>(InstancedAbility))
				{
					InstancedGA->SetSpawnCenterDAta(SpawnAreaData);
					ASC->TryActivateAbility(AbilitySpec->Handle);
				}
			}
		}
	}
}


