#pragma once

#include "AttributeSet.h"
#include "Delegates/Delegate.h"
#include "Kismet/BlueprintFunctionLibrary.h"
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

USTRUCT(BlueprintType)
struct FAttributeInfo
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly)
	FString Name;

	UPROPERTY(BlueprintReadOnly)
	float BaseValue;
	
};

UCLASS()
class BATTLEACTIONGAME_API UBattleAttributeSetHelper : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
public:

	UFUNCTION(BlueprintCallable)
	static TArray<FAttributeInfo> GetAttributeInfos(UAttributeSet* AttributeSet);

	UFUNCTION(BlueprintCallable)
	static TArray<FAttributeInfo> GetAttributeInfosWithClass(UClass* AttributeSet);
	
};

UCLASS()
class BATTLEACTIONGAME_API UBattleAttributeSet : public UAttributeSet
{
	GENERATED_BODY()
public:

	UBattleAttributeSet();

	UBattleAbilitySystemComponent* GetBattleAbilitySystemComponent() const;

	UFUNCTION(BlueprintCallable)
	TArray<FAttributeInfo> GetGameplayAttributes();

	
	
};
