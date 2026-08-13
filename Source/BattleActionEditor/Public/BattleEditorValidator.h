#pragma once
#include "EditorValidatorBase.h"

#include "BattleEditorValidator.generated.h"

UCLASS()
class UBattleEditorValidator : public UEditorValidatorBase
{
	GENERATED_BODY()
public:
	
	virtual bool CanValidateAsset_Implementation(UObject* InAsset) const override;
	virtual EDataValidationResult ValidateLoadedAsset_Implementation(UObject* InAsset, TArray<FText>& ValidationErrors) override;
	
	
	
};
