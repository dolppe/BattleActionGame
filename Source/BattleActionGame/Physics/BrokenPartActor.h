#pragma once


#include "BrokenPartActor.generated.h"

USTRUCT()
struct FBoneInfo
{
	GENERATED_BODY()

	UPROPERTY()
	FTransform BoneTransform;

	UPROPERTY()
	int BoneIndex;
	
};

UCLASS()
class ABrokenPartActor : public AActor
{
	GENERATED_BODY()
public:

	ABrokenPartActor(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	virtual void TickActor(float DeltaTime, ELevelTick TickType, FActorTickFunction& ThisTickFunction) override;
	
	virtual void BeginPlay() override;

	USkeletalMeshComponent* SkeletalMeshComponent;
	
	UFUNCTION()
	void OnRep_BoneInfos();

	UPROPERTY(ReplicatedUsing=OnRep_BoneInfos)
	TArray<FBoneInfo> BoneInfos;
	
};
