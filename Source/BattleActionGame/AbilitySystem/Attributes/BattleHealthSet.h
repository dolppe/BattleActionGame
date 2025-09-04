#pragma once

#include "BattleAttributeSet.h"
#include "AbilitySystemComponent.h"
#include "NativeGameplayTags.h"
#include "BattleHealthSet.generated.h"

UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Gameplay_DamageImmunity);

UCLASS(BlueprintType)
class UBattleHealthSet : public UBattleAttributeSet
{
	GENERATED_BODY()
public:

	UBattleHealthSet();

	ATTRIBUTE_ACCESSORS(UBattleHealthSet, Health);
	ATTRIBUTE_ACCESSORS(UBattleHealthSet, MaxHealth);
	ATTRIBUTE_ACCESSORS(UBattleHealthSet, Stamina);
	ATTRIBUTE_ACCESSORS(UBattleHealthSet, MaxStamina);
	ATTRIBUTE_ACCESSORS(UBattleHealthSet, Damage);
	ATTRIBUTE_ACCESSORS(UBattleHealthSet, Healing);

	mutable FBattleAttributeEvent OnOutOfHealth;

protected:

	UFUNCTION()
	void OnRep_Health(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	void OnRep_MaxHealth(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	void OnRep_Stamina(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	void OnRep_MaxStamina(const FGameplayAttributeData& OldValue);

	virtual bool PreGameplayEffectExecute(FGameplayEffectModCallbackData& Data) override;
	virtual void PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data) override;

	virtual void PreAttributeBaseChange(const FGameplayAttribute& Attribute, float& NewValue) const override;
	virtual void PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue) override;
	virtual void PostAttributeChange(const FGameplayAttribute& Attribute, float OldValue, float NewValue) override;

	void ClampAttribute(const FGameplayAttribute& Attribute, float& NewValue) const;

private:

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing=OnRep_Health, Category="Battle|Health", Meta = (HideFromModifiers, AllowPrivateAccess = true))
	FGameplayAttributeData Health;

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing=OnRep_MaxHealth, Category="Battle|Health", Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData MaxHealth;

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing=OnRep_Stamina, Category="Battle|Health", Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData Stamina;

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing=OnRep_MaxStamina, Category="Battle|Health", Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData MaxStamina;

	bool bOutOfHealth;
	
private:
	
	UPROPERTY(BlueprintReadOnly, Category="Battle|Health", Meta=(AllowPrivateAccess=true))
	FGameplayAttributeData Healing;
	
	UPROPERTY(BlueprintReadOnly, Category="Battle|Health", Meta=(AllowPrivateAccess=true))
	FGameplayAttributeData Damage;
	

};
