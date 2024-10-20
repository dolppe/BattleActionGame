#include "BattleEnemyCharacter.h"

#include "BattleEnemyController.h"
#include "BattleEnemyData.h"
#include "BattleActionGame/AbilitySystem/BattleAbilitySet.h"
#include "BattleActionGame/AbilitySystem/BattleAbilitySystemComponent.h"
#include "BattleActionGame/AbilitySystem/Attributes/BattleCombatSet.h"
#include "BattleActionGame/AbilitySystem/Attributes/BattleHealthSet.h"
#include "BattleActionGame/Character/BattleCharacterMovementComponent.h"
#include "BattleActionGame/Character/BattleHealthComponent.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(BattleEnemyCharacter)

ABattleEnemyCharacter::ABattleEnemyCharacter(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer.SetDefaultSubobjectClass<UBattleCharacterMovementComponent>(ACharacter::CharacterMovementComponentName))
{
	AIControllerClass = ABattleEnemyController::StaticClass();
	
	AbilitySystemComponent = ObjectInitializer.CreateDefaultSubobject<UBattleAbilitySystemComponent>(this, TEXT("AbilitySystemComponent"));
	AbilitySystemComponent->SetIsReplicated(true);
	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Mixed);

	CreateDefaultSubobject<UBattleHealthSet>(TEXT("HealthSet"));
	CreateDefaultSubobject<UBattleCombatSet>(TEXT("CombatSet"));
	
	HealthComponent = CreateDefaultSubobject<UBattleHealthComponent>(TEXT("HealthComponent"));

	NetUpdateFrequency = 100.0f;
}

UAbilitySystemComponent* ABattleEnemyCharacter::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

UBattleAbilitySystemComponent* ABattleEnemyCharacter::GetBattleAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

UBattleHealthComponent* ABattleEnemyCharacter::GetHealthComponent() const
{
	return HealthComponent;
}

void ABattleEnemyCharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	AbilitySystemComponent->InitAbilityActorInfo(this, this);

	HealthComponent->InitializeWithAbilitySystem(AbilitySystemComponent);

	if (EnemyData)
	{
		for (UBattleAbilitySet* AbilitySet : EnemyData->AbilitySets)
		{
			if (AbilitySet)
			{
				AbilitySet->GiveToAbilitySystem(AbilitySystemComponent, nullptr);
			}
		}
	}
	
	
}
