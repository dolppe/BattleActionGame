#include "BattleEditorValidator.h"

#include "GameplayCueNotify_Actor.h"
#include "GameplayCueNotify_Static.h"
#include "InputMappingContext.h"
#include "PlayerMappableInputConfig.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(BattleEditorValidator)

bool UBattleEditorValidator::CanValidateAsset_Implementation(UObject* InAsset) const
{
	if (InAsset->GetClass() == AGameplayCueNotify_Actor::StaticClass())
	{
		return true;
	}
	if (InAsset->GetClass() == UGameplayCueNotify_Static::StaticClass())
	{
		return true;
	}
	if (InAsset->GetClass() == UPlayerMappableInputConfig::StaticClass())
	{
		return true;
	}
	if (InAsset->GetClass() == UInputMappingContext::StaticClass())
	{
		return true;
	}
	if (InAsset->GetClass() == UDataTable::StaticClass())
	{
		UDataTable* DataTable = Cast<UDataTable>(InAsset);
		if (DataTable->GetRowStruct() == FAttributeMetaData::StaticStruct())
		{
			return true;
		}
	}
	
	return false;
}

EDataValidationResult UBattleEditorValidator::ValidateLoadedAsset_Implementation(UObject* InAsset,
                                                                                 TArray<FText>& ValidationErrors)
{
	if (InAsset->GetClass() == AGameplayCueNotify_Actor::StaticClass())
	{
		AGameplayCueNotify_Actor* GCN_Actor = Cast<AGameplayCueNotify_Actor>(InAsset);
		if (GCN_Actor->GameplayCueTag.IsValid())
		{
			AssetPasses(InAsset);
			return EDataValidationResult::Valid;
		}
		else
		{
			AssetFails(InAsset, FText::FromString("GCN must have GCNTag"),ValidationErrors);
			return EDataValidationResult::Invalid;
		}
	}
	if (InAsset->GetClass() == UGameplayCueNotify_Static::StaticClass())
	{
		UGameplayCueNotify_Static* GCN_Actor = Cast<UGameplayCueNotify_Static>(InAsset);
		if (GCN_Actor->GameplayCueTag.IsValid())
		{
			AssetPasses(InAsset);
			return EDataValidationResult::Valid;
		}
		else
		{
			AssetFails(InAsset, FText::FromString("GCN must have GCNTag"),ValidationErrors);
			return EDataValidationResult::Invalid;
		}
	}
	if (InAsset->GetClass() == UPlayerMappableInputConfig::StaticClass())
	{
		UPlayerMappableInputConfig* PMI = Cast<UPlayerMappableInputConfig>(InAsset);
		if (PMI->GetMappingContexts().IsEmpty())
		{
			AssetFails(InAsset, FText::FromString("PMI must have Context"),ValidationErrors);
			return EDataValidationResult::Invalid;
		}
		else
		{
			AssetPasses(InAsset);
			return EDataValidationResult::Valid;
		}
	}
	if (InAsset->GetClass() == UInputMappingContext::StaticClass())
	{
		UInputMappingContext* IMC = Cast<UInputMappingContext>(InAsset);
		if (IMC->GetMappings().IsEmpty())
		{
			AssetFails(InAsset, FText::FromString("IMC must have Mappings"),ValidationErrors);
			return EDataValidationResult::Invalid;
		}
		else
		{
			AssetPasses(InAsset);
			return EDataValidationResult::Valid;
		}
	}
	if (InAsset->GetClass() == UDataTable::StaticClass())
	{
		UDataTable* DataTable = Cast<UDataTable>(InAsset);
		
		bool bHasError = false;
		if (DataTable->GetRowStruct() == FAttributeMetaData::StaticStruct())
		{
			DataTable->ForeachRow<FAttributeMetaData>(TEXT("Validate"), [&](const FName& RowName, const FAttributeMetaData& Row)
			{
				
				if (RowName == TEXT("BattleHealthSet.MaxHealth") && (Row.BaseValue <= 0.0f || Row.MaxValue <= 0.0f))
				{
					AssetFails(InAsset, FText::FromString("BattleHealthSet.MaxHealth Value must not be zero"),ValidationErrors);
					bHasError = true;
				}
				else if (RowName == TEXT("BattleHealthSet.MaxStamina")&& (Row.BaseValue <= 0.0f || Row.MaxValue <= 0.0f))
				{
					AssetFails(InAsset, FText::FromString("BattleHealthSet.MaxStamina Value must not be zero"),ValidationErrors);
					bHasError = true;
				}
				else if (RowName == TEXT("BattleEnemySet.MaxGroggyValue")&& (Row.BaseValue <= 0.0f || Row.MaxValue <= 0.0f))
				{
					AssetFails(InAsset, FText::FromString("BattleEnemySet.MaxGroggyValue Value must not be zero"),ValidationErrors);
					bHasError = true;
				}
				else if (RowName == TEXT("BattleEnemySet.MaxPoiseValue")&& (Row.BaseValue <= 0.0f || Row.MaxValue <= 0.0f))
				{
					AssetFails(InAsset, FText::FromString("BattleEnemySet.MaxPoiseValue Value must not be zero"),ValidationErrors);
					bHasError = true;
				}
			});
		}
		if (bHasError)
		{
			return EDataValidationResult::Invalid; 
		}
	}
	
	AssetPasses(InAsset);
	return EDataValidationResult::Valid;
}
