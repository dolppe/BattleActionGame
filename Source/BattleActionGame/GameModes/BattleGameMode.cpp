#include "BattleGameMode.h"

#include "BattleExperienceManagerComponent.h"
#include "BattleGameState.h"
#include "BattleUserFacingExperienceDefinition.h"
#include "BattleWorldSettings.h"
#include "CommonSessionSubsystem.h"
#include "BattleActionGame/Character/BattleCharacter.h"
#include "BattleActionGame/Character/BattlePawnData.h"
#include "BattleActionGame/Character/BattlePawnExtensionComponent.h"
#include "BattleActionGame/Player/BattlePlayerController.h"
#include "BattleActionGame/Player/BattlePlayerState.h"
#include "BattleActionGame/System/BattleAssetManager.h"
#include "BattleActionGame/UI/BattleHUD.h"
#include "Kismet/GameplayStatics.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(BattleGameMode)

ABattleGameMode::ABattleGameMode(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	GameStateClass = ABattleGameState::StaticClass();
	PlayerControllerClass = ABattlePlayerController::StaticClass();
	PlayerStateClass = ABattlePlayerState::StaticClass();
	DefaultPawnClass = ABattleCharacter::StaticClass();
	HUDClass = ABattleHUD::StaticClass();
}

void ABattleGameMode::InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage)
{
	Super::InitGame(MapName, Options, ErrorMessage);

	// 아직 GameInstance를 통해 초기화 작업이 진행중인 상태
	// 현 프레임에는 Experience 처리가 불가능함 => 한 프레임 뒤에 처리하도록 걸어두는 것.

	if (!UGameplayStatics::HasOption(OptionsString, TEXT("bTraveled")))
	{
		GetWorld()->GetTimerManager().SetTimerForNextTick(this, &ThisClass::TryMapLoad);
	}
	else
	{
		GetWorld()->GetTimerManager().SetTimerForNextTick(this, &ThisClass::HandleMatchAssignmentIfNotExpectingOne);
	}
	

}


void ABattleGameMode::InitGameState()
{
	Super::InitGameState();

	// Experience 비동기 로딩을 위한 델리게이트
	UBattleExperienceManagerComponent* ExperienceManagerComponent = GameState->FindComponentByClass<UBattleExperienceManagerComponent>();
	check(ExperienceManagerComponent);

	ExperienceManagerComponent->CallOrRegister_OnExperienceLoaded(FOnBattleExperienceLoaded::FDelegate::CreateUObject(this, &ThisClass::OnExperienceLoaded));
}

void ABattleGameMode::HandleStartingNewPlayer_Implementation(APlayerController* NewPlayer)
{
	// Experience가 로딩되지 않았을 때 NewPlayer를 생성하고 Possess하는 것을 막음.
	if (IsExperienceLoaded())
	{
		Super::HandleStartingNewPlayer_Implementation(NewPlayer);
	}
}

APawn* ABattleGameMode::SpawnDefaultPawnAtTransform_Implementation(AController* NewPlayer,
	const FTransform& SpawnTransform)
{
	FActorSpawnParameters SpawnInfo;
	SpawnInfo.Instigator = GetInstigator();
	SpawnInfo.ObjectFlags |= RF_Transient;
	SpawnInfo.bDeferConstruction = true;

	// PawnData를 통해 PawnClass를 가져와서 Spawn 진행함.
	if (UClass* PawnClass = GetDefaultPawnClassForController(NewPlayer))
	{
		if (APawn* SpawnedPawn = GetWorld()->SpawnActor<APawn>(PawnClass, SpawnTransform,SpawnInfo))
		{
			if (UBattlePawnExtensionComponent* PawnExtComp = UBattlePawnExtensionComponent::FindPawnExtensionComponent(SpawnedPawn))
			{
				if (const UBattlePawnData* PawnData = GetPawnDataForController(NewPlayer))
				{
					PawnExtComp->SetPawnData(PawnData);
				}
			}
			SpawnedPawn->FinishSpawning(SpawnTransform);
			return SpawnedPawn;
		}
	}
	
	return nullptr;	

}

UClass* ABattleGameMode::GetDefaultPawnClassForController_Implementation(AController* InController)
{
	if (const UBattlePawnData* PawnData = GetPawnDataForController(InController))
	{
		if (PawnData->PawnClass)
		{
			return PawnData->PawnClass;
		}
	}

	return Super::GetDefaultPawnClassForController_Implementation(InController);
}

bool ABattleGameMode::PlayerCanRestart_Implementation(APlayerController* Player)
{
	return ControllerCanRestart(Player);
}


void ABattleGameMode::RequestPlayerRestartNextFrame(AController* Controller, bool bForceReset)
{
	if (bForceReset && (Controller != nullptr))
	{
		Controller->Reset();
	}

	if (APlayerController* PC = Cast<APlayerController>(Controller))
	{
		GetWorldTimerManager().SetTimerForNextTick(PC, &APlayerController::ServerRestartPlayer_Implementation);
	}
	
}


void ABattleGameMode::HandleMatchAssignmentIfNotExpectingOne()
{
	// Experience에 대해 PrimaryAssetId를 생성하여, OnMatchAssignmentGiven으로 넘겨줌.
	FPrimaryAssetId ExperienceId;

	UWorld* World = GetWorld();

	// ServerTravel때 넘긴 ExtraArgs는 OptionString에 들어있음.
	if (!ExperienceId.IsValid() && UGameplayStatics::HasOption(OptionsString, TEXT("Experience")))
	{
		const FString ExperienceFromOptions = UGameplayStatics::ParseOption(OptionsString, TEXT("Experience"));
		ExperienceId = FPrimaryAssetId(UBattleExperienceDefinition::StaticClass()->GetFName(), FName(*ExperienceFromOptions));
	}

	if (!ExperienceId.IsValid())
	{
		if (ABattleWorldSettings* TypedWorldSettings = Cast<ABattleWorldSettings>(GetWorldSettings()))
		{
			ExperienceId = TypedWorldSettings->GetDefaultGameplayExperience();
		}
	}

	// Experience가 없으면 Default로 설정된 것으로 가져옴.
	// 만들어둔 것을 Default하게 설정해서 PrimaryAssetId로 생성함
	// 5.1 Ver 기준으로는 Config나 다른 곳에서 가져오는 것이 아니라 그냥 바로 코드상 입력되어 있음.
	if (!ExperienceId.IsValid())
	{
		ExperienceId = FPrimaryAssetId(FPrimaryAssetType("BattleExperienceDefinition"), FName("BP_BattleDefaultExperience"));
	}

	OnMatchAssignmentGiven(ExperienceId);
	
}


void ABattleGameMode::OnMatchAssignmentGiven(FPrimaryAssetId ExperienceId)
{
#if WITH_SERVER_CODE
	// ExperienceManagerComponent를 활용하여 Experience를 로딩하기 위함.
	check(ExperienceId.IsValid());

	UBattleExperienceManagerComponent* ExperienceManagerComponent = GameState->FindComponentByClass<UBattleExperienceManagerComponent>();
	check(ExperienceManagerComponent);

	ExperienceManagerComponent->ServerSetCurrentExperience(ExperienceId);
#endif
}



bool ABattleGameMode::IsExperienceLoaded() const
{
	check(GameState);
	UBattleExperienceManagerComponent* ExperienceManagerComponent = GameState->FindComponentByClass<UBattleExperienceManagerComponent>();
	check(ExperienceManagerComponent);

	return ExperienceManagerComponent->IsExperienceLoaded();
}

const UBattlePawnData* ABattleGameMode::GetPawnDataForController(const AController* InController) const
{
	if (InController)
	{
		if (const ABattlePlayerState* BattlePS = InController->GetPlayerState<ABattlePlayerState>())
		{
			if (const UBattlePawnData* PawnData = BattlePS->GetPawnData<UBattlePawnData>())
			{
				return PawnData;
			}
		}
	}

	check(GameState);
	UBattleExperienceManagerComponent* ExperienceManagerComponent = GameState->FindComponentByClass<UBattleExperienceManagerComponent>();
	check(ExperienceManagerComponent);

	// Experience가 로딩됐으면 DefaultPawnData를 가져와서 리턴
	if (ExperienceManagerComponent->IsExperienceLoaded())
	{
		const UBattleExperienceDefinition* Experience = ExperienceManagerComponent->GetCurrentExperienceChecked();

		if (Experience->DefaultPawnData)
		{
			return Experience->DefaultPawnData;
		}
	}
	return nullptr;
}

void ABattleGameMode::OnExperienceLoaded(const UBattleExperienceDefinition* CurrentExperience)
{
	// PC를 순회
	for (FConstPlayerControllerIterator Iterator = GetWorld()->GetPlayerControllerIterator();Iterator;++Iterator)
	{
		APlayerController* PC = Cast<APlayerController>(*Iterator);

		// PC가 아직 Pawn에 Possess하지 않았으면, RestartPlayer를 통해 Pawn을 다시 Spawn함.
		if (PC && PC->GetPawn() == nullptr)
		{
			if (PlayerCanRestart(PC))
			{
				RestartPlayer(PC);
			}
		}
	}
	
}


bool ABattleGameMode::ControllerCanRestart(AController* Controller)
{
	if (APlayerController* PC = Cast<APlayerController>(Controller))
	{
		if (!Super::PlayerCanRestart_Implementation(PC))
		{
			return false;
		}
	}
	else
	{
		if ((Controller == nullptr) || Controller->IsPendingKillPending())
		{
			return false;
		}
	}

	return true;
	
}

void ABattleGameMode::TryMapLoad()
{
	UBattleAssetManager& AssetManager = UBattleAssetManager::Get();
	
	FSoftObjectPath Path(TEXT("/Script/Engine.World'/Game/Maps/L_TestMap.L_TestMap'"));
	
	UObject* Temp = AssetManager.SynchronousLoadAsset(Path);

	TArray<FPrimaryAssetId> UserFacingList;
	FPrimaryAssetType UserFacingType(TEXT("BattleUserFacingExperienceDefinition"));

	AssetManager.GetPrimaryAssetIdList(UserFacingType,UserFacingList);

	for (FPrimaryAssetId UserItem : UserFacingList)
	{
		UE_LOG(LogTemp,Log,TEXT("%s"), *UserItem.ToString());
	}
	
	UObject* TempUser = AssetManager.SynchronousLoadAsset(AssetManager.GetPrimaryAssetPath(UserFacingList[0]));
	
	UE_LOG(LogTemp,Log,TEXT("%s"), *TempUser->GetName());
	
	//UObject* UserFacing = AssetManager.GetPrimaryAssetObject(UserFacingList[0]);
	UBattleUserFacingExperienceDefinition* UserFacing = Cast<UBattleUserFacingExperienceDefinition>(TempUser);
	GetWorld()->ServerTravel(UserFacing->CreateHostingRequest()->ConstructTravelURL());

	
	
}

void ABattleGameMode::GoToMap()
{
	 
	
}

