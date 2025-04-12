#include "BattleEnemyCharacter.h"

#include "BattleEnemyController.h"
#include "BattleEnemyData.h"
#include "GameplayEffect.h"
#include "BattleActionGame/BattleGameplayTags.h"
#include "BattleActionGame/AbilitySystem/BattleAbilitySet.h"
#include "BattleActionGame/AbilitySystem/BattleAbilitySystemComponent.h"
#include "BattleActionGame/AbilitySystem/Attributes/BattleCombatSet.h"
#include "BattleActionGame/AbilitySystem/Attributes/BattleEnemySet.h"
#include "BattleActionGame/AbilitySystem/Attributes/BattleHealthSet.h"
#include "BattleActionGame/Character/BattleCharacterMovementComponent.h"
#include "BattleActionGame/Character/BattleHealthComponent.h"
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
				AbilitySet->GiveToAbilitySystem(AbilitySystemComponent, nullptr);
			}
		}

		AbilitySystemComponent->SetTagRelationshipMapping(EnemyData->TagRelationshipMapping);
	}

	HealthComponent->InitializeWithAbilitySystem(AbilitySystemComponent);

	FBreakablePart LeftLegBreakablePart = FBreakablePart(1, [this](FGameplayTag InGameplayTag)
	{
		USkeletalMeshComponent* SkeletalMeshComponent = GetMesh();
		if (SkeletalMeshComponent)
		{
			SkeletalMeshComponent->HideBoneByName(TEXT("Dummy012"), PBO_None);  
			SkeletalMeshComponent->SetAllBodiesBelowSimulatePhysics(TEXT("Dummy012"), true, false);  
			SkeletalMeshComponent->SetAllBodiesBelowPhysicsDisabled(TEXT("Dummy012"), true, true);  
		}
		if (UBattleUtilityAIComponent* UtilityAIComponent = Cast<UBattleUtilityAIComponent>(GetComponentByClass(UBattleUtilityAIComponent::StaticClass())))
		{
			UtilityAIComponent->BreakParts(InGameplayTag);
		}
	});

	FBreakablePart RightLegBreakablePart = FBreakablePart(1, [this](FGameplayTag InGameplayTag)
	{
		USkeletalMeshComponent* SkeletalMeshComponent = GetMesh();
		if (SkeletalMeshComponent)
		{
			SkeletalMeshComponent->HideBoneByName(TEXT("Dummy020"), PBO_None);  
			SkeletalMeshComponent->SetAllBodiesBelowSimulatePhysics(TEXT("Dummy020"), true, false);  
			SkeletalMeshComponent->SetAllBodiesBelowPhysicsDisabled(TEXT("Dummy020"), true, true);  
		}

		if (UBattleUtilityAIComponent* UtilityAIComponent = Cast<UBattleUtilityAIComponent>(GetComponentByClass(UBattleUtilityAIComponent::StaticClass())))
		{
			UtilityAIComponent->BreakParts(InGameplayTag);
		}
	});
	
	BreakableParts.Add(FBattleGameplayTags::Get().Gameplay_Breakable_LeftLeg, LeftLegBreakablePart);
	BreakableParts.Add(FBattleGameplayTags::Get().Gameplay_Breakable_RightLeg, RightLegBreakablePart);
	
	const UBattleEnemySet* EnemySet = AbilitySystemComponent->GetSet<UBattleEnemySet>();

	if (EnemySet)
	{
		EnemySet->OnGroggyState.AddUObject(this, &ThisClass::HandleGroggyState);
	}
}

PRAGMA_DISABLE_OPTIMIZATION

void ABattleEnemyCharacter::AttackBreakablePart(FGameplayTag InGameplayTag)
{
	if (BreakableParts.Contains(InGameplayTag))
	{
		BreakableParts[InGameplayTag].RemainHp--;
		if (BreakableParts[InGameplayTag].RemainHp == 0)
		{
			BreakableParts[InGameplayTag].DestroyFunction(InGameplayTag);
		}
	}
}

void ABattleEnemyCharacter::HandleGroggyState(AActor* GEInstigator, AActor* GECauser, const FGameplayEffectSpec& GEEffectSpec, float GEMagnitude)
{
	UE_LOG(LogTemp, Log, TEXT("Groggy"));

	//AbilitySystemComponent->AddLooseGameplayTag(FBattleGameplayTags::Get().Status_Groggy);

	FGameplayTagContainer GameplayTags;
	GameplayTags.AddTag(FBattleGameplayTags::Get().Ability_Type_Action_Groggy);
	AbilitySystemComponent->TryActivateAbilitiesByTag(GameplayTags, false);
	
}


PRAGMA_ENABLE_OPTIMIZATION
