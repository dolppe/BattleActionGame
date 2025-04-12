#pragma once

#include "BattleGameplayAbility.h"
#include "BattleGameplayAbility_JustDash.generated.h"


UCLASS()
class UBattleGameplayAbility_JustDash : public UBattleGameplayAbility
{
	GENERATED_BODY()
public:

	UBattleGameplayAbility_JustDash(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:

	void DoneAddingNativeTags();

	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;


	UPROPERTY(EditAnywhere)
	FGameplayTag OwnerJustDashEffectTag;

	UPROPERTY(EditAnywhere)
	FGameplayTag GlobalJustDashEffectTag;

	UPROPERTY(EditAnywhere)
	TSubclassOf<UGameplayEffect> GE_AllowedCounterAttack;
	
};
