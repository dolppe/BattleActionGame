#pragma once

#include "BattleGameplayAbility_Attack_Parent.h"
#include "BattleGameplayAbility_TargetedAttack.generated.h"

struct FTargetedAttack;

UCLASS()
class UBattleGameplayAbility_TargetedAttack : public UBattleGameplayAbility_Attack_Parent
{
	GENERATED_BODY()
public:

	UBattleGameplayAbility_TargetedAttack(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
	
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	void SetAreaCenterData(const TArray<FVector>& InAreaCenterData)
	{
		AreaCenterData = InAreaCenterData;
	}


protected:

	virtual void OnTargetDataReadyCallback(const FGameplayAbilityTargetDataHandle& InData, FGameplayTag ApplicationTag) override;

	UFUNCTION(BlueprintImplementableEvent)
	void OnTargetDataReady(const FGameplayAbilityTargetDataHandle& TargetData);

	virtual void OnCompleted() override;
	virtual void OnInterrupted() override;

	virtual void OnRep_AlreadyHitActors() override;

	UFUNCTION()
	void OnRep_AreaCenterData();

	UFUNCTION()
	virtual void StartHitCheck(FGameplayTag Channel, const FBattleVerbMessage& Notification) override;
	
	UFUNCTION()
	virtual void EndHitCheck(FGameplayTag Channel, const FBattleVerbMessage& Notification) override;
	
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing=OnRep_AreaCenterData)
	TArray<FVector> AreaCenterData;

	UPROPERTY(BlueprintReadOnly)
	float AttackRadius = 10.f;

private:
	
	const FTargetedAttack* CurrentAttackData;

	
	
	
	
};
