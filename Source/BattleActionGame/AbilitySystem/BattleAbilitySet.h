#pragma once

#include "Containers/Array.h"
#include "Engine/DataAsset.h"
#include "GameplayTagContainer.h"
#include "GameplayAbilitySpec.h"
#include "Templates/SubclassOf.h"
#include "UObject/ObjectPtr.h"
#include "UObject/UObjectGlobals.h"
#include "BattleAbilitySet.generated.h"

class UBattleAbilitySystemComponent;
class UBattleGameplayAbility;
class UGameplayEffect;

USTRUCT(BlueprintType)
struct FBattleAbilitySet_GameplayAbility
{
	GENERATED_BODY()

public:

	// 허용된 어빌리티
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UBattleGameplayAbility> Ability = nullptr;

	// Input 처리를 위한 어빌리티 태그
	UPROPERTY(EditDefaultsOnly)
	FGameplayTag InputTag;

	// 어빌리티 허용 조건 레벨
	UPROPERTY(EditDefaultsOnly)
	int32 AbilityLevel = 1;
	
};

USTRUCT(BlueprintType)
struct FBattleAbilitySet_GameplayEffect
{
	GENERATED_BODY()

public:

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UGameplayEffect> GameplayEffect = nullptr;

	UPROPERTY(EditDefaultsOnly)
	float EffectLevel = 1.0f;
};

USTRUCT(BlueprintType)
struct FBattleAbilitySet_AttributeSet
{
	GENERATED_BODY()

public:

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UAttributeSet> AttributeSet;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TSoftObjectPtr<UDataTable> InitializationData;

};

USTRUCT()
struct FBattleAbilitySet_GrantedHandles
{
	GENERATED_BODY()

	void AddAbilitySpecHandle(const FGameplayAbilitySpecHandle& Handle);
	void AddGameplayEffectHandle(const FActiveGameplayEffectHandle& Handle);
	void AddAttributeSet(UAttributeSet* Set);

	
	void TakeFromAbilitySystem(UBattleAbilitySystemComponent* BattleASC);
	
protected:
	UPROPERTY()
	TArray<FGameplayAbilitySpecHandle> AbilitySpecHandles;
	
	UPROPERTY()
	TArray<FActiveGameplayEffectHandle> GameplayEffectHandles;
	
	UPROPERTY()
	TArray<TObjectPtr<UAttributeSet>> GrantedAttributeSets;
	
};

// Gameplay Ability를 좀 더 쉽게 관리하기 위한 Set
UCLASS(BlueprintType)
class UBattleAbilitySet : public UPrimaryDataAsset
{
	GENERATED_BODY()
public:
	UBattleAbilitySet(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	void GiveToAbilitySystem(UBattleAbilitySystemComponent* BattleASC, FBattleAbilitySet_GrantedHandles* OutGrantedHandles, UObject* SourceObject= nullptr) const;

protected:
	
	// 허용된 GameplayAbilities
	UPROPERTY(EditDefaultsOnly, Category="Gameplay Abilities")
	TArray<FBattleAbilitySet_GameplayAbility> GrantedGameplayAbilities;

	UPROPERTY(EditDefaultsOnly, Category = "Gameplay Effects", meta=(TitleProperty=GameplayEffect))
	TArray<FBattleAbilitySet_GameplayEffect> GrantedGameplayEffects;

	// Attribute sets to grant when this ability set is granted.
	UPROPERTY(EditDefaultsOnly, Category = "Attribute Sets", meta=(TitleProperty=AttributeSet))
	TArray<FBattleAbilitySet_AttributeSet> GrantedAttributes;
	
};