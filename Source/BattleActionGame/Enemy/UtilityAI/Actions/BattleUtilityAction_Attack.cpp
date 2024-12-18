#include "BattleUtilityAction_Attack.h"

#include "AbilitySystemComponent.h"
#include "BattleActionGame/BattleGameplayTags.h"
#include "BattleActionGame/Character/BattleCharacterBase.h"
#include "BattleActionGame/Combat/BattleGameplayAbility_Attack_Parent.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(BattleUtilityAction_Attack)

UBattleUtilityAction_AttackSingle::UBattleUtilityAction_AttackSingle()
{
	Priority = 3;
}

void UBattleUtilityAction_AttackSingle::StartAction()
{
	Super::StartAction();

	ASC = CachedAIComponent->ConsiderList->MyCharacter->GetAbilitySystemComponent();

	Age = 0.0f;

	FTimerHandle TimerHandle;

	GetWorld()->GetTimerManager().ClearTimer(TimerHandle);
	GetWorld()->GetTimerManager().SetTimer(TimerHandle, this, &UBattleUtilityAction_AttackSingle::UpdateAge, CoolTime, false);
	
	if (ASC)
	{
		ASC->TryActivateAbilityByClass(GA_Attack);
	}
	
}

void UBattleUtilityAction_AttackSingle::EndAction()
{

	Super::EndAction();
}

bool UBattleUtilityAction_AttackSingle::TickAction(float DeltaTime)
{
	if (ASC)
	{
		if (!ASC->HasMatchingGameplayTag(FBattleGameplayTags::Get().Status_Attack_Attacking))
		{
			return true;
		}
		return false;
	}

	return true;
}

float UBattleUtilityAction_AttackSingle::EvaluateScore(const UConsiderationFactors* ConsiderList)
{
	return Super::EvaluateScore(ConsiderList);
}

void UBattleUtilityAction_AttackSingle::UpdateAge()
{
	Age = 1.0f;
}

