#pragma once

#include "Animation/AnimInstance.h"
#include "DestroyedAnimInstance.generated.h"

UCLASS()
class UDestroyedAnimInstance : public UAnimInstance
{
	GENERATED_BODY()
public:

	UDestroyedAnimInstance(const FObjectInitializer& ObjectInitializer);

	void SetPartsName(FName PartBoneName)
	{
		PartName = PartBoneName;
	}

protected:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Animation")
	FName PartName;
	
};
