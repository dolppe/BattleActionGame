#include "BattleGameplayAbility_ConvertToAI.h"

#include "BattleActionGame/Character/BattleCharacter.h"
#include "BattleActionGame/Character/BattleObserverPawn.h"
#include "BattleActionGame/Character/BattlePawnExtensionComponent.h"
#include "BattleActionGame/Player/BattlePlayerAIController.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(BattleGameplayAbility_ConvertToAI)

UBattleGameplayAbility_ConvertToAI::UBattleGameplayAbility_ConvertToAI(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::ServerOnly;
}

void UBattleGameplayAbility_ConvertToAI::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
	
	if (ABattleCharacter* AvatarCharacter = Cast<ABattleCharacter>(GetAvatarActorFromActorInfo()))
	{
		if (ABattleObserverPawn* ObserverPawn = AvatarCharacter->GetObserverPawn())
		{
			if (AController* Controller = AvatarCharacter->GetController())
			{
				if (ABattlePlayerAIController* PlayerAIController = AvatarCharacter->GetAiController())
				{
					Controller->Possess(ObserverPawn);			
					PlayerAIController->Possess(AvatarCharacter);
					FTimerHandle TimerHandle;
	
					GetWorld()->GetTimerManager().SetTimer(TimerHandle,[this]()
					{
						this->EndAbility(this->GetCurrentAbilitySpecHandle(), this->GetCurrentActorInfo(), this->GetCurrentActivationInfo(), true, false);
					}, 1.f, false);
					return;
				}
			}
		}
		else
		{
			if (UBattlePawnExtensionComponent* PawnExtensionComp = Cast<UBattlePawnExtensionComponent>(AvatarCharacter->GetComponentByClass(UBattlePawnExtensionComponent::StaticClass())))
			{
				if (AController* Controller = AvatarCharacter->GetController())
				{
					if (const UBattlePawnData* PawnData = PawnExtensionComp->GetPawnData<UBattlePawnData>())
					{
						if (CheckValidPawnDataForConvertToAI(*PawnData))
						{
							FActorSpawnParameters SpawnParams;
			
							SpawnParams.Owner = AvatarCharacter->GetController();
							SpawnParams.Instigator = AvatarCharacter;
							{
								FVector SpawnTransform = AvatarCharacter->GetActorLocation();
								ABattleObserverPawn* NewObserverPawn = GetWorld()->SpawnActor<ABattleObserverPawn>(SpawnTransform, FRotator::ZeroRotator, SpawnParams);
								NewObserverPawn->AttachToActor(AvatarCharacter, FAttachmentTransformRules::SnapToTargetIncludingScale);
								NewObserverPawn->SetDefaultData(*PawnData);
								AvatarCharacter->SetObserverPawn(NewObserverPawn);
						
								Controller->Possess(NewObserverPawn);
							}
							{
								ABattlePlayerAIController* PlayerAIController = GetWorld()->SpawnActor<ABattlePlayerAIController>(SpawnParams);
								AvatarCharacter->SetAIController(PlayerAIController);
						
								PlayerAIController->SetAttackGA(PawnData->AIComboGA, PawnData->AIComboStrongGA);
								PlayerAIController->Possess(AvatarCharacter);
							}
							
							FTimerHandle TimerHandle;
	
							GetWorld()->GetTimerManager().SetTimer(TimerHandle,[this]()
							{
								this->EndAbility(this->GetCurrentAbilitySpecHandle(), this->GetCurrentActorInfo(), this->GetCurrentActivationInfo(), true, false);
							}, 1.f, false);
							return;
						}
					}
				}
			}
		}
	}
	
	
	
	EndAbility(GetCurrentAbilitySpecHandle(), ActorInfo, ActivationInfo, true, true);
}

void UBattleGameplayAbility_ConvertToAI::EndAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	bool bReplicateEndAbility, bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

bool UBattleGameplayAbility_ConvertToAI::CheckValidPawnDataForConvertToAI(const UBattlePawnData& PawnData)
{
	if (PawnData.ObserverInputData == nullptr)
	{
		return false;
	}
	if (PawnData.ObserverDefaultCameraMode == nullptr)
	{
		return false;
	}
	if (PawnData.ObserverInputConfigs.IsEmpty())
	{
		return false;
	}
	if (PawnData.AIComboGA == nullptr)
	{
		return false;
	}
	if (PawnData.AIComboStrongGA == nullptr)
	{
		return false;
	}
	
	return true;
}
