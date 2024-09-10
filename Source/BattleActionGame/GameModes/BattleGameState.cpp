#include "BattleGameState.h"

#include "BattleActionGame/AbilitySystem/BattleAbilitySystemComponent.h"
#include "BattleExperienceManagerComponent.h"

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
