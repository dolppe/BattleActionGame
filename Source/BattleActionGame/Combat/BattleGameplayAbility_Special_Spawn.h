#pragma once

#include "GameplayMessageSubsystem.h"
#include "BattleActionGame/AbilitySystem/Abilities/BattleGameplayAbility.h"
#include "BattleActionGame/Combat/Action/BattleGameplayAbility_SpecialAction_VerbMessage.h"
#include "BattleGameplayAbility_Special_Spawn.generated.h"

struct FSpecialSpawnData;

UCLASS()
class UBattleGameplayAbility_Special_Spawn : public UBattleGameplayAbility_SpecialAction_VerbMessage
{
	GENERATED_BODY()
public:

	UBattleGameplayAbility_Special_Spawn(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
	int GetSpawnMode() const
	{
		return SpawnMode;
	}

	void SetSpawnCenterDAta(const TArray<FVector>& InSpawnCenterData)
	{
		SpawnCenterData = InSpawnCenterData;
	}

protected:

	UFUNCTION()
	virtual void OnCompleted();
	UFUNCTION()
	virtual void OnInterrupted();
	UFUNCTION()
	virtual void OnBlendOut();

	UFUNCTION()
	void OnRep_SpawnCenterData();
		
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing=OnRep_SpawnCenterData)
	TArray<FVector> SpawnCenterData;

	const FSpecialSpawnData* CurrentSpawnData;

	virtual void StartActionTrigger(FGameplayTag Channel, const FBattleVerbMessage& Notification) override;

	UPROPERTY()
	TSubclassOf<AActor> SpawnActorClass;
	
private:

	
	UPROPERTY()
	TObjectPtr<UAnimMontage> CurrentSpawnMontage;
	
	UPROPERTY(EditAnywhere)
	int SpawnMode = 0;
	
};

