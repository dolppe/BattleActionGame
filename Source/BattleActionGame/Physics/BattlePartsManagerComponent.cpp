#include "BattlePartsManagerComponent.h"
#include "BattleActionGame/Enemy/BattleEnemyCharacter.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(BattlePartsManagerComponent)

void FPartEventData::TriggerEvent(FPartData* PartData, ABattleEnemyCharacter* MyCharacter, const TArray<FName>& BoneNames)
{
	if (EventType == EPartEventType::Damage)
	{
		PartData->SurfaceState = NextSurfaceType;
	}
	else if (EventType == EPartEventType::Destroy)
	{
		MyCharacter->DestroyParts(BoneNames);
		PartData->bIsDestroyed = true;
	}
	
}

void FPartData::HandleDamaged(ABattleEnemyCharacter* MyCharacter, int DamagedHp)
{
	PartHp = FMath::Clamp(PartHp - DamagedHp, 0, PartMaxHp);

	if (PartEvents.IsValidIndex(PartEventIdx))
	{
		if (PartEvents[PartEventIdx].TriggerHp >= PartHp)
		{
			PartEvents[PartEventIdx].TriggerEvent(this, MyCharacter, BoneNames);
			PartEventIdx++;
		}
	}
}

UBattlePartsManagerComponent::UBattlePartsManagerComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void UBattlePartsManagerComponent::HandleDamagedToPart(FName BoneName, FGameplayTag PartTag)
{
	if (!HasAuthority())
	{
		return;
	}
	if (FPartData* PartData = PartsData.Find(PartTag))
	{
		if (ABattleEnemyCharacter* EnemyCharacter = Cast<ABattleEnemyCharacter>(GetOwner()))
		{
			PartData->HandleDamaged(EnemyCharacter);
		}
	}
}

EPhysicalSurface UBattlePartsManagerComponent::GetSurfaceStateFromPart(FGameplayTag PartTag)
{
	if (FPartData* PartData = PartsData.Find(PartTag))
	{
		return PartData->SurfaceState;
	}

	return DefaultSurfaceType;
}

bool UBattlePartsManagerComponent::IsPartDestroyed(FGameplayTag PartTag)
{
	if (FPartData* PartData = PartsData.Find(PartTag))
	{
		return PartData->bIsDestroyed;
	}
	
	return true;
}
