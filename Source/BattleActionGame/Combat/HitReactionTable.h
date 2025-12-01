#pragma once

#include "HitReactionTable.generated.h"

class UNiagaraSystem;

UENUM()
enum class EStrikeType : uint8
{
	None,
	SwordA,
	SwordB,
	SwordC,
	SwordStrongA,
	SwordStrongB,
	SwordStrongC,
	
};

// UENUM()
// enum class ESurfaceType : uint8
// {
// 	None,
// 	Default,
// 	Armor,
// 	Flesh,
// 	
// };

USTRUCT(BlueprintType)
struct FReactionKey
{
	GENERATED_BODY()


	UPROPERTY(EditAnywhere)
	EStrikeType StrikeType;

	UPROPERTY(EditAnywhere)
	TEnumAsByte<EPhysicalSurface> SurfaceType;

	bool operator ==(const FReactionKey& Other) const
	{
		return StrikeType == Other.StrikeType && SurfaceType == Other.SurfaceType;
	}
};

FORCEINLINE uint32 GetTypeHash(const FReactionKey& Key)
{
	return HashCombine(::GetTypeHash(Key.StrikeType), GetTypeHash(Key.SurfaceType));
}

USTRUCT(BlueprintType)
struct FHitReactionData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<USoundBase*> HitSound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<UNiagaraSystem*> HitEffect;
	
};

UCLASS()
class UHitReactionTable : public UDataAsset
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintCallable)
	bool GetReactionData(EStrikeType StrikeType, EPhysicalSurface SurfaceType, FHitReactionData& OutHitReactionData);

	UPROPERTY(EditAnywhere)
	TMap<FReactionKey, FHitReactionData> ReactionMap;
	
	
};



