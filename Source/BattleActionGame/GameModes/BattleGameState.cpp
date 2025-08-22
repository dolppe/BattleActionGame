#include "BattleGameState.h"

#include "BattleActionGame/AbilitySystem/BattleAbilitySystemComponent.h"
#include "BattleExperienceManagerComponent.h"
#include "BattleActionGame/BattleLogChannels.h"
#include "GameFramework/PlayerState.h"
#include "Net/UnrealNetwork.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(BattleGameState)

ABattleGameState::ABattleGameState(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	AbilitySystemComponent = ObjectInitializer.CreateDefaultSubobject<UBattleAbilitySystemComponent>(this, TEXT("AbilitySystemComponent"));
	AbilitySystemComponent->SetIsReplicated(true);
	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Mixed);

	ExperienceManagerComponent = CreateDefaultSubobject<UBattleExperienceManagerComponent>(TEXT("ExperneceManagerComponent"));
}

void ABattleGameState::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	check(AbilitySystemComponent);
	AbilitySystemComponent->InitAbilityActorInfo(/*Owner=*/ this, /*Avatar=*/ this);
}

UAbilitySystemComponent* ABattleGameState::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

void ABattleGameState::AddPlayerState(APlayerState* PlayerState)
{
	Super::AddPlayerState(PlayerState);

	if (HasAuthority())
	{
		OnPlayerAddedOnlyServer.Broadcast(PlayerState);
	}
}

void ABattleGameState::RemovePlayerState(APlayerState* PlayerState)
{	
	Super::RemovePlayerState(PlayerState);

	if (HasAuthority())
	{
		OnPlayerRemovedOnlyServer.Broadcast(PlayerState);
	}

}

void ABattleGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass, GameReadyNum);
	DOREPLIFETIME(ThisClass, BestStatValue);
	DOREPLIFETIME(ThisClass, BestPlayerIdx);
	DOREPLIFETIME(ThisClass, GameEndingType);

	
}

void ABattleGameState::OnRep_GameReadyNum()
{
}

void ABattleGameState::SelectBestPlayer()
{
	TArray<FCombatStat*> CombatStats;

	BestPlayerIdx.Init(0,(int) ECombatStatType::MAX);
	
	for (APlayerState* PS : PlayerArray)
	{
		if (ABattlePlayerState* BattlePS = Cast<ABattlePlayerState>(PS))
		{
			CombatStats.Add(&BattlePS->CombatStat);
		}
	}

	for (int StatTypeIdx = 0; StatTypeIdx < (int) ECombatStatType::MAX; ++StatTypeIdx)
	{
		ECombatStatType CurType = (ECombatStatType) StatTypeIdx;
		float BestValue = -1.0f;
		int BestIdx = 0;
		
		for (int PlayerStatsIdx = 0; PlayerStatsIdx < CombatStats.Num();++PlayerStatsIdx)
		{
			float CurValue = CombatStats[PlayerStatsIdx]->GetValue(CurType);

			if (BestValue < CurValue )
			{
				BestValue = CurValue;
				BestIdx = PlayerStatsIdx;
			}
		}

		BestStatValue.SetValue(CurType, BestValue);
		BestPlayerIdx[StatTypeIdx] = BestIdx;
	}

	
}

void ABattleGameState::OnRep_BestStatValue()
{
}

void ABattleGameState::OnRep_BestPlayerIdx()
{
}

void ABattleGameState::OnRep_GameEndingType()
{
}
