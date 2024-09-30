#pragma once

#include "AttributeSet.h"
#include "Delegates/Delegate.h"
#include "BattleAttributeSet.generated.h"


class UBattleAbilitySystemComponent;
struct FGameplayEffectSpec;
class AActor;


#define ATTRIBUTE_ACCESSORS(ClassName, PropertyName) \
	GAMEPLAYATTRIBUTE_PROPERTY_GETTER(ClassName, PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_GETTER(PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_SETTER(PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_INITTER(PropertyName)


DECLARE_MULTICAST_DELEGATE_FourParams(FBattleAttributeEvent, AActor* /*EffectInstigator*/, AActor* /*EffectCauser*/, const FGameplayEffectSpec& /*EffectSpec*/, float /*EffectMagnitude*/);

UCLASS()
class BATTLEACTIONGAME_API UBattleAttributeSet : public UAttributeSet
{
	GENERATED_BODY()
public:

	UBattleAttributeSet();

	UBattleAbilitySystemComponent* GetBattleAbilitySystemComponent() const;
	
};
