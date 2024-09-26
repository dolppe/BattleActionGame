#pragma once

#include "Abilities/Tasks/AbilityTask.h"
#include "BattleAbilityTask_HitCheck.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FHitCheckDelegate, const FHitResult, HitResult,const float, AttackTime);

UCLASS()
class UBattleAbilityTask_HitCheck : public UAbilityTask
{
	GENERATED_BODY()
public:

	UBattleAbilityTask_HitCheck();

	UFUNCTION(BlueprintCallable, Category="Ability|Tasks", meta=(DisplayName = "AT_HitCheck"))
	static UBattleAbilityTask_HitCheck* CreateTask(UGameplayAbility* OwningAbility);

	virtual void Activate() override;
	virtual void OnDestroy(bool bInOwnerFinished) override;

	virtual void TickTask(float DeltaTime) override;

public:
	UPROPERTY()
	FHitCheckDelegate OnHitChecked;
	
};
