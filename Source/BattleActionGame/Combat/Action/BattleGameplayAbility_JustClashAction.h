#pragma once

#include "BattleGameplayAbility_TargetedAction_Parent.h"
#include "BattleGameplayAbility_JustClashAction.generated.h"


UCLASS()
class UBattleGameplayAbility_JustClashAction : public UBattleGameplayAbility_TargetedAction_Parent
{
	GENERATED_BODY()
public:

	UBattleGameplayAbility_JustClashAction(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	

protected:

	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;

	virtual void MoveToBestSpot() override;
	virtual void PlaySpecialActionMontage() override;
	virtual void PlayStartCue() override;

	UPROPERTY(EditAnywhere)
	UAnimMontage* TargetMontage;

	UPROPERTY(EditAnywhere)
	UAnimMontage* OwnerMontage;

	UPROPERTY(EditAnywhere)
	FGameplayTag ClashGCN;

	void DoneAddingNativeTags();

private:

	FVector ImpactPoint;
	
};
