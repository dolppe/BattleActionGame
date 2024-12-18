#include "BattleGameplayEffectContext.h"

#include "BattleAbilitySourceInterface.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(BattleGameplayEffectContext)

FBattleGameplayEffectContext* FBattleGameplayEffectContext::ExtractEffectContext(FGameplayEffectContextHandle Handle)
{
	FGameplayEffectContext* BaseEffectContext = Handle.Get();
	if ((BaseEffectContext != nullptr) && BaseEffectContext->GetScriptStruct()->IsChildOf(FBattleGameplayEffectContext::StaticStruct()))
	{
		return (FBattleGameplayEffectContext*) BaseEffectContext;
	}
	
	return nullptr;
	
}

void FBattleGameplayEffectContext::SetAbilitySource(const IBattleAbilitySourceInterface* InObject, float InSourceLevel)
{
	AbilitySourceObject = MakeWeakObjectPtr(Cast<const UObject>(InObject));
}

const IBattleAbilitySourceInterface* FBattleGameplayEffectContext::GetAbilitySource() const
{
	return Cast<IBattleAbilitySourceInterface>(AbilitySourceObject.Get());
}

const UPhysicalMaterial* FBattleGameplayEffectContext::GetPhysicalMaterial() const
{
	if (const FHitResult* HitResultPtr = GetHitResult())
	{
		return HitResultPtr->PhysMaterial.Get();
	}
	return nullptr;
}

bool FBattleGameplayEffectContext::NetSerialize(FArchive& Ar, UPackageMap* Map, bool& bOutSuccess)
{
	FGameplayEffectContext::NetSerialize(Ar, Map, bOutSuccess);

	return true;
}
