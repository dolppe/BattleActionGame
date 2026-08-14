#include "BattleAssetValidationSubsystem.h"

#include "AssetRegistry/AssetRegistryModule.h"
#include "BattleActionGame/BattleLogChannels.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(BattleAssetValidationSubsystem)

bool UBattleAssetValidationSubsystem::ValidateAssets(FValidateAssetsResults& OutResults)
{
	FAssetRegistryModule& AssetRegistryModule = FModuleManager::LoadModuleChecked<FAssetRegistryModule>(TEXT("AssetRegistry"));
	
	IAssetRegistry& AssetRegistry = AssetRegistryModule.Get();
	
	TArray<FName> ValidationPaths =
	{
		TEXT("/Game/Characters"),
		TEXT("/Game/DataTable"),
		TEXT("/Game/Environmental"),
		TEXT("/Game/GameplayCueNotifies"),
		TEXT("/Game/GameplayEffects"),
		TEXT("/Game/Input"),
		TEXT("/Game/Enemy"),
		TEXT("/Game/Maps"),
		TEXT("/Game/Physics"),
		TEXT("/Game/System"),
		TEXT("/Game/UI"),

		// Plugin
		TEXT("/ActionCore")
	};
	
	TArray<FAssetData> AssetDataList;
	
	for (const FName& Path : ValidationPaths)
	{
		AssetRegistry.GetAssetsByPath(Path,AssetDataList,true);
	}
	
	UEditorValidatorSubsystem* ValidatorSubsystem =	GEditor->GetEditorSubsystem<UEditorValidatorSubsystem>();
	
	if (!ValidatorSubsystem)
	{
		UE_LOG(LogBattle, Warning, TEXT("Failed To Get EditorValidatorSubsystem"));
		return false;
	}
	FValidateAssetsSettings Settings;
	
	Settings.bShowIfNoFailures = true;
	Settings.bLoadAssetsForValidation = true;
	Settings.ValidationUsecase = EDataValidationUsecase::Manual;
	Settings.bSkipExcludedDirectories = true;
	
	const int32 NumIssues =	ValidatorSubsystem->ValidateAssetsWithSettings(AssetDataList, Settings,OutResults);
	
	UE_LOG(LogBattle, Log,TEXT("ValidateFinished - Checked: %d, Validated: %d, InValidated: %d, Skipped: %d, Ext: %d"), OutResults.NumChecked, OutResults.NumValid, OutResults.NumInvalid, OutResults.NumSkipped, OutResults.NumUnableToValidate + OutResults.NumWarnings);
	
	return true;
}
