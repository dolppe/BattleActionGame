#include "BrokenPartActor.h"

#include "BattleActionGame/BattleLogChannels.h"
#include "BattleActionGame/Animation/DestroyedAnimInstance.h"
#include "Net/UnrealNetwork.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(BrokenPartActor)

PRAGMA_DISABLE_OPTIMIZATION

ABrokenPartActor::ABrokenPartActor(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	bReplicates = true;
	
	SkeletalMeshComponent = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("SkeletalMeshComponent"));
	SkeletalMeshComponent->SetIsReplicated(true);

	SetRootComponent(SkeletalMeshComponent);

	PrimaryActorTick.bCanEverTick = true;
	
	SetReplicateMovement(true);

	if (GetWorld())
	{
		UE_LOG(LogBattle, Log, TEXT("%s"), GetWorld()->GetNetMode()==NM_Client?TEXT("Client"):TEXT("Server"));
	}
}

void ABrokenPartActor::TickActor(float DeltaTime, ELevelTick TickType, FActorTickFunction& ThisTickFunction)
{
	Super::TickActor(DeltaTime, TickType, ThisTickFunction);
	
	if (HasAuthority())
	{
		const TArray<FTransform>& ComponentSpaceTransform = SkeletalMeshComponent->GetComponentSpaceTransforms();
		for (FBoneInfo& BoneInfo: BoneInfos)
		{
			BoneInfo.BoneTransform = ComponentSpaceTransform[BoneInfo.BoneIndex];
		}
	}
}

void ABrokenPartActor::BeginPlay()
{
	Super::BeginPlay();
}

void ABrokenPartActor::OnRep_BoneInfos()
{

}


void ABrokenPartActor::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	DOREPLIFETIME(ThisClass, BoneInfos);
	
}

PRAGMA_ENABLE_OPTIMIZATION
