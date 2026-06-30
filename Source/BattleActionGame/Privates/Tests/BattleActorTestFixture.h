#pragma once

#if WITH_DEV_AUTOMATION_TESTS

#include "BattleActionGame/GameModes/BattleGameMode.h"
#include "BattleActionGame/System/BattleGameInstance.h"
#include "Misc/AutomationTest.h"

class FBattleActorTestFixture : public FAutomationTestBase
{

public:
	FBattleActorTestFixture(const FString& InName, const bool bInComplexTask)
		: FAutomationTestBase(InName, bInComplexTask)
	{

	}
	
	~FBattleActorTestFixture()
	{
	}
	
	void SetInitializationValues(UWorld::InitializationValues* InIVS)
	{
		IVS = InIVS;
	}
	
	bool SetupWorld()
	{
		ClearWorld();
		
		if (IVS == nullptr)
		{
			World = UWorld::CreateWorld(EWorldType::Game, false, TEXT("BattleActorTestWorld"));
		}
		else
		{
			World = UWorld::CreateWorld(EWorldType::Game, false, TEXT("BattleActorTestWorld"), NULL, true, ERHIFeatureLevel::Num, IVS);
		}
		
		if (!World)
		{
			return false;
		}
		if (GEngine)
		{
			FWorldContext& WorldContext = GEngine->CreateNewWorldContext(EWorldType::Game);
			WorldContext.SetCurrentWorld(World);
		}
		
		
		return true;
	}
	
	void ClearWorld()
	{
		if (World)
		{
			if (GEngine)
			{
				GEngine->DestroyWorldContext(World);
			}

			World->DestroyWorld(false);
			World = nullptr;
		}
		
		CollectGarbage(GARBAGE_COLLECTION_KEEPFLAGS);
	}
	
	UWorld* GetWorld() const
	{
		return World;
	}
	
	void BeginPlay()
	{
		if (World)
		{
			World->BeginPlay();
		}
	}
	
	void InitializeActors()
	{
		if (World && !World->AreActorsInitialized())
		{
			FURL Url;
			World->InitializeActorsForPlay(Url, true);
		}
	}

private:
	
	UWorld::InitializationValues* IVS;
	UWorld* World;

	
};


class FBattleFullActorTestFixture : public FAutomationTestBase
{

public:
	FBattleFullActorTestFixture(const FString& InName, const bool bInComplexTask)
		: FAutomationTestBase(InName, bInComplexTask)
	{

	}
	
	~FBattleFullActorTestFixture()
	{
	}
	
	bool SetupWorld()
	{
		ClearWorld();
		
		World = UWorld::CreateWorld(EWorldType::Game, false, TEXT("BattleActorTestWorld"));
			
		if (!World)
		{
			return false;
		}
		if (GEngine)
		{
			FWorldContext& WorldContext = GEngine->CreateNewWorldContext(EWorldType::Game);
			WorldContext.SetCurrentWorld(World);
		}
		
		GameInstance = NewObject<UBattleGameInstance>(GEngine);
		GameInstance->InitializeStandalone();
		
		GetWorld()->GetWorldSettings()->DefaultGameMode = ABattleGameMode::StaticClass();
		
		FURL Url;
		GetWorld()->SetGameInstance(GameInstance);
		GetWorld()->SetGameMode(Url);
		
		return true;
	}
	
	void ClearWorld()
	{
		if (World)
		{
			if (GameInstance)
			{
				GameInstance->Shutdown();
				GameInstance = nullptr;
			}
			
			if (GEngine)
			{
				GEngine->DestroyWorldContext(World);
			}

			World->DestroyWorld(false);
			World = nullptr;
		}
		
		CollectGarbage(GARBAGE_COLLECTION_KEEPFLAGS);
	}
	
	UWorld* GetWorld() const
	{
		return World;
	}
	
	UGameInstance* GetGameInstance() const
	{
		return GameInstance;
	}
	
	AGameModeBase* GetGameMode() const
	{
		return World->GetAuthGameMode();
	}
	
	void BeginPlay()
	{
		if (World)
		{
			World->BeginPlay();
		}
	}
	
	void InitializeActors()
	{
		if (World && !World->AreActorsInitialized())
		{
			FURL Url;
			World->InitializeActorsForPlay(Url, true);
		}
	}

private:

	UGameInstance* GameInstance;
	UWorld* World;

	
};



#endif

