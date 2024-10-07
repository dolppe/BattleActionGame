#include "BattleGameplayAbility_HitCheckAttack.h"
#include UE_INLINE_GENERATED_CPP_BY_NAME(BattleGameplayAbility_HitCheckAttack)

UBattleGameplayAbility_HitCheckAttack::UBattleGameplayAbility_HitCheckAttack(
	const FObjectInitializer& ObjectInitializer)
		: Super(ObjectInitializer)
{
}

void UBattleGameplayAbility_HitCheckAttack::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
}

void UBattleGameplayAbility_HitCheckAttack::EndAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	bool bReplicateEndAbility, bool bWasCancelled)
{

	ABattleCharacter* Character = Cast<ABattleCharacter>(ActorInfo->AvatarActor);
	
	if (GetWorld()->GetNetMode() == NM_Client)
	{
		// 테스크 실행
		UE_LOG(LogTemp, Log, TEXT("ClientEndAbility"));
	}
	else
	{
		// 몽타주 멀티캐스트 필요
		UE_LOG(LogTemp, Log, TEXT("ServerEndAbility"));
		Character->GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Walking);
	}
	
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UBattleGameplayAbility_HitCheckAttack::GetLifetimeReplicatedProps(
	TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
}

void UBattleGameplayAbility_HitCheckAttack::ServerRPCNotifyHit_Implementation(const FHitResult& HitResult,
	float HitCheckTime)
{
	Super::ServerRPCNotifyHit_Implementation(HitResult, HitCheckTime);
}

void UBattleGameplayAbility_HitCheckAttack::AttackHitConfirm(const FHitResult& HitResult)
{
	Super::AttackHitConfirm(HitResult);
}

void UBattleGameplayAbility_HitCheckAttack::OnTargetDataReadyCallback(const FGameplayAbilityTargetDataHandle& InData,
	FGameplayTag ApplicationTag)
{
	if (GetWorld()->GetNetMode() != NM_Client)
	{
		OnTargetDataReady(InData);
	}
}

void UBattleGameplayAbility_HitCheckAttack::SelectHitCheck(const FHitResult HitResult, const float AttackTime)
{
	Super::SelectHitCheck(HitResult, AttackTime);
}

void UBattleGameplayAbility_HitCheckAttack::OnCompleted()
{
	Super::OnCompleted();
}

void UBattleGameplayAbility_HitCheckAttack::OnInterrupted()
{
	Super::OnInterrupted();
}

void UBattleGameplayAbility_HitCheckAttack::OnRep_AlreadyHitActors()
{
}
