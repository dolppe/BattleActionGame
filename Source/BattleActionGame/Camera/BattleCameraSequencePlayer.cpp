#include "BattleCameraSequencePlayer.h"

#include "BattleActionGame/BattleLogChannels.h"
#include "BattleActionGame/Character/BattleHeroComponent.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(BattleCameraSequencePlayer)

UBattleCameraSequencePreset::UBattleCameraSequencePreset()
{
}

UBattleCameraSequencePlayer::UBattleCameraSequencePlayer()
{
	
}

void UBattleCameraSequencePlayer::Initialize(UBattleHeroComponent* InHeroComponent)
{
	HeroComponent = InHeroComponent;
	bIsPlaying = false;
}

bool UBattleCameraSequencePlayer::StartCameraSequence(const UBattleCameraSequencePreset* InPreset,
                                                      const FBattleCameraModeInitData& InitData)
{
	if (bIsPlaying)
	{
		return false;
	}
	if (InPreset->Items.IsEmpty())
	{
		return false;
	}
	
	bIsPlaying = true;
	CurSequencePreset = InPreset;
	CurPresetIndex = 0; 
	CurInitData = InitData;
	
	UpdateCameraMode();
	
	
	
	return true;
}


void UBattleCameraSequencePlayer::UpdateCameraMode()
{
	if (!CurSequencePreset->Items.IsValidIndex(CurPresetIndex))
	{
		EndCameraSequence();
		return;
	}
	
	const FBattleCameraSequenceItem& SequenceItem = CurSequencePreset->Items[CurPresetIndex];
	//UE_LOG(LogBattle,Log,TEXT("UpdateCameraMode CMCMCM: StartCS %s, %s "), *GetOuter()->GetName(), *HeroComponent->GetOwner()->GetName());
	HeroComponent->SetSpecialCameraMode(SequenceItem.CameraMode, CurInitData);	
	HeroComponent->GetWorld()->GetTimerManager().SetTimer(TimerHandle, this, &UBattleCameraSequencePlayer::UpdateCameraMode, SequenceItem.Duration, false);
	
	CurPresetIndex++;
	
}

void UBattleCameraSequencePlayer::EndCameraSequence()
{
	EndCameraSequenceDelegate.ExecuteIfBound();
	bIsPlaying = false;
}
