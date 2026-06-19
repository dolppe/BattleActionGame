#pragma once
#include "BattleGameplayAbility.h"

#include "BattleGameplayAbility_ConvertToAI.generated.h"

class UBattlePawnData;

UCLASS()
class UBattleGameplayAbility_ConvertToAI : public UBattleGameplayAbility
{
	GENERATED_BODY()
public:
	
	UBattleGameplayAbility_ConvertToAI(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
	
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;
	
	bool CheckValidPawnDataForConvertToAI(const UBattlePawnData& PawnData);
	
};


