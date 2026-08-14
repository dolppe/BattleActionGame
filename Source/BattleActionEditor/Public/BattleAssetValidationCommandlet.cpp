#include "BattleAssetValidationCommandlet.h"

#include "BattleAssetValidationSubsystem.h"
#include "BattleActionGame/BattleLogChannels.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(BattleAssetValidationCommandlet)

int32 UBattleAssetValidationCommandlet::Main(const FString& Params)
{
	UE_LOG(
		LogBattle,
		Display,
		TEXT("Battle Asset Validation Started"));

	UBattleAssetValidationSubsystem* ValidationSubsystem = GEditor->GetEditorSubsystem<UBattleAssetValidationSubsystem>();

	if (!ValidationSubsystem)
	{
		UE_LOG(LogBattle,Error,TEXT("BattleAssetValidationSubsystem not found"));
		return 2;
	}

	FValidateAssetsResults Results;

	if (!ValidationSubsystem->ValidateAssets(Results))
	{
		UE_LOG(LogBattle,Error,TEXT("Asset Validation 실행에 실패했습니다."));

		return 2;
	}

	UE_LOG(
		LogBattle,Display,TEXT(
			"Asset Validation Finished - "
			"Checked: %d, Valid: %d, Invalid: %d, "
			"Skipped: %d, Warnings: %d"),
		Results.NumChecked,
		Results.NumValid,
		Results.NumInvalid,
		Results.NumSkipped,
		Results.NumWarnings);

	if (Results.NumInvalid > 0)
	{
		UE_LOG(
			LogBattle,
			Error,
			TEXT("%d개의 Invalid Asset이 발견되었습니다."),
			Results.NumInvalid);

		return 2;
	}

	return 0;
}
