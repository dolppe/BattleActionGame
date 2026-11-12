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

TArray<FAttributeInfo> UBattleAttributeSetHelper::GetAttributeInfosWithClass(UClass* AttributeSetClass)
{	
	TArray<FAttributeInfo> Result;
	
	if (!AttributeSetClass || !AttributeSetClass->IsChildOf(UAttributeSet::StaticClass()))
	{
		return Result;
	}
	
	// Attribute 초기 설정값 조회를 위한 CDO 확보
	const UObject* DefaultObject = AttributeSetClass->GetDefaultObject();
	
	for (TFieldIterator<FProperty> PropertyIt(AttributeSetClass); PropertyIt; ++PropertyIt)
	{
		FStructProperty* StructProperty = CastField<FStructProperty>(*PropertyIt);

		// FGameplayAttributeData 타입의 프로퍼티만 수집
		if (!StructProperty || StructProperty->Struct != FGameplayAttributeData::StaticStruct())
		{
			continue;
		}
		
		const FGameplayAttributeData* AttributeData = 
			StructProperty->ContainerPtrToValuePtr<FGameplayAttributeData>(DefaultObject);
		
		FAttributeInfo Info;

		Info.Name = StructProperty->GetName();
		Info.BaseValue = AttributeData->GetBaseValue();

		Result.Add(Info);
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
