#pragma once

#include "BattleCombatData.h"
#include "Components/PawnComponent.h"
#include "Animation/AnimMontage.h"
#include "Item/BattleQuickBarComponent.h"
#include "BattleCombatManagerComponent.generated.h"

class UBattleGameplayAbility_ComboAttack;
enum class EItemType : uint8;
class UGameplayAbility;

UENUM()
enum class EAttackType : uint8
{
	Single,
	Combo,
	HitCheck,
};


UCLASS(Blueprintable, Meta=(BlueprintSpawnableComponent))
class UBattleCombatManagerComponent : public UPawnComponent
{
	GENERATED_BODY()

public:

	UBattleCombatManagerComponent(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	virtual void BeginPlay() override;

	UFUNCTION(BlueprintCallable)
	void UseItem(EItemType Item);
	
	UFUNCTION(BlueprintCallable)
	UBattleCombatData* GetAttackData() const
	{
		return CombatData;
	}

	UFUNCTION(BlueprintCallable)
	UAnimMontage* GetAttackMontage(EAttackType AttackType, int Idx) const
	{
		switch (AttackType)
		{
		case EAttackType::Single:
			return CombatData->SingleAttacks[Idx].Montage;
		case EAttackType::Combo:
			return CombatData->ComboAttacks[Idx].Montage;
		case EAttackType::HitCheck:
			return CombatData->HitCheckAttacks[Idx].Montage;
		}
		return nullptr;
	}

	int GetCurrentComboIndex();

	void SetComboGA(UBattleGameplayAbility_ComboAttack* InComboAttack);

	UFUNCTION()
	void OnRep_CurrentUsedItemInfo();

	UPROPERTY(ReplicatedUsing=OnRep_CurrentUsedItemInfo, BlueprintReadWrite)
	FBattleItemInfo CurrentUsedItemInfo;

	UPROPERTY(EditAnywhere)
	TObjectPtr<UBattleCombatData> CombatData;

private:
	
	
	UPROPERTY()
	UBattleGameplayAbility_ComboAttack* CurrentCombo = nullptr;
	
	uint8 ComboStep = 0;

	UPROPERTY(EditAnywhere)
	TMap<EItemType, TSubclassOf<UGameplayAbility>> ItemTypeToAbility;


	

};