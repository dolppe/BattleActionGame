#include "BattlePawnData.h"
#include "BattleActionGame/Camera/BattleCameraMode.h"
#include "Misc/DataValidation.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(BattlePawnData)

UBattlePawnData::UBattlePawnData(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	PawnClass = nullptr;
	DefaultCameraMode = nullptr;
	InputConfig = nullptr;
}

EDataValidationResult UBattlePawnData::IsDataValid(class FDataValidationContext& Context)
{
	EDataValidationResult Result = Super::IsDataValid(Context);

	if (!PawnClass)
	{
		Context.AddError(FText::FromString(TEXT("UBattlePawnData must have PawnClass")));
		Result = EDataValidationResult::Invalid;
	}

	return Result;
}
