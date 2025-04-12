#pragma once

#include "BattleCombatData.h"
#include "Components/PawnComponent.h"
#include "Animation/AnimMontage.h"
#include "Item/BattleQuickBarComponent.h"
#include "BattleCombatManagerComponent.generated.h"

class UAttackCollisionMethod;
enum class ECollisionMethodType : uint8;
class UBattleGameplayAbility_ComboAttack;
enum class EItemType : uint8;
class UGameplayAbility;
enum class EAttackType : uint8;



UCLASS(Blueprintable, Meta=(BlueprintSpawnableComponent))
class UBattleCombatManagerComponent : public UPawnComponent
{
	GENERATED_BODY()

public:

	UBattleCombatManagerComponent(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	virtual void BeginPlay() override;

	TObjectPtr<AActor> GetCurrentTargetActor()
	{
		return CurrentTargetActor;
	}

	void SetCurrentTargetActor(AActor* InCurrentTargetActor)
	{
		CurrentTargetActor = InCurrentTargetActor;
	}

	UFUNCTION(BlueprintCallable)
	void UseItem(EItemType Item);
	
	UFUNCTION(BlueprintCallable)
	UBattleCombatData* GetAttackData() const
	{
		return CombatData;
	}

	int GetCurrentComboIndex();

	void SetComboGA(UBattleGameplayAbility_ComboAttack* InComboAttack);

	UFUNCTION()
	void OnRep_CurrentUsedItemInfo();

	UFUNCTION(BlueprintCallable)
	UAttackCollisionMethod* GetCollisionMethod(ECollisionMethodType CollisionMethod);

	UPROPERTY(ReplicatedUsing=OnRep_CurrentUsedItemInfo, BlueprintReadWrite)
	FBattleItemInfo CurrentUsedItemInfo;

	UPROPERTY(EditAnywhere)
	TObjectPtr<UBattleCombatData> CombatData;

private:

	UFUNCTION()
	void OnRep_CurrentTargetActor();

	UPROPERTY(ReplicatedUsing=OnRep_CurrentTargetActor)
	TObjectPtr<AActor> CurrentTargetActor = nullptr;
	
	UPROPERTY()
	UBattleGameplayAbility_ComboAttack* CurrentCombo = nullptr;
	
	uint8 ComboStep = 0;

	UPROPERTY(EditAnywhere)
	TMap<EItemType, TSubclassOf<UGameplayAbility>> ItemTypeToAbility;

	UPROPERTY()
	TMap<ECollisionMethodType, UAttackCollisionMethod*> InstancedCollisionMethod;
	

};