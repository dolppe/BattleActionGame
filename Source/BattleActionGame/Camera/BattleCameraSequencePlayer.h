#pragma once

#include "BattleCameraMode.h"
#include "BattleCameraSequencePlayer.generated.h"

class UBattleHeroComponent;

USTRUCT()
struct FBattleCameraSequenceItem
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere)
	TSubclassOf<UBattleCameraMode> CameraMode;
	
	UPROPERTY(EditAnywhere)
	float Duration;	
};

UCLASS()
class UBattleCameraSequencePreset : public UDataAsset 
{
	GENERATED_BODY()

public:
	
	UBattleCameraSequencePreset();
	
	UPROPERTY(EditAnywhere)
	TArray<FBattleCameraSequenceItem> Items;
	
};

UCLASS()
class UBattleCameraSequencePlayer : public UObject
{
	GENERATED_BODY()
public:
	
	UBattleCameraSequencePlayer();
	
	void Initialize(UBattleHeroComponent* InHeroComponent);
	
	bool StartCameraSequence(const UBattleCameraSequencePreset* InPreset, const FBattleCameraModeInitData& InitData);
	
	void UpdateCameraMode();
	
	void EndCameraSequence();
	
	FSimpleDelegate EndCameraSequenceDelegate;
	
private:
	
	bool bIsPlaying = false;
	
	FTimerHandle TimerHandle;
	
	const UBattleCameraSequencePreset* CurSequencePreset;
	int CurPresetIndex;
	FBattleCameraModeInitData CurInitData;
	
	UBattleHeroComponent* HeroComponent;
};




