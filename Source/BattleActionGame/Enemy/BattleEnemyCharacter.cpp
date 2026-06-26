#include "BattleEnemyCharacter.h"

#include "BattleEnemyController.h"
#include "BattleEnemyData.h"
#include "GameplayEffect.h"
#include "BattleActionGame/BattleGameplayTags.h"
#include "BattleActionGame/BattleLogChannels.h"
#include "BattleActionGame/AbilitySystem/BattleAbilitySet.h"
#include "BattleActionGame/AbilitySystem/BattleAbilitySystemComponent.h"
#include "BattleActionGame/AbilitySystem/Attributes/BattleCombatSet.h"
#include "BattleActionGame/AbilitySystem/Attributes/BattleEnemySet.h"
#include "BattleActionGame/AbilitySystem/Attributes/BattleHealthSet.h"
#include "BattleActionGame/Character/BattleCharacterMovementComponent.h"
#include "BattleActionGame/Character/BattleHealthComponent.h"
#include "BattleActionGame/Navigation/GridRiskMapComponent.h"
#include "BattleActionGame/Physics/BattleCollisionChannels.h"
#include "BattleActionGame/Physics/BattlePartsManagerComponent.h"
#include "BattleActionGame/Physics/PhysicalMaterialWithTags.h"
#include "UtilityAI/BattleUtilityAIComponent.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(BattleEnemyCharacter)

ABattleEnemyCharacter::ABattleEnemyCharacter(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer.SetDefaultSubobjectClass<UBattleCharacterMovementComponent>(ACharacter::CharacterMovementComponentName))
{
	AIControllerClass = ABattleEnemyController::StaticClass();
	
	AbilitySystemComponent = ObjectInitializer.CreateDefaultSubobject<UBattleAbilitySystemComponent>(this, TEXT("AbilitySystemComponent"));
	AbilitySystemComponent->SetIsReplicated(true);
	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Mixed);

	//CreateDefaultSubobject<UBattleHealthSet>(TEXT("HealthSet"));
	//CreateDefaultSubobject<UBattleCombatSet>(TEXT("CombatSet"));
	//CreateDefaultSubobject<UBattleEnemySet>(TEXT("EnemySet"));
	
	HealthComponent = CreateDefaultSubobject<UBattleHealthComponent>(TEXT("HealthComponent"));
	PartsManagerComponent = CreateDefaultSubobject<UBattlePartsManagerComponent>(TEXT("PartsManagerComponent"));
	PartsManagerComponent->SetIsReplicated(true);
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
	
	if (EnemyData)
	{
		for (UBattleAbilitySet* AbilitySet : EnemyData->AbilitySets)
		{
			if (AbilitySet)
			{
				BA_DEFAULT_LOG(LogBattle, Log, TEXT("StartGive GA, GE, AttributeSet"));
				AbilitySet->GiveToAbilitySystem(AbilitySystemComponent, nullptr);
			}
		}

		AbilitySystemComponent->SetTagRelationshipMapping(EnemyData->TagRelationshipMapping);
	}

	if (HasAuthority())
	{
		HealthComponent->InitializeWithAbilitySystem(AbilitySystemComponent);	
	}
	

	if (const UBattleEnemySet* EnemySet  = AbilitySystemComponent->GetSet<UBattleEnemySet>())
	{
		EnemySet->OnGroggyState.AddUObject(this, &ThisClass::OnGroggyStateRequest);
		EnemySet->OnPoiseBreakState.AddUObject(this, &ThisClass::OnPoiseBreakRequest);
		
	}

}

PRAGMA_DISABLE_OPTIMIZATION

void ABattleEnemyCharacter::HandleDamageToPart(FGameplayTag PartTag, const FVector& AttackDirection)
{
	if (!HasAuthority())
	{
		return;
	}

	PartsManagerComponent->HandleDamagedToPart(PartTag, AttackDirection);
}

void ABattleEnemyCharacter::OnPoiseBreakRequest(AActor* DamageInstigator, AActor* DamageCauser,
	const FGameplayEffectSpec& DamageEffectSpec, float DamageMagnitude)
{
	PerformPoiseBreak();
}

void ABattleEnemyCharacter::OnGroggyStateRequest(AActor* DamageInstigator, AActor* DamageCauser,
	const FGameplayEffectSpec& DamageEffectSpec, float DamageMagnitude)
{
	PerformGroggy();
}


PRAGMA_ENABLE_OPTIMIZATION
