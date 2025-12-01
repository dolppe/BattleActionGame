#include "BattleAttributeSet.h"
#include "BattleActionGame/AbilitySystem/BattleAbilitySystemComponent.h"
#include UE_INLINE_GENERATED_CPP_BY_NAME(BattleAttributeSet)

TArray<FAttributeInfo> UBattleAttributeSetHelper::GetAttributeInfos(UAttributeSet* AttributeSet)
{
	TArray<FAttributeInfo> Result;

	if (!AttributeSet)
		return Result;

	UClass* Class = AttributeSet->GetClass();

	for (TFieldIterator<FProperty> PropertyIt(Class); PropertyIt; ++PropertyIt)
	{
		FProperty* Property = *PropertyIt;

		if (FStructProperty* StructProperty = CastField<FStructProperty>(Property))
		{
			if (StructProperty->Struct == FGameplayAttributeData::StaticStruct())
			{
				FGameplayAttributeData* AttData = StructProperty->ContainerPtrToValuePtr<FGameplayAttributeData>(AttributeSet);

				FAttributeInfo Info;

				Info.Name = Property->GetName();
				Info.BaseValue = AttData->GetBaseValue();

				Result.Add(Info);
			}
		}
		
	}

	return Result;
}

TArray<FAttributeInfo> UBattleAttributeSetHelper::GetAttributeInfosWithClass(UClass* AttributeSet)
{	TArray<FAttributeInfo> Result;
	
	for (TFieldIterator<FProperty> PropertyIt(AttributeSet); PropertyIt; ++PropertyIt)
	{
		FProperty* Property = *PropertyIt;

		if (FStructProperty* StructProperty = CastField<FStructProperty>(Property))
		{
			if (StructProperty->Struct == FGameplayAttributeData::StaticStruct())
			{
				FGameplayAttributeData* AttData = StructProperty->ContainerPtrToValuePtr<FGameplayAttributeData>(AttributeSet->GetDefaultObject());

				FAttributeInfo Info;

				Info.Name = Property->GetName();
				Info.BaseValue = AttData->GetBaseValue();

				Result.Add(Info);
			}
		}
		
	}

	return Result;
}

UBattleAttributeSet::UBattleAttributeSet()
{
}

UBattleAbilitySystemComponent* UBattleAttributeSet::GetBattleAbilitySystemComponent() const
{
	return Cast<UBattleAbilitySystemComponent>(GetOwningAbilitySystemComponent());
}

TArray<FAttributeInfo> UBattleAttributeSet::GetGameplayAttributes()
{
	TArray<FAttributeInfo> Result;

	UClass* Class = GetClass();

	for (TFieldIterator<FProperty> PropertyIt(Class); PropertyIt; ++PropertyIt)
	{
		FProperty* Property = *PropertyIt;

		if (FStructProperty* StructProperty = CastField<FStructProperty>(Property))
		{
			if (StructProperty->Struct == FGameplayAttributeData::StaticStruct())
			{
				FGameplayAttributeData* AttData = StructProperty->ContainerPtrToValuePtr<FGameplayAttributeData>(this);

				FAttributeInfo Info;

				Info.Name = Property->GetName();
				Info.BaseValue = AttData->GetBaseValue();

				Result.Add(Info);
			}
		}
		
	}

	return Result;
}
