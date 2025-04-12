#include "BattleGameplayAbility_JustDash.h"

#include "AbilitySystemComponent.h"
#include "GameplayTagsManager.h"
#include "BattleActionGame/BattleGameplayTags.h"
#include "BattleActionGame/BattleLogChannels.h"
#include "BattleActionGame/Combat/BattleCombatManagerComponent.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(BattleGameplayAbility_JustDash)

UBattleGameplayAbility_JustDash::UBattleGameplayAbility_JustDash(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	// 서버가 특정 기간동안 상태를 유지
	// 클라는 이펙트와 함께 특수 키 입력이 가능한 상태로 변경
	// 클라에서 특수키 입력하면 반격 Ability가 실행되도록 (서버에서 판정)
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::ServerOnly;

	UGameplayTagsManager::Get().CallOrRegister_OnDoneAddingNativeTagsDelegate(FSimpleDelegate::CreateUObject(this, &ThisClass::DoneAddingNativeTags));
}

void UBattleGameplayAbility_JustDash::DoneAddingNativeTags()
{
	if (HasAnyFlags(RF_ClassDefaultObject))
	{
		FAbilityTriggerData TriggerData;
		TriggerData.TriggerTag = FBattleGameplayTags::Get().GameplayEvent_JustDash;
		TriggerData.TriggerSource = EGameplayAbilityTriggerSource::GameplayEvent;
		AbilityTriggers.Add(TriggerData);
	}
}

void UBattleGameplayAbility_JustDash::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
                                                      const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
                                                      const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);


	if (HasAuthority(&ActivationInfo))
	{
		BA_DEFAULT_LOG(LogBattle, Log, TEXT("SErverJustDash"));
		FGameplayCueParameters Params;
		Params.Instigator = const_cast<AActor*>(TriggerEventData->Instigator.Get());

		UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo();
		
		ASC->ExecuteGameplayCue(GlobalJustDashEffectTag, Params);
		ASC->ExecuteGameplayCue(OwnerJustDashEffectTag, Params);

		GetAbilitySystemComponentFromActorInfo()->ApplyGameplayEffectToSelf(GE_AllowedCounterAttack.GetDefaultObject(), 1.0f,ASC->MakeEffectContext());

		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
	}

}

void UBattleGameplayAbility_JustDash::EndAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	bool bReplicateEndAbility, bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}
