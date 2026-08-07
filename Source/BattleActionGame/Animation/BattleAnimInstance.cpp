#include "BattleAnimInstance.h"

#include "AbilitySystemGlobals.h"
#include "Animation/AnimInstanceProxy.h"
#include "BattleActionGame/BattleLogChannels.h"
#include "BattleActionGame/Character/BattleCharacterMovementComponent.h"
#include "GameFramework/Character.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(BattleAnimInstance)

UBattleAnimInstance::UBattleAnimInstance(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	RunThreshold = 10.0f;
	JumpingThreshold = 100.0f;
}

void UBattleAnimInstance::InitializeWithAbilitySystem(UAbilitySystemComponent* ASC)
{
	check(ASC);

	GameplayTagPropertyMap.Initialize(this, ASC);
}


void UBattleAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	if (AActor* OwningActor = GetOwningActor())
	{
		if (UAbilitySystemComponent* ASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(OwningActor))
		{
			InitializeWithAbilitySystem(ASC);
		}
		if (ACharacter* Character = Cast<ACharacter>(OwningActor))
		{
			Owner = Character;
			CharacterMovement = Cast<UBattleCharacterMovementComponent>(Owner->GetCharacterMovement());
		}
	}
}

void UBattleAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	if (CharacterMovement)
	{
		CharacterVelocity = CharacterMovement->Velocity;
		CharacterSpeed = CharacterVelocity.Size2D();
		bIsIdle = CharacterSpeed < RunThreshold;
		bIsFalling = CharacterMovement->IsFalling();
		bIsJumping = bIsFalling & (CharacterVelocity.Z > JumpingThreshold);
	}
	//
	// UWorld* World = GetWorld();
	// if (!World)
	// {
	// 	return;
	// }
	//
	// static TMap<UAnimInstance*, double> LastReportTimeMap;
	// static TMap<UAnimInstance*, double> AccumDeltaMap;
	// static TMap<UAnimInstance*, int32> UpdateCountMap;
	// static TMap<UAnimInstance*, uint64> LastFrameMap;
	// static TMap<UAnimInstance*, int32> SameFrameUpdateCountMap;
	//
	// double& LastReportTime = LastReportTimeMap.FindOrAdd(this);
	// double& AccumDelta = AccumDeltaMap.FindOrAdd(this);
	// int32& UpdateCount = UpdateCountMap.FindOrAdd(this);
	// uint64& LastFrame = LastFrameMap.FindOrAdd(this);
	// int32& SameFrameUpdateCount = SameFrameUpdateCountMap.FindOrAdd(this);
	//
	// AccumDelta += DeltaSeconds;
	// UpdateCount++;
	//
	// if (LastFrame == GFrameCounter)
	// {
	// 	SameFrameUpdateCount++;
	// }
	// LastFrame = GFrameCounter;
	//
	// const double Now = World->GetTimeSeconds();
	//
	// if (LastReportTime <= 0.0)
	// {
	// 	LastReportTime = Now;
	// }
	//
	// if (Now - LastReportTime >= 1.0)
	// {
	// 	BA_DEFAULT_LOG(LogBattle, Warning,
	// 		TEXT("[%s] RealTime=%.3f / AnimDeltaSum=%.6f / UpdateCount=%d / SameFrameUpdates=%d / WorldDelta=%.6f"),
	// 		*GetClass()->GetName(),
	// 		Now - LastReportTime,
	// 		AccumDelta,
	// 		UpdateCount,
	// 		SameFrameUpdateCount,
	// 		World->GetDeltaSeconds()
	// 	);
	//
	// 	LastReportTime = Now;
	// 	AccumDelta = 0.0;
	// 	UpdateCount = 0;
	// 	SameFrameUpdateCount = 0;
	// }
	//
	// USkeletalMeshComponent* MeshComp = GetSkelMeshComponent();
	// AActor* OwnerActor = GetOwningActor();
	//
	// if (!MeshComp || !World)
	// {
	// 	return;
	// }
	//
	// const float WorldDelta = World->GetDeltaSeconds();
	// const float DeltaRatio =
	// 	WorldDelta > KINDA_SMALL_NUMBER ? DeltaSeconds / WorldDelta : 0.0f;
	//
	// BA_DEFAULT_LOG(LogBattle, Warning,
	// 	TEXT("[%s] Delta=%.6f / WorldDelta=%.6f / Ratio=%.3f / GlobalAnimRateScale=%.3f / OwnerCustomTimeDilation=%.3f / bURO=%d / VisibilityTick=%d / TickInterval=%.6f"),
	// 	*GetClass()->GetName(),
	// 	DeltaSeconds,
	// 	WorldDelta,
	// 	DeltaRatio,
	// 	MeshComp->GlobalAnimRateScale,
	// 	OwnerActor ? OwnerActor->CustomTimeDilation : -1.0f,
	// 	MeshComp->bEnableUpdateRateOptimizations ? 1 : 0,
	// 	(int32)MeshComp->VisibilityBasedAnimTickOption,
	// 	MeshComp->PrimaryComponentTick.TickInterval
	// );
	//
	// if (!MeshComp || !World)
	// {
	// 	return;
	// }
	//
	// const float Ratio = WorldDelta > KINDA_SMALL_NUMBER
	// 	? DeltaSeconds / WorldDelta
	// 	: 0.0f;
	//
	// UE_LOG(LogBattle, Warning,
	// 	TEXT("[%s] This=%p / Owner=%s / Mesh=%s / Role=%d / RemoteRole=%d / Frame=%llu / Delta=%.6f / WorldDelta=%.6f / Ratio=%.3f / GlobalAnimRateScale=%.3f / CustomTimeDilation=%.3f / bURO=%d / VisibilityTick=%d / TickInterval=%.6f"),
	// 	*GetClass()->GetName(),
	// 	this,
	// 	OwnerActor ? *OwnerActor->GetName() : TEXT("None"),
	// 	*MeshComp->GetName(),
	// 	OwnerActor ? (int32)OwnerActor->GetLocalRole() : -1,
	// 	OwnerActor ? (int32)OwnerActor->GetRemoteRole() : -1,
	// 	GFrameCounter,
	// 	DeltaSeconds,
	// 	WorldDelta,
	// 	Ratio,
	// 	MeshComp->GlobalAnimRateScale,
	// 	OwnerActor ? OwnerActor->CustomTimeDilation : -1.0f,
	// 	MeshComp->bEnableUpdateRateOptimizations ? 1 : 0,
	// 	(int32)MeshComp->VisibilityBasedAnimTickOption,
	// 	MeshComp->PrimaryComponentTick.TickInterval
	// );
	//
	// FAnimInstanceProxy& Proxy = GetProxyOnGameThread<FAnimInstanceProxy>();
	//
	// UE_LOG(LogBattle, Warning,
	// TEXT("RootMotionMode=%d / Montage=%s / MontagePos=%.3f / MontageRate=%.3f / HasRootMotion=%d"),
	// (int32)RootMotionMode,
	// *GetNameSafe(GetCurrentActiveMontage()),
	// GetCurrentActiveMontage()
	// 	? Montage_GetPosition(GetCurrentActiveMontage())
	// 	: -1.0f,
	// GetCurrentActiveMontage()
	// 	? Montage_GetPlayRate(GetCurrentActiveMontage())
	// 	: -1.0f,
	// Proxy.GetExtractedRootMotion().bHasRootMotion ? 1 : 0
	// );
	//
	
}
