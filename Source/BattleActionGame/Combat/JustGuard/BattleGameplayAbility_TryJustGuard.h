#pragma once

#include "BattleActionGame/AbilitySystem/Abilities/BattleGameplayAbility.h"
#include "BattleGameplayAbility_TryJustGuard.generated.h"


/*
 * Enemy가 Player에게 GiveAbilityAndOnceActivate를 통해 Give, Activate되는 GA
 * Player의 입력을 일정 시간 기다리고 오면 JustGuard 요청을 Enemy에게 보내는 것
 * TriggerEvent에 지속시간과 Enemy에 대한 정보가 담겨있음.
 */

class UBattleCombatManagerComponent;

UCLASS()
class UBattleGameplayAbility_TryJustGuard : public UBattleGameplayAbility
{
	GENERATED_BODY()
public:

	UBattleGameplayAbility_TryJustGuard(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;

protected:

	UFUNCTION(Server, Reliable)
	void ServerKeyPressed();
	
	UFUNCTION()
	void GuardKeyPressed();
	
private:

	UBattleCombatManagerComponent* TargetCombatManager;
	FDelegateHandle InputDelegateHandle;
	FTimerHandle AbilityEndTimer;

	bool bPressedKey = false;
	
};

