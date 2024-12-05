#pragma once

#include "BattleItemData.h"
#include "GameplayTags.h"
#include "BattleItemData_AttributeBased.generated.h"

USTRUCT(BlueprintType)
struct FAttributeMagData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category="Battle|Item")
	FGameplayTag AttributeTag;

	UPROPERTY(EditAnywhere, Category="Battle|Item")
	float Magnitude;
	
};

USTRUCT(BlueprintType)
struct FAttributeData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category="Battle|Item")
	float Duration;

	UPROPERTY(EditAnywhere, Category="Battle|Item")
	TArray<FAttributeMagData> AttributeMag;
	
};

UCLASS()
class UBattleItemData_AttributeBased : public UBattleItemData
{
	GENERATED_BODY()
public:

	UBattleItemData_AttributeBased();

	UPROPERTY(EditAnywhere, Category="Battle|Item")
	TArray<FAttributeData> AttributeData;


};
