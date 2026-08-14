#include "BattleFunctionalTestBase.h"

#include "BattleActionGame/BattleGameplayTags.h"
#include "BattleActionGame/Character/BattlePawnExtensionComponent.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(BattleFunctionalTestBase)


PRAGMA_DISABLE_OPTIMIZATION

ABattleFunctionalTestBase::ABattleFunctionalTestBase(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	
}

void ABattleFunctionalTestBase::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);


	if (!bPlayerDataReady)
	{
		if (UWorld* World = GetWorld())
		{
			if (APlayerController* PC = World->GetFirstPlayerController())
			{
				if (ABattleFunctionalTestPlayerController* TestClientAgentComponent = Cast<ABattleFunctionalTestPlayerController>(PC))
				{
					PlayerController = TestClientAgentComponent;
					bPlayerDataReady = true;
					PlayerControllerReady();
					
				}
			}
			
		}
		
	}
}

void ABattleFunctionalTestBase::PlayerControllerReady()
{
	OnPlayerDataReady();
	
	if (APawn* Pawn = PlayerController->GetPawn())
	{
		if (UBattlePawnExtensionComponent* PawnExtensionComponent = Cast<UBattlePawnExtensionComponent>(Pawn->GetComponentByClass(UBattlePawnExtensionComponent::StaticClass())))
		{
			const FBattleGameplayTags& GameplayTags = FBattleGameplayTags::Get();
			
			if (PawnExtensionComponent->GetInitState() == GameplayTags.InitState_GameplayReady)
			{
				GameplayReady();
			}
			else
			{
				PawnExtensionComponent->OnGameplayReady.AddUObject(this, &ThisClass::GameplayReady);
			}
		}
	}
}

void ABattleFunctionalTestBase::GameplayReady()
{
	OnGameplayReady();
	
	
}

void ABattleFunctionalTestBase::ClientInputPress(const FKey& Key)
{
	if (PlayerController)
	{
		PlayerController->ClientRPCInputPress(Key);
	}

	
	
}

void ABattleFunctionalTestBase::ClientInputPressRepeat(const FKey& Key, float Duration, float Period)
{
	if (PlayerController)
	{
		PlayerController->ClientRPCInputPressRepeat(Key,Duration,Period);
	}
}

void ABattleFunctionalTestBase::ClientInputAxis(const FKey& Key, const FVector& Delta)
{
	if (PlayerController)
	{
		PlayerController->ClientRPCInputAxis(Key, Delta);
	}
}

void ABattleFunctionalTestBase::ClientInputInjection(const UInputAction* InputAction)
{
	if (PlayerController)
	{
		PlayerController->ClientRPCInjectionInputAction(InputAction);
	}
}


PRAGMA_ENABLE_OPTIMIZATION
