#include "BattleItemData_AttributeBased.h"

#include "BattleActionGame/BattleGameplayTags.h"
#include "Misc/DataValidation.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(BattleItemData_AttributeBased)

UBattleItemData_AttributeBased::UBattleItemData_AttributeBased()
{
	ItemType = EItemType::AttributeBased;
}
#if WITH_EDITOR
EDataValidationResult UBattleItemData_AttributeBased::IsDataValid(class FDataValidationContext& Context)
{
	EDataValidationResult Result = Super::IsDataValid(Context);

	if (AttributeData.IsEmpty())
	{
		Context.AddError(FText::FromString(TEXT("UBattleItemData_AttributeBased must have at least one AttributeData")));
		Result = EDataValidationResult::Invalid;
	}
	else
	{
		for (FAttributeData& Attribute : AttributeData)
		{
			if (Attribute.AttributeMag.IsEmpty())
			{
				Context.AddError(FText::FromString(TEXT("UBattleItemData_AttributeBased-AttributeData must have at least one AttributeMag")));
				Result = EDataValidationResult::Invalid;
			}
			else
			{
				for (FAttributeMagData& AttributeMagData : Attribute.AttributeMag)
				{
					if (!AttributeMagData.AttributeTag.IsValid())
					{
						Context.AddError(FText::FromString(TEXT("UBattleItemData_AttributeBased-AttributeTag must have valid AttributeTag ")));
						Result = EDataValidationResult::Invalid;
					}
				}
			}
		
		}
	}


	return Result;
}
#endif

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
