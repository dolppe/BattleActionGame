#pragma once

#include "BattleItemData.h"
#include "AttributeSet.h"
#include "BattleItemData_AttributeBased.generated.h"

USTRUCT(BlueprintType)
struct FAttributeData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category="Battle|Item")
	FGameplayAttribute Attribute;

	UPROPERTY(EditAnywhere, Category="Battle|Item")
	float Magnitude;
	
	UPROPERTY(EditAnywhere, Category="Battle|Item")
	float Duration;
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
