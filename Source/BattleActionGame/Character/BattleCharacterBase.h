#pragma once

#include "AbilitySystemInterface.h"
#include "ModularCharacter.h"

#include "BattleCharacterBase.generated.h"

struct FGameplayTag;
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

	void NetSetControlRotation(const FRotator& NewRotation);
	
	void NetPlayMontage(UAnimMontage* AnimMontage, float InPlayRate = 1.f, FName StartSectionName = NAME_None);
	
	void NetJumpToSection(UAnimMontage* AnimMontage, FName SectionName);

	UFUNCTION(BlueprintCallable)
	virtual void HandleDamageToPart(FGameplayTag PartTag, const FVector& AttackDirection);

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
	void NetStopMotion(float StopSeconds, float TimeDilation = 0.0f);

	UFUNCTION(BlueprintCallable)
	void PerformGroggy();

	UFUNCTION(BlueprintCallable)
	void PerformPoiseBreak();
	
	virtual void OnGroggy();
	
	virtual void OnPoiseBreak();
	
	
	
	FTimerHandle StopMotionHandle;
	
protected:
	
	UFUNCTION(NetMulticast, Unreliable)
	void MulticastPlayMontage(UAnimMontage* AnimMontage, float InPlayRate, FName StartSectionName= NAME_None);

	UFUNCTION(Server, Reliable)
	void ServerPlayMontage(UAnimMontage* AnimMontage, float InPlayRate, FName StartSectionName);
	
	UFUNCTION(NetMulticast, Reliable)
	void MulticastJumpToSection(UAnimMontage* AnimMontage, FName SectionName);
	
	UFUNCTION(Server, Reliable)
	void ServerJumpToSection(UAnimMontage* AnimMontage, FName SectionName);

	UFUNCTION(NetMulticast, Unreliable)
	void MulticastStopMotion(float StopSeconds, float TimeDilation = 0.0f);

	UFUNCTION(Server, Reliable)
	void ServerStopMotion(float StopSeconds, float TimeDilation = 0.0f);

	UFUNCTION(Client, Reliable)
	void ClientSetControlRotation(const FRotator& NewRotation);
	
	UFUNCTION()
	void ResumeMotion();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Battle|Character", Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UBattleHealthComponent> HealthComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Battle|Character", Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UBattleCombatManagerComponent> CombatComponent;

	UPROPERTY(EditAnywhere)
	TMap<TEnumAsByte<EPhysicalSurface>, UPhysicalMaterial*> PhysicalSurfaceMap;


	
	
};

