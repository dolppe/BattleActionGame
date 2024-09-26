#pragma once

#include "BattleComboData.h"
#include "Components/PawnComponent.h"
#include "Animation/AnimMontage.h"
#include "BattleGameplayAbility_ComboAttack.h"
#include "BattleCombatManagerComponent.generated.h"


UCLASS(Blueprintable, Meta=(BlueprintSpawnableComponent))
class UBattleCombatManagerComponent : public UPawnComponent
{
	GENERATED_BODY()

public:

	UBattleCombatManagerComponent(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	UBattleComboData* GetComboData(int idx)
	{
		return ComboData[idx];
	}
	UAnimMontage* GetComboMontage(int idx)
	{
		return ComboAttackMontages[idx];
	}

	UBattleComboData* GetAttackData(int idx)
	{
		return AttackData[idx];
	}

	UAnimMontage* GetAttackMontage(int idx)
	{
		return AttackMontages[idx];
	}

	int GetCurrentComboIndex();

	void SetComboGA(UBattleGameplayAbility_ComboAttack* InComboAttack);
	
	

	

private:

	/*
	 * 0 => 일반공격
	 */
	
	UPROPERTY(EditDefaultsOnly)
	TArray<UBattleComboData*> ComboData;
	
	UPROPERTY(EditDefaultsOnly)
	TArray<UAnimMontage*> ComboAttackMontages;

	/*
	* 0 => 강공격
	*/

	UPROPERTY(EditDefaultsOnly)
	TArray<UBattleComboData*> AttackData;

	UPROPERTY(EditDefaultsOnly)
	TArray<UAnimMontage*> AttackMontages;

	UPROPERTY()
	UBattleGameplayAbility_ComboAttack* CurrentCombo = nullptr;
	
	uint8 ComboStep = 0;

};