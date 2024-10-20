#include "BattleBotCreationComponent.h"

#include "BattleExperienceManagerComponent.h"
#include "BattleActionGame/Enemy/BattleEnemyCharacter.h"
#include "BattleActionGame/Enemy/BattleEnemyController.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(BattleBotCreationComponent)

UBattleBotCreationComponent::UBattleBotCreationComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void UBattleBotCreationComponent::BeginPlay()
{
	Super::BeginPlay();

	AGameStateBase* GameState = GetGameStateChecked<AGameStateBase>();
	UBattleExperienceManagerComponent* ExperienceComponent = GameState->FindComponentByClass<UBattleExperienceManagerComponent>();
	check(ExperienceComponent);
	ExperienceComponent->CallOrRegister_OnExperienceLoaded(FOnBattleExperienceLoaded::FDelegate::CreateUObject(this, &ThisClass::OnExperienceLoaded));
}

void UBattleBotCreationComponent::SpawnOneBot(TSubclassOf<ABattleEnemyCharacter> EnemyClass, TSubclassOf<ABattleEnemyController> EnemyControllerClass)
{
	if (EnemyClass)
	{
		if (UWorld* World = GetWorld())
		{
			FActorSpawnParameters EnemySpawnParams;
			FVector SpawnLocation(100.f, 100.f, 50.f);
			EnemySpawnParams.Name = FName(*EnemyClass->GetName());
			EnemySpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
			ABattleEnemyCharacter* SpawnedEnemy = World->SpawnActor<ABattleEnemyCharacter>(EnemyClass, SpawnLocation, FRotator::ZeroRotator, EnemySpawnParams);

			FActorSpawnParameters EnemyControllerSpawnParams;
			EnemyControllerSpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
			EnemyControllerSpawnParams.OverrideLevel = GetComponentLevel();
			EnemyControllerSpawnParams.ObjectFlags |= RF_Transient;

			ABattleEnemyController* NewController = World->SpawnActor<ABattleEnemyController>(EnemyControllerClass, FVector::ZeroVector, FRotator::ZeroRotator, EnemyControllerSpawnParams);
			
			if (SpawnedEnemy && NewController)
			{
				NewController->Possess(SpawnedEnemy);
			}
		}

		
	}
	
}

void UBattleBotCreationComponent::OnExperienceLoaded(const UBattleExperienceDefinition* Experience)
{
	SpawnOneBot(DefaultEnemyClass, DefaultEnemyControllerClass);
}
