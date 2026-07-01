#pragma once


#include "BattleActionGame/Combat/BattleCombatManagerComponent.h"
#include "BattleActionGame/Combat/BattleGameplayAbility_Attack_Parent.h"
#include "BattleActionGame/Combat/Item/BattleItemData.h"
#include "BattleCombatTests.generated.h"

UCLASS()
class UMockBattleGameplayAbility_Attack_Parent : public UBattleGameplayAbility_Attack_Parent
{
	GENERATED_BODY()
public:
	
	UMockBattleGameplayAbility_Attack_Parent(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
	
	TArray<AActor*>& GetAlreadyHitActors()
	{
		return AlreadyHitActors;
	}
	
	virtual void OnTargetDataReadyCallback(const FGameplayAbilityTargetDataHandle& InData, FGameplayTag ApplicationTag) override
	{
		TargetDataCount++;
		CurTargetDataLists.Add(InData.Data[0].Get()->GetHitResult()->GetActor());
	}
	
	TArray<AActor*> CurTargetDataLists;
	int TargetDataCount;
	
};


UCLASS()
class UMockTestBattleCombatManagerComponent : public UBattleCombatManagerComponent
{
	GENERATED_BODY()
public:
	virtual void OnHitEvent(const FBattleHitMessage& HitMessage) override;
	
	int HitEventCount;
	
};

UCLASS()
class UMockBattleItemData_A : public UBattleItemData
{
	GENERATED_BODY()

public:
	
};

UCLASS()
class UMockBattleItemData_B : public UBattleItemData
{
	GENERATED_BODY()

public:
	
};

UCLASS()
class UMockBattleItemData_C : public UBattleItemData
{
	GENERATED_BODY()

public:
	
};

UCLASS()
class UMockBattleItemData_D : public UBattleItemData
{
	GENERATED_BODY()

public:
	
};




