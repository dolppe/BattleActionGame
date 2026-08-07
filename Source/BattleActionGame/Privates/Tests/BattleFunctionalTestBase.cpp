#include "BattleFunctionalTestBase.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(BattleFunctionalTestBase)


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
					OnPlayerDataReady();
				}
			}
			
		}
		
	}
}

void ABattleFunctionalTestBase::ClientInputPress(const FKey& Key)
{
	if (PlayerController)
	{
		PlayerController->ClientRPCInputPress(Key);
	}
	
	
	
}
