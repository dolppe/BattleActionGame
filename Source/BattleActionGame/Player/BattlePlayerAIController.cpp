#include "BattlePlayerAIController.h"

#include "NavigationSystem.h"
#include "BattleActionGame/BattleGameplayTags.h"
#include "BattleActionGame/AbilitySystem/BattleAbilitySystemComponent.h"
#include "BattleActionGame/Character/BattleCharacter.h"
#include "BattleActionGame/Character/BattleObserverPawn.h"
#include "BattleActionGame/Combat/BattleCombatManagerComponent.h"
#include "BattleActionGame/Combat/BattleGameplayAbility_ComboAttack.h"
#include "BattleActionGame/Enemy/BattleEnemyCharacter.h"
#include "Kismet/GameplayStatics.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(BattlePlayerAIController)

PRAGMA_DISABLE_OPTIMIZATION

ABattlePlayerAIController::ABattlePlayerAIController(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	
}

void ABattlePlayerAIController::TickActor(float DeltaTime, enum ELevelTick TickType,
	FActorTickFunction& ThisTickFunction)
{
	Super::TickActor(DeltaTime, TickType, ThisTickFunction);
	
	if (CanThink())
	{
		EPlayerAIActionType CurAction = Think();


		switch (CurAction)
		{
		case EPlayerAIActionType::SelectBoss:
			SelectBoss();
			MoveToBoss();
			break;
		case EPlayerAIActionType::MoveToBoss:
			MoveToBoss();
			break;
		case EPlayerAIActionType::AttackEnemy:
			AttackToBoss();
			break;
		case EPlayerAIActionType::AvoidAttack:
			AvoidAttack();
			break;
		default:
			break;
		}
		
		LastAction = CurAction;		
	}
	
}

bool ABattlePlayerAIController::CanThink()
{
	ABattleCharacter* MyCharacter = GetPawn<ABattleCharacter>();
	if (MyCharacter == nullptr)
	{
		return false;
	}
	if (GetWorld()->GetTimeSeconds() - LastThinkTime < ThinkPeriod)
	{
		return false;
	}
	
	
	
	return true;
}

EPlayerAIActionType ABattlePlayerAIController::Think()
{
	if (SelectedBoss == nullptr)
	{
		return EPlayerAIActionType::SelectBoss;
	}
	
	if (bUsePerfectMoving)
	{
		if (UAbilitySystemComponent* BossASC = SelectedBoss->GetAbilitySystemComponent())
		{
			if (BossASC->HasMatchingGameplayTag(FBattleGameplayTags::Get().AI_Attack_Intent))
			{
				return EPlayerAIActionType::AvoidAttack;
			}
		}
	}
	
	float Distance = FVector::Dist2D(GetPawn()->GetActorLocation(), SelectedBoss->GetActorLocation());
	
	if (Distance < AllowedAttackRange)
	{
		return EPlayerAIActionType::AttackEnemy;
	}
	if (UPathFollowingComponent* AIPathFollowingComponent = GetPathFollowingComponent())
	{
		EPathFollowingStatus::Type FollowStatus = AIPathFollowingComponent->GetStatus();
		if (EPathFollowingStatus::Type::Moving != FollowStatus)
		{
			return EPlayerAIActionType::MoveToBoss;
		}
	}
	
	return EPlayerAIActionType::None;
}

void ABattlePlayerAIController::SelectBoss()
{
	TArray<AActor*> EnemyActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ABattleEnemyCharacter::StaticClass(),EnemyActors);
	
	for (AActor* EnemyActor : EnemyActors)
	{
		if (ABattleEnemyCharacter* EnemyBoss = Cast<ABattleEnemyCharacter>(EnemyActor))
		{
			SelectedBoss = EnemyBoss;
			return;
		}
	}
	
	return;
		
}

void ABattlePlayerAIController::MoveToBoss()
{
	if (UPathFollowingComponent* AIPathFollowingComponent = GetPathFollowingComponent())
	{
		if (EPathFollowingStatus::Type::Moving == AIPathFollowingComponent->GetStatus())
		{
			if (SelectedBoss == CurMoveToBoss)
			{
				// 움직이는 중에 목표도 같음
				return;
			}
		}
	}
	
	// 움직이지 않고 있거나 새로운 목표가 생김
	
	MoveToActor(SelectedBoss, AllowedMoveRange);
	CurMoveToBoss = SelectedBoss;	
}

void ABattlePlayerAIController::AttackToBoss()
{
	FVector MyLocation = GetPawn()->GetActorLocation();
	FVector TargetLocation = SelectedBoss->GetActorLocation();
	FVector ToTargetLocation = (TargetLocation - MyLocation).GetSafeNormal2D();
	
	GetPawn()->SetActorRotation(ToTargetLocation.Rotation());
	
	if (UBattleCombatManagerComponent* CombatManagerComponent = Cast<UBattleCombatManagerComponent>(GetPawn()->GetComponentByClass(UBattleCombatManagerComponent::StaticClass())))
	{
		if (ComboStrongAbilitySpec != nullptr && ComboStrongAbilitySpec->IsActive())
		{
			return;
		}
		if (ComboAbilitySpec != nullptr && ComboAbilitySpec->IsActive())
		{
			if (UBattleGameplayAbility_ComboAttack* ComboAttack = Cast<UBattleGameplayAbility_ComboAttack>(CombatManagerComponent->GetCurrentAttackGA()))
			{
				ComboAttack->RequestNextCombo();
				CombatManagerComponent->RequestStrongAttack();
			}
		}
		else
		{
			ComboAbilitySpec = CharacterASC->FindAbilitySpecFromClass(GA_ComboAttack);
			if (ComboAbilitySpec)
			{
				CharacterASC->TryActivateAbility(ComboAbilitySpec->Handle);
			}
		}
	}


	
	
	
}

void ABattlePlayerAIController::AvoidAttack()
{
	if (SelectedBoss == nullptr)
	{
		return;
	}
	
	
	if (LastAction == EPlayerAIActionType::AvoidAttack)
	{
		if (UPathFollowingComponent* AIPathFollowingComponent = GetPathFollowingComponent())
		{
			EPathFollowingStatus::Type FollowStatus = AIPathFollowingComponent->GetStatus();
			if (EPathFollowingStatus::Type::Moving == FollowStatus)
			{
				return;
			}
			else
			{
				AvoidPathIndex++;
				if (AvoidPaths.IsValidIndex(AvoidPathIndex))
				{
					MoveToLocation(AvoidPaths[AvoidPathIndex], 100.f);
				}
			}
		}	
	}
	else
	{
		AvoidPaths.Empty();
		AvoidPathIndex = 0;
		
		if (UAbilitySystemComponent* BossASC = SelectedBoss->GetAbilitySystemComponent())
		{
			UNavigationSystemV1* NavSystem = UNavigationSystemV1::GetCurrent(GetWorld());
			FNavLocation ProjectedNavLocation;
			if (BossASC->HasMatchingGameplayTag(FBattleGameplayTags::Get().AI_Attack_Intent_Front))
			{
				FVector ToTarget = (SelectedBoss->GetActorLocation() - GetPawn()->GetActorLocation()).GetSafeNormal2D();
				FVector BossSideLocation;
				if (SelectedBoss->GetActorRightVector().Dot(ToTarget) > 0.f)
				{
					BossSideLocation = SelectedBoss->GetActorLocation() - SelectedBoss->GetActorRightVector()*800.f;
				}
				else
				{
					// 보스 기준 오른쪽
					BossSideLocation = SelectedBoss->GetActorLocation() + SelectedBoss->GetActorRightVector()*800.f;
				}
				
				if (NavSystem->ProjectPointToNavigation(BossSideLocation, ProjectedNavLocation, FVector(700.f,700.f,1000.f)))
				{
					BossSideLocation = ProjectedNavLocation.Location;
				}
				
				FVector BossBackLocation = SelectedBoss->GetActorLocation() - SelectedBoss->GetActorForwardVector()*500.f;
				if (NavSystem->ProjectPointToNavigation(BossBackLocation, ProjectedNavLocation, FVector(700.f,700.f,1000.f)))
				{
					BossBackLocation = ProjectedNavLocation.Location;
				}
				
				
				AvoidPaths.Add(BossSideLocation);
				AvoidPaths.Add(BossBackLocation);
			}
			else if (BossASC->HasMatchingGameplayTag(FBattleGameplayTags::Get().AI_Attack_Intent_Around))
			{
				FVector RunawayFromBoss = (GetPawn()->GetActorLocation() - SelectedBoss->GetActorLocation()).GetSafeNormal2D(); 
				RunawayFromBoss = SelectedBoss->GetActorLocation() + RunawayFromBoss*2000.f;
				
				if (NavSystem->ProjectPointToNavigation(RunawayFromBoss, ProjectedNavLocation, FVector(700.f,700.f,1000.f)))
				{
					RunawayFromBoss = ProjectedNavLocation.Location;
				}
				
				AvoidPaths.Add(RunawayFromBoss);
			}
			else if (BossASC->HasMatchingGameplayTag(FBattleGameplayTags::Get().AI_Attack_Intent_Rush))
			{
				FVector ToTarget = (SelectedBoss->GetActorLocation() - GetPawn()->GetActorLocation()).GetSafeNormal2D();
				FVector BossSideLocation;
				if (SelectedBoss->GetActorRightVector().Dot(ToTarget) > 0.f)
				{
					// 보스 기준 왼쪽
					BossSideLocation = SelectedBoss->GetActorLocation() - SelectedBoss->GetActorRightVector()*800.f;
				}
				else
				{
					// 보스 기준 오른쪽
					BossSideLocation = SelectedBoss->GetActorLocation() + SelectedBoss->GetActorRightVector()*800.f;
				}
				
				if (NavSystem->ProjectPointToNavigation(BossSideLocation, ProjectedNavLocation, FVector(700.f,700.f,1000.f)))
				{
					BossSideLocation = ProjectedNavLocation.Location;
				}
				
				AvoidPaths.Add(BossSideLocation);
			}
			else if (BossASC->HasMatchingGameplayTag(FBattleGameplayTags::Get().AI_Attack_Intent_AOE))
			{
				
			}
			else if (BossASC->HasMatchingGameplayTag(FBattleGameplayTags::Get().AI_Attack_Intent_Random))
			{
				FVector ToTarget = (SelectedBoss->GetActorLocation() - GetPawn()->GetActorLocation()).GetSafeNormal2D();
				FVector BossSideLocation;
				if (SelectedBoss->GetActorRightVector().Dot(ToTarget) > 0.f)
				{
					BossSideLocation = SelectedBoss->GetActorLocation() - SelectedBoss->GetActorRightVector()*800.f;
				}
				else
				{
					// 보스 기준 오른쪽
					BossSideLocation = SelectedBoss->GetActorLocation() + SelectedBoss->GetActorRightVector()*800.f;
				}
				
				if (NavSystem->ProjectPointToNavigation(BossSideLocation, ProjectedNavLocation, FVector(700.f,700.f,1000.f)))
				{
					BossSideLocation = ProjectedNavLocation.Location;
				}
				
				FVector BossBackLocation = SelectedBoss->GetActorLocation() - SelectedBoss->GetActorForwardVector()*500.f;
				if (NavSystem->ProjectPointToNavigation(BossBackLocation, ProjectedNavLocation, FVector(700.f,700.f,1000.f)))
				{
					BossBackLocation = ProjectedNavLocation.Location;
				}
				
				
				AvoidPaths.Add(BossSideLocation);
				AvoidPaths.Add(BossBackLocation);
			}
			
			for (FVector& AvoidPath : AvoidPaths)
			{
				FVector StartLine = AvoidPath;
				FVector EndLine = AvoidPath;
			
				StartLine.Z = AvoidPath.Z +5000.f;
				EndLine.Z = AvoidPath.Z -5000.f;
			
				//DrawDebugLine(GetWorld(), StartLine, EndLine, FColor::Red, false, 5.f, 1, 10.f);
				DrawDebugPoint(GetWorld(), AvoidPath, 10.f, FColor::Red, false, 5.f, 1);	
			}
			
			if (AvoidPaths.IsValidIndex(0))
			{
				MoveToLocation(AvoidPaths[0], 100.f);
			}
		}
	}
	
	

	
}

void ABattlePlayerAIController::StartStrongAttack()
{
	if (ComboStrongAbilitySpec == nullptr)
	{
		ComboStrongAbilitySpec = CharacterASC->FindAbilitySpecFromClass(GA_StrongAttack);
	}
	
	CharacterASC->TryActivateAbility(ComboStrongAbilitySpec->Handle);
}

void ABattlePlayerAIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);
	
	if (ABattleCharacter* BattleCharacter = Cast<ABattleCharacter>(InPawn))
	{
		CharacterASC = BattleCharacter->GetBattleAbilitySystemComponent();
	}
}

PRAGMA_ENABLE_OPTIMIZATION
