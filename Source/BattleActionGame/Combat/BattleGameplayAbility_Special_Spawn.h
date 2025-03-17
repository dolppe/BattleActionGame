#pragma once

#include "GameplayMessageSubsystem.h"
#include "BattleActionGame/AbilitySystem/Abilities/BattleGameplayAbility.h"
#include "BattleGameplayAbility_Special_Spawn.generated.h"

struct FSpecialSpawnData;

UCLASS()
class UBattleGameplayAbility_Special_Spawn : public UBattleGameplayAbility
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

	UFUNCTION(BlueprintImplementableEvent)
	void OnSpawnStart();

	UFUNCTION()
	void OnRep_SpawnCenterData();
		
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing=OnRep_SpawnCenterData)
	TArray<FVector> SpawnCenterData;

	const FSpecialSpawnData* CurrentSpawnData;
	
	UFUNCTION()
	void StartSpawn(FGameplayTag Channel, const FBattleVerbMessage& Notification);

	UPROPERTY()
	TSubclassOf<AActor> SpawnActorClass;
	
private:
	
	FGameplayMessageListenerHandle StartListenerHandle;
	
	UPROPERTY()
	TObjectPtr<UAnimMontage> CurrentSpawnMontage;
	
	UPROPERTY(EditAnywhere)
	int SpawnMode = 0;
	
};

