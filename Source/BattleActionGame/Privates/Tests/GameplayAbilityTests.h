#pragma once
#include "BattleActionGame/AbilitySystem/BattleAbilitySet.h"
#include "BattleActionGame/AbilitySystem/BattleAbilityTagRelationshipMapping.h"
#include "BattleActionGame/AbilitySystem/Abilities/Phase/BattleGamePhaseAbility.h"

#include "GameplayAbilityTests.generated.h"

UCLASS()
class UMockTestGameplayAbilityBase : public UBattleGameplayAbility
{
	GENERATED_BODY()
public:
	
	UMockTestGameplayAbilityBase(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
	
	int ActivateCount = 0;
	int InputPressedCount = 0;
	int InputReleasedCount = 0;
	int EndAbilityCount = 0;
	
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	virtual void InputPressed(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo) override;
	virtual void InputReleased(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo) override;
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;
	
	
};

UCLASS()
class UMockTestGameplayAbility_A : public UMockTestGameplayAbilityBase
{
	GENERATED_BODY()
public:
	UMockTestGameplayAbility_A(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
	
};

UCLASS()
class UMockTestGameplayAbility_B : public UMockTestGameplayAbilityBase
{
	GENERATED_BODY()
public:
	UMockTestGameplayAbility_B(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
	
};

UCLASS()
class UMockTestGameplayAbility_C : public UMockTestGameplayAbilityBase
{
	GENERATED_BODY()
public:
	UMockTestGameplayAbility_C(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
	
};


UCLASS()
class UMockGameplayAbility_WhileInputActive : public UMockTestGameplayAbilityBase
{
	GENERATED_BODY()
public:
	UMockGameplayAbility_WhileInputActive(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
};

UCLASS()
class UMockGameplayAbility_InputTriggered : public UMockTestGameplayAbilityBase
{
	GENERATED_BODY()
public:
	UMockGameplayAbility_InputTriggered(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
};

UCLASS()
class UMockGameplayEffect_A : public UGameplayEffect
{
	GENERATED_BODY()
public:
	UMockGameplayEffect_A(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
	
};

UCLASS()
class UMockGameplayEffect_B : public UGameplayEffect
{
	GENERATED_BODY()
public:
	UMockGameplayEffect_B(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
	
};

UCLASS()
class UMockAttributeSet_A : public UAttributeSet
{
	GENERATED_BODY()
public:
	UMockAttributeSet_A(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
	
};

UCLASS()
class UMockAttributeSet_B : public UAttributeSet
{
	GENERATED_BODY()
public:
	UMockAttributeSet_B(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
	
};

UCLASS()
class UBattleAbilityTagRelationshipMappingForTest : public UBattleAbilityTagRelationshipMapping
{
	GENERATED_BODY()
public:
	
	void AddTagRelationship(const FBattleAbilityTagRelationship& InTagRelationship)
	{
		AbilityTagRelationships.Add(InTagRelationship);
	}
	
	
};


UCLASS()
class UBattleAbilitySetForTest : public UBattleAbilitySet 
{
	GENERATED_BODY()
public:
	
	void AddGameplayAbility(const FBattleAbilitySet_GameplayAbility& InGameplayAbility)
	{
		GrantedGameplayAbilities.Add(InGameplayAbility);
	}
	
	void AddGameplayEffect(const FBattleAbilitySet_GameplayEffect& InGameplayEffect)
	{
		GrantedGameplayEffects.Add(InGameplayEffect);
	}
	
	void AddAttributeSet(const FBattleAbilitySet_AttributeSet& InAttributeSet)
	{
		GrantedAttributes.Add(InAttributeSet);
	}
	
};

UCLASS()
class UMockGamePhaseAbility_A : public UBattleGamePhaseAbility 
{
	GENERATED_BODY()
public:
	
	UMockGamePhaseAbility_A(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
	
};

UCLASS()
class UMockGamePhaseAbility_B : public UBattleGamePhaseAbility 
{
	GENERATED_BODY()
public:
	
	UMockGamePhaseAbility_B(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
	
};

UCLASS()
class UMockGamePhaseAbility_A_C : public UBattleGamePhaseAbility 
{
	GENERATED_BODY()
public:
	
	UMockGamePhaseAbility_A_C(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
	
};


