#include "BattlePlayerAIController.h"

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
