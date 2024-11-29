#pragma once

#include "Engine/DataAsset.h"
#include "BattleItemData.generated.h"


class UNiagaraSystem;

UENUM(BlueprintType)
enum class EItemType : uint8
{
	AttributeBased,
};



UCLASS()
class UBattleItemData : public UDataAsset
{
	GENERATED_BODY()
public:

	UBattleItemData();

	UPROPERTY(EditAnywhere, Category="Battle|Item")
	EItemType ItemType;
	
	UPROPERTY(EditAnywhere, Category="Battle|Item")
	USoundBase* ItemSound;

	UPROPERTY(EditAnywhere, Category="Battle|Item")
	UNiagaraSystem* ItemEffect;

	UPROPERTY(EditAnywhere, Category="Battle|Item")
	TObjectPtr<UAnimMontage> UseAnimation;
	
	
};

