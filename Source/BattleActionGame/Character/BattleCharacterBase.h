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

	UFUNCTION(BlueprintCallable, Category="Battle|Character")
	virtual UBattleAbilitySystemComponent* GetBattleAbilitySystemComponent() const;
	
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

	
protected:

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Battle|Character", Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UBattleHealthComponent> HealthComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Battle|Character", Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UBattleCombatManagerComponent> CombatComponent;
	
};

