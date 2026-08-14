#pragma once

#include "EditorSubsystem.h"
#include "EditorValidatorSubsystem.h"
#include "BattleAssetValidationSubsystem.generated.h"

UCLASS()
class UBattleAssetValidationSubsystem : public UEditorSubsystem
{
	GENERATED_BODY()
public:
	
	bool ValidateAssets(FValidateAssetsResults& OutResults);
	
};

