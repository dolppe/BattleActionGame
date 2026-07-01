#pragma once

#include "BattleActionGame/Physics/BattlePartsManagerComponent.h"
#include "PartsSystemTests.generated.h"

UCLASS()
class UMockTestPartsManagerComponent : public UBattlePartsManagerComponent
{
	GENERATED_BODY()
public:
	
	void SetPartsData(const TMap<FGameplayTag, FPartData>& InPartsData)
	{
		PartsData = InPartsData;
	}
	
	TMap<FGameplayTag, FPartData>& GetPartsData()
	{
		return PartsData;
	}
	
	void SetDestroyedAnimInstance(TSubclassOf<UDestroyedAnimInstance> InDestroyedAnimInstance)
	{
		DestroyedAnimInstance = InDestroyedAnimInstance;
	}
	
	TArray<ABrokenPartActor*>& GetDestroyedParts()
	{
		return DestroyedParts;
	}
	
	TArray<FGameplayTag>& GetAppliedPartTags()
	{
		return AppliedPartTags;
	}
	
	TArray<FGameplayTag>& GetDestroyedPartTags()
	{
		return DestroyedPartTags;
	}
	
};
