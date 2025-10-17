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
}

PRAGMA_DISABLE_OPTIMIZATION

void ABattleEnemyCharacter::AttackBreakablePart(FGameplayTag InGameplayTag)
{

}

void ABattleEnemyCharacter::ChangePhyMatPart(FName BoneName, FGameplayTag InGameplayTag)
{
	
	GetMesh()->GetBodyInstance(BoneName)->SetPhysMaterialOverride(*PhysicMaterialWithSurface.Find(InGameplayTag));
	
}

void ABattleEnemyCharacter::HandleGroggyState(AActor* GEInstigator, AActor* GECauser, const FGameplayEffectSpec& GEEffectSpec, float GEMagnitude)
{
	//UE_LOG(LogTemp, Log, TEXT("Groggy"));

	//AbilitySystemComponent->AddLooseGameplayTag(FBattleGameplayTags::Get().Status_Groggy);

	FGameplayTagContainer GameplayTags;
	GameplayTags.AddTag(FBattleGameplayTags::Get().Ability_Type_Action_Groggy);
	AbilitySystemComponent->TryActivateAbilitiesByTag(GameplayTags, false);
	
}

void ABattleEnemyCharacter::HandleDamageToPart(FName BoneName, FGameplayTag PartTag)
{
	if (!HasAuthority())
	{
		return;
	}

	PartsManagerComponent->HandleDamagedToPart(BoneName, PartTag);
}

void ABattleEnemyCharacter::DestroyParts(TArray<FName> BoneNames)
{
	USkeletalMeshComponent* SkeletalMeshComponent = GetMesh();
	for (const FName& BoneName : BoneNames)
	{
		SkeletalMeshComponent->HideBoneByName(BoneName, PBO_None);  
		SkeletalMeshComponent->SetAllBodiesBelowSimulatePhysics(BoneName, true, false);  
		SkeletalMeshComponent->SetAllBodiesBelowPhysicsDisabled(BoneName, true, true);  
	}
}

// void ABattleEnemyCharacter::DamagedParts(TArray<FName> BoneNames, FGameplayTag TargetTag)
// {
// 	if (UPhysicalMaterial* PhysicalMaterial = *PhysicMaterialWithSurface.Find(TargetTag))
// 	{
// 		UPhysicalMaterialWithTags* PhysicalMaterialWithTags = Cast<UPhysicalMaterialWithTags>(PhysicalMaterial);
// 		PhysicalMaterialWithTags->PartTag = FBattleGameplayTags::Get().Combat_Attack_Hit;
// 		for (const FName& BoneName: BoneNames)
// 		{
// 			GetMesh()->GetBodyInstance(BoneName)->SetPhysMaterialOverride(PhysicalMaterial);
// 		}
// 		
// 	}
// }


PRAGMA_ENABLE_OPTIMIZATION
