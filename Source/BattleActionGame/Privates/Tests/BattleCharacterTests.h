#pragma once

#include "BattleActionGame/Character/BattleCharacter.h"
#include "BattleActionGame/Combat/BattleCombatManagerComponent.h"

#include "BattleCharacterTests.generated.h"


UCLASS()
class UMockTestObject : public UObject
{
	GENERATED_BODY()
public:
	
	UFUNCTION()
	void HealthAttributeChange(UBattleHealthComponent* HealthComponent, float OldValue, float NewValue, AActor* Instigator)
	{
		HealthAttributeChangeCount++;
		CurOldValue = OldValue;
		CurNewValue = NewValue;
	}
	
	UFUNCTION()
	void ActorEvent(AActor* InActor)
	{
		ActorEventCount++;
	}
	
	float CurOldValue = 0.f;
	float CurNewValue = 0.f;
	int HealthAttributeChangeCount = 0;
	
	int ActorEventCount = 0;
	
};


UCLASS()
class AMockTestBattleCharacter : public ABattleCharacter
{
	GENERATED_BODY()
public:
	
	AMockTestBattleCharacter(const FObjectInitializer& ObjectInitializer)
		: Super(ObjectInitializer)
	{
		
	}
	
	virtual void OnGroggy() override;
	virtual void OnPoiseBreak() override;
	
	float GetGroggyThreshold()
	{
		return GroggyThreshold;
	}
	
	float GetPoiseBreakThreshold()
	{
		return PoiseBreakThreshold;
	}
	
	void TestHandleImpactDamage(float DamageMagnitude);
	
	int GroggyCount = 0;
	int PoiseBreakCount = 0;
	
	
};

UCLASS()
class UMockTestBattleCombatManagerComponent : public UBattleCombatManagerComponent
{
	GENERATED_BODY()
public:
	virtual void OnHitEvent(const FBattleHitMessage& HitMessage) override;
	
	int HitEventCount;
	
};
