#pragma once

#include "AbilitySystemInterface.h"
#include "ModularCharacter.h"

#include "BattleCharacterBase.generated.h"

class UBattleCombatManagerComponent;
class UBattleHealthComponent;

UCLASS()
class ABattleCharacterBase : public AModularCharacter, public IAbilitySystemInterface
{
	GENERATED_BODY()
public:

	ABattleCharacterBase(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	UFUNCTION(BlueprintCallable, Category="Battle|Character")
	virtual UBattleAbilitySystemComponent* GetBattleAbilitySystemComponent() const;
	
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

	virtual UBattleHealthComponent* GetHealthComponent() const;

	void NetPlayMontage(UAnimMontage* AnimMontage, float InPlayRate = 1.f, FName StartSectionName = NAME_None);

	UFUNCTION(BlueprintCallable)
	virtual void HandleDamageToPart(FName BoneName, FGameplayTag PartTag);

	UFUNCTION(BlueprintCallable)
	UPhysicalMaterial* GetPhysicalSurface(EPhysicalSurface PhysicalSurfaceType)
	{
		if (UPhysicalMaterial* PhysicalMaterial = *PhysicalSurfaceMap.Find(PhysicalSurfaceType))
		{
			return PhysicalMaterial;
		}

		return nullptr;
	}

	UFUNCTION(BlueprintCallable)
	void NetStopMotion(float StopSeconds);

	UFUNCTION(BlueprintCallable)
	void PerformGroggy();

	UFUNCTION(BlueprintCallable)
	void PerformPoiseBreak();
	
protected:
	
	UFUNCTION(NetMulticast, Unreliable)
	void MulticastPlayMontage(UAnimMontage* AnimMontage, float InPlayRate, FName StartSectionName);

	UFUNCTION(Server, Reliable)
	void ServerPlayMontage(UAnimMontage* AnimMontage, float InPlayRate, FName StartSectionName);

	UFUNCTION(NetMulticast, Unreliable)
	void MulticastStopMotion(float StopSeconds);

	UFUNCTION(Server, Reliable)
	void ServerStopMotion(float StopSeconds);
	
	UFUNCTION()
	void ResumeMotion();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Battle|Character", Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UBattleHealthComponent> HealthComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Battle|Character", Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UBattleCombatManagerComponent> CombatComponent;

	UPROPERTY(EditAnywhere)
	TMap<TEnumAsByte<EPhysicalSurface>, UPhysicalMaterial*> PhysicalSurfaceMap;

	FTimerHandle StopMotionHandle;
	
	
};

