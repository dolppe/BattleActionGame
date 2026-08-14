#include "BattleAssetValidationSubsystem.h"

#include "EditorValidatorSubsystem.h"
#include "AssetRegistry/AssetRegistryModule.h"
#include "BattleActionGame/BattleLogChannels.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(BattleAssetValidationSubsystem)

void UBattleAssetValidationSubsystem::ValidateAssets()
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
		return;
	}
	FValidateAssetsSettings Settings;
	
	Settings.bShowIfNoFailures = true;
	Settings.bLoadAssetsForValidation = true;
	Settings.ValidationUsecase = EDataValidationUsecase::Manual;
	Settings.bSkipExcludedDirectories = true;
	
	FValidateAssetsResults Results;
	
	const int32 NumIssues =	ValidatorSubsystem->ValidateAssetsWithSettings(AssetDataList, Settings,Results);
	
	UE_LOG(LogBattle, Log,TEXT("ValidateFinished - Checked: %d, Validated: %d, InValidated: %d, Skipped: %d, Ext: %d"), Results.NumChecked, Results.NumValid, Results.NumInvalid, Results.NumSkipped, Results.NumUnableToValidate + Results.NumWarnings);
}
