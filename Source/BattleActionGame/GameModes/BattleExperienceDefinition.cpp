#include "BattleExperienceDefinition.h"

#include "GameFeatureAction.h"
#include "Misc/DataValidation.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(BattleExperienceDefinition)

UBattleExperienceDefinition::UBattleExperienceDefinition(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}
#if WITH_EDITOR
EDataValidationResult UBattleExperienceDefinition::IsDataValid(class FDataValidationContext& Context)
{
	EDataValidationResult Result = Super::IsDataValid(Context);

	if (!DefaultPawnData)
	{
		Context.AddError(FText::FromString(TEXT("UBattleExperienceDefinition must have DefaultPawnData")));
		Result = EDataValidationResult::Invalid;
	}

	return Result;
}
#endif
#if WITH_EDITORONLY_DATA
void UBattleExperienceDefinition::UpdateAssetBundleData()
{
	Super::UpdateAssetBundleData();

	for (UGameFeatureAction* Action : Actions)
	{
		if (Action)
		{
			// UGameFeatureAction의 메서드이며, 임의적으로 호출해서 AssetBundleData에 추가해준다.
			Action->AddAdditionalAssetBundleData(AssetBundleData);
		}
	}
}
#endif