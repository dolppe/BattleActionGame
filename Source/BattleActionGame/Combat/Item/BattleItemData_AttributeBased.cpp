#include "BattleItemData_AttributeBased.h"

#include "BattleActionGame/BattleGameplayTags.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(BattleItemData_AttributeBased)

UBattleItemData_AttributeBased::UBattleItemData_AttributeBased()
{
	ItemType = EItemType::AttributeBased;
}


TArray<FGameplayTag>& UBattleItemData_AttributeBased::GetAttributeTags()
{
	static TArray<FGameplayTag> AttributeTags = 
	{	
		FBattleGameplayTags::Get().GameplayEffect_Caller_MaxHp,
		FBattleGameplayTags::Get().GameplayEffect_Caller_Stamina,
		FBattleGameplayTags::Get().GameplayEffect_Caller_MaxStamina,
		FBattleGameplayTags::Get().GameplayEffect_Caller_Healing,
		FBattleGameplayTags::Get().GameplayEffect_Caller_Damage,
		FBattleGameplayTags::Get().GameplayEffect_Caller_AttackPower
	};
	
	return AttributeTags;
	
}
