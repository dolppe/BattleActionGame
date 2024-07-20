#include "BattlePawnData.h"
#include "BattleActionGame/Camera/BattleCameraMode.h"
#include UE_INLINE_GENERATED_CPP_BY_NAME(BattlePawnData)

UBattlePawnData::UBattlePawnData(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	PawnClass = nullptr;
	DefaultCameraMode = nullptr;
	InputConfig = nullptr;
}
