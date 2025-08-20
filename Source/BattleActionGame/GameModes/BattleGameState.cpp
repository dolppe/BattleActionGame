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
}

void ABattleGameState::OnRep_GameReadyNum()
{
}
