#include "BattleBootstrapGameMode.h"

#include "BattleUserFacingExperienceDefinition.h"
#include "CommonSessionSubsystem.h"
#include "BattleActionGame/System/BattleAssetManager.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(BattleBootstrapGameMode)

ABattleBootstrapGameMode::ABattleBootstrapGameMode(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void ABattleBootstrapGameMode::InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage)
{
	Super::InitGame(MapName, Options, ErrorMessage);
	
	GetWorld()->GetTimerManager().SetTimerForNextTick(this, &ThisClass::TryMapLoad);
}

void ABattleBootstrapGameMode::TryMapLoad()
{
	UBattleAssetManager& AssetManager = UBattleAssetManager::Get();
	
	FSoftObjectPath Path(TEXT("/Script/Engine.World'/Game/Maps/L_TestMap.L_TestMap'"));
	
	UObject* Temp = AssetManager.SynchronousLoadAsset(Path);

	TArray<FPrimaryAssetId> UserFacingList;
	FPrimaryAssetType UserFacingType(TEXT("BattleUserFacingExperienceDefinition"));

	AssetManager.GetPrimaryAssetIdList(UserFacingType,UserFacingList);

	for (FPrimaryAssetId UserItem : UserFacingList)
	{
		//UE_LOG(LogTemp,Log,TEXT("%s"), *UserItem.ToString());
	}
	
	UObject* TempUser = AssetManager.SynchronousLoadAsset(AssetManager.GetPrimaryAssetPath(UserFacingList[0]));
	
	//UE_LOG(LogTemp,Log,TEXT("%s"), *TempUser->GetName());
	
	//UObject* UserFacing = AssetManager.GetPrimaryAssetObject(UserFacingList[0]);
	UBattleUserFacingExperienceDefinition* UserFacing = Cast<UBattleUserFacingExperienceDefinition>(TempUser);
	GetWorld()->ServerTravel(UserFacing->CreateHostingRequest()->ConstructTravelURL());
}
