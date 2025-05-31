#include "BattleWorldInfoSubsystem.h"
#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(BattleWorldInfoSubsystem)

void UBattleWorldInfoSubsystem::PostInitialize()
{
	if (GetWorld()->GetNetMode() == NM_Client)
	{
		return;
	}

	TArray<AActor*> FoundSpots;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ABattleUtilitySpot::StaticClass(), FoundSpots);

	for (AActor* FoundSpot : FoundSpots)
	{
		if (ABattleUtilitySpot* UtilitySpot = Cast<ABattleUtilitySpot>(FoundSpot))
		{
			UtilitySpotMap.Add(UtilitySpot->GetSpotType(), UtilitySpot);
		}
	}
	
}
