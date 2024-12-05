#pragma once

#include "Engine/DataAsset.h"
#include "BattleItemData.generated.h"


class UNiagaraSystem;

UENUM(BlueprintType)
enum class EItemType : uint8
{
	AttributeBased,
};



UCLASS(BlueprintType)
class UBattleItemData : public UDataAsset
{
	GENERATED_BODY()
public:

	UBattleItemData();

	UPROPERTY(EditAnywhere, BlueprintReadWrite,Category="Battle|Item")
	EItemType ItemType;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite,Category="Battle|Item")
	USoundBase* ItemSound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite,Category="Battle|Item")
	UNiagaraSystem* ItemEffect;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite,Category="Battle|Item")
	TObjectPtr<UAnimMontage> UseAnimation;
	
	
};

