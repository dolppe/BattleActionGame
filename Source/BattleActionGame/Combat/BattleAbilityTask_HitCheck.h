#pragma once

#include "Abilities/Tasks/AbilityTask.h"
#include "BattleActionGame/Physics/BattleCollisionChannels.h"
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

	UFUNCTION()
	void SetHitCheckData(FString InStartName, FString InEndName, float InAttackRadius, TEnumAsByte<ECollisionChannel> InCollisionChannel);

	virtual void Activate() override;
	virtual void OnDestroy(bool bInOwnerFinished) override;

	virtual void TickTask(float DeltaTime) override;

public:
	UPROPERTY()
	FHitCheckDelegate OnHitChecked;

	FString StartName = TEXT("weapon");
	FString EndName = TEXT("weapon_end");
	float AttackRadius = 10.f;
	TEnumAsByte<ECollisionChannel> CollisionChannel = Battle_TraceChannel_AttackToEnemy;
	
	
};
