#pragma once

#include "EditorSubsystem.h"
#include "BattleAssetValidationSubsystem.generated.h"

UCLASS()
class UBattleAssetValidationSubsystem : public UEditorSubsystem
{
	GENERATED_BODY()
public:
	
	void ValidateAssets();
	
};

