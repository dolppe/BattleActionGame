#include "BattlePlayerController.h"

#include "BattlePlayerState.h"
#include "BattleActionGame/BattleLogChannels.h"
#include "BattleActionGame/Camera/BattlePlayerCameraManager.h"
#include "BattleActionGame/AbilitySystem/BattleAbilitySystemComponent.h"
#include UE_INLINE_GENERATED_CPP_BY_NAME(BattlePlayerController)

ABattlePlayerController::ABattlePlayerController(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	PlayerCameraManagerClass = ABattlePlayerCameraManager::StaticClass();
}

void ABattlePlayerController::PostProcessInput(const float DeltaTime, const bool bGamePaused)
{
	if (UBattleAbilitySystemComponent* BattleASC = GetBattleABilitySystemComponent())
	{
		BattleASC->ProcessAbilityInput(DeltaTime, bGamePaused);
	}
	
	Super::PostProcessInput(DeltaTime, bGamePaused);
	
}

ABattlePlayerState* ABattlePlayerController::GetBattlePlayerState() const
{
	return CastChecked<ABattlePlayerState>(PlayerState, ECastCheckedType::NullAllowed);
}

UBattleAbilitySystemComponent* ABattlePlayerController::GetBattleABilitySystemComponent() const
{
	const ABattlePlayerState* BattlePS = GetBattlePlayerState();
	return (BattlePS ? BattlePS->GetBattleAbilitySystemComponent() : nullptr);
}

// void ABattlePlayerController::TickActor(float DeltaTime, ELevelTick TickType, FActorTickFunction& ThisTickFunction)
// {
// 	Super::TickActor(DeltaTime, TickType, ThisTickFunction);
// 	if (GetBattlePlayerState() && HasAuthority())
// 	{
// 		FCombatStat& CombatStat = GetBattlePlayerState()->CombatStat;
// 		BA_LOG(LogBattle, Log, TEXT("%f, %f, %d, %d, %d"), CombatStat.BossDamage, CombatStat.ReceivedDamage, CombatStat.ItemCount, CombatStat.JustDashCount, CombatStat.JustClashCount);	
// 	}
// 	
// }

