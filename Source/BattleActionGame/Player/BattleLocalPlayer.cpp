#include "BattleLocalPlayer.h"
#include UE_INLINE_GENERATED_CPP_BY_NAME(BattleLocalPlayer)

UBattleLocalPlayer::UBattleLocalPlayer()
	: Super()
{
}

APlayerController* UBattleLocalPlayer::GetPlayerControllerWithBP(const UWorld* const InWorld) const
{
	return GetPlayerController(InWorld);
}
