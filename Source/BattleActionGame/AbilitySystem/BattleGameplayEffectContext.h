#pragma once

#include "BattleAbilitySourceInterface.h"
#include "GameplayEffectTypes.h"
#include "UObject/Class.h"
#include "UObject/WeakObjectPtr.h"
#include "UObject/WeakObjectPtrTemplates.h"
#include "BattleGameplayEffectContext.generated.h"

class FArchive;
class IBattleAbilitySourceInterface;

USTRUCT()
struct FBattleGameplayEffectContext : public FGameplayEffectContext
{
	GENERATED_BODY()

	FBattleGameplayEffectContext()
		: FGameplayEffectContext()
	{
	}
	FBattleGameplayEffectContext(AActor* InInstigator, AActor* InEffectCauser)
		: FGameplayEffectContext(InInstigator, InEffectCauser)
	{
	}

	static BATTLEACTIONGAME_API FBattleGameplayEffectContext* ExtractEffectContext(struct FGameplayEffectContextHandle Handle);

	void SetAbilitySource(const IBattleAbilitySourceInterface* InObject, float InSourceLevel);
	const IBattleAbilitySourceInterface* GetAbilitySource() const;

	const UPhysicalMaterial* GetPhysicalMaterial() const;

	virtual FGameplayEffectContext* Duplicate() const override
	{
		FBattleGameplayEffectContext* NewContext = new FBattleGameplayEffectContext();
		*NewContext = *this;
		if (GetHitResult())
		{
			// Does a deep copy of the hit result
			NewContext->AddHitResult(*GetHitResult(), true);
		}
		return NewContext;
	}
	
	virtual bool NetSerialize(FArchive& Ar, class UPackageMap* Map, bool& bOutSuccess) override;

	virtual UScriptStruct* GetScriptStruct() const override
	{
		return FBattleGameplayEffectContext::StaticStruct();
	}

	
protected:
	// IBattleAbilitySourceObject
	UPROPERTY()
	TWeakObjectPtr<const UObject> AbilitySourceObject;
	
};


template<>
struct TStructOpsTypeTraits<FBattleGameplayEffectContext> : public TStructOpsTypeTraitsBase2<FBattleGameplayEffectContext>
{
	enum
	{
		WithNetSerializer = true,
		WithCopy = true
	};
};

