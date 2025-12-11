#pragma once

#include "BattleCombatData.h"
#include "Components/PawnComponent.h"
#include "Animation/AnimMontage.h"
#include "Item/BattleQuickBarComponent.h"
#include "BattleCombatManagerComponent.generated.h"

struct FBattleHitMessage;
class UBattleGameplayAbility_Attack_Parent;
class UAttackCollisionMethod;
enum class ECollisionMethodType : uint8;
class UBattleGameplayAbility_ComboAttack;
enum class EItemType : uint8;
class UGameplayAbility;
enum class EAttackType : uint8;

USTRUCT(BlueprintType)
struct FHitInfo
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly)
	FVector HitDirection;

	UPROPERTY(BlueprintReadOnly)
	float HitTime;
	
};



UCLASS(Blueprintable, Meta=(BlueprintSpawnableComponent))
class UBattleCombatManagerComponent : public UPawnComponent
{
	GENERATED_BODY()

public:

	UBattleCombatManagerComponent(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	virtual void BeginPlay() override;

	UFUNCTION(BlueprintCallable)
	AActor* GetCurrentTargetActor()
	{
		return CurrentTargetActor;
	}

	UFUNCTION(BlueprintCallable)
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
	void OnEndAbilityComboGA();

	UFUNCTION()
	void OnRep_CurrentUsedItemInfo();

	UFUNCTION(BlueprintCallable)
	UAttackCollisionMethod* GetCollisionMethod(ECollisionMethodType CollisionMethod);

	UPROPERTY(ReplicatedUsing=OnRep_CurrentUsedItemInfo, BlueprintReadWrite)
	FBattleItemInfo CurrentUsedItemInfo;

	UPROPERTY(EditAnywhere)
	TObjectPtr<UBattleCombatData> CombatData;

	void SetAreaCenterData(const TArray<FVector>& InAreaCenterData)
	{
		AreaCenterData = InAreaCenterData;
	}

	TArray<FVector>& GetAreaCenterData()
	{
		return AreaCenterData;
	}

	void SetLastHitInfo(const FHitInfo& InHitInfo)
	{
		LastHitInfo = InHitInfo;
	}

	UFUNCTION(BlueprintCallable)
	const FHitInfo& GetLastHitInfo() const
	{
		return LastHitInfo;
	}

	void SetCurrentAttackGA(UBattleGameplayAbility_Attack_Parent* InCurrentAttackGA)
	{
		CurrentAttackGA = InCurrentAttackGA;
	}

	UFUNCTION(BlueprintCallable)
	void OnHitEvent(const FBattleHitMessage& HitMessage);

	UFUNCTION(BlueprintCallable)
	void AllowGuardEvent();

	UFUNCTION(Server, Reliable)
	void TryJustGuard(AActor* TryActor);

	UFUNCTION()
	void OnAttackWarnSign();

protected:

	UFUNCTION()
	void OnRep_AreaCenterData();

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing=OnRep_AreaCenterData)
	TArray<FVector> AreaCenterData;

private:

	UFUNCTION()
	void OnRep_CurrentTargetActor();

	UFUNCTION()
	void OnRep_LastHitInfo();

	UFUNCTION()
	void OnRep_CurrentCombo();

	UPROPERTY(ReplicatedUsing=OnRep_CurrentTargetActor)
	TObjectPtr<AActor> CurrentTargetActor = nullptr;
	
	UPROPERTY(ReplicatedUsing=OnRep_CurrentCombo)
	UBattleGameplayAbility_ComboAttack* CurrentCombo = nullptr;
	
	uint8 ComboStep = 0;

	UPROPERTY(EditAnywhere)
	TMap<EItemType, TSubclassOf<UGameplayAbility>> ItemTypeToAbility;

	UPROPERTY()
	TMap<ECollisionMethodType, UAttackCollisionMethod*> InstancedCollisionMethod;

	UPROPERTY(ReplicatedUsing=OnRep_LastHitInfo)
	FHitInfo LastHitInfo;
	
	UBattleGameplayAbility_Attack_Parent* CurrentAttackGA = nullptr;

	FTimerHandle ComboEndTimerHandle;


};