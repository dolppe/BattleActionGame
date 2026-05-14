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
			AController* Controller = AvatarCharacter->GetController();
			Controller->Possess(ObserverPawn);
			
			ABattlePlayerAIController* PlayerAIController = AvatarCharacter->GetAiController();
			PlayerAIController->Possess(AvatarCharacter);
		}
		else
		{
			FActorSpawnParameters SpawnParams;
			
			SpawnParams.Owner = AvatarCharacter->GetController();
			SpawnParams.Instigator = AvatarCharacter;
			FVector SpawnTransform = AvatarCharacter->GetActorLocation();
			
			ABattleObserverPawn* NewObserverPawn = GetWorld()->SpawnActor<ABattleObserverPawn>(SpawnTransform, FRotator::ZeroRotator, SpawnParams);
			
			NewObserverPawn->AttachToActor(AvatarCharacter, FAttachmentTransformRules::SnapToTargetIncludingScale);
			
			if (UBattlePawnExtensionComponent* PawnExtensionComp = Cast<UBattlePawnExtensionComponent>(AvatarCharacter->GetComponentByClass(UBattlePawnExtensionComponent::StaticClass())))
			{
				NewObserverPawn->SetDefaultData(*PawnExtensionComp->GetPawnData<UBattlePawnData>());
			}
			
			AvatarCharacter->SetObserverPawn(NewObserverPawn);
			
			AController* Controller = AvatarCharacter->GetController();
			Controller->Possess(NewObserverPawn);
			
			ABattlePlayerAIController* PlayerAIController = GetWorld()->SpawnActor<ABattlePlayerAIController>(SpawnParams);
			AvatarCharacter->SetAIController(PlayerAIController);
			PlayerAIController->Possess(AvatarCharacter);

		}
	}
	
	FTimerHandle TimerHandle;
	
	GetWorld()->GetTimerManager().SetTimer(TimerHandle,[this]()
	{
		this->EndAbility(this->GetCurrentAbilitySpecHandle(), this->GetCurrentActorInfo(), this->GetCurrentActivationInfo(), true, false);
	}, 1.f, false);
	
	//EndAbility(GetCurrentAbilitySpecHandle(), ActorInfo, ActivationInfo, true, false);
}

void UBattleGameplayAbility_ConvertToAI::EndAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	bool bReplicateEndAbility, bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}
