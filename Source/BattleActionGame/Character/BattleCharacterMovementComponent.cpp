#include "BattleCharacterMovementComponent.h"

#include "AbilitySystemComponent.h"
#include "AbilitySystemGlobals.h"
#include "BattleActionGame/BattleGameplayTags.h"
#include "BattleActionGame/BattleLogChannels.h"
#include "GameFramework/Character.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(BattleCharacterMovementComponent)

UBattleCharacterMovementComponent::UBattleCharacterMovementComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void UBattleCharacterMovementComponent::PerformMovement(float DeltaTime)
{
	Super::PerformMovement(DeltaTime);
}

void UBattleCharacterMovementComponent::TickComponent(float DeltaTime, ELevelTick TickType,
	FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

FRotator UBattleCharacterMovementComponent::GetDeltaRotation(float DeltaTime) const
{
	if (UAbilitySystemComponent* ASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(GetOwner()))
	{
		if (ASC->HasMatchingGameplayTag(FBattleGameplayTags::Get().Block_Movement_AllowRotation))
		{
			return Super::GetDeltaRotation(DeltaTime);
		}
		if (ASC->HasMatchingGameplayTag(FBattleGameplayTags::Get().Block_Movement))
		{
			return FRotator(0,0,0);
		}
	}

	return Super::GetDeltaRotation(DeltaTime);
}

float UBattleCharacterMovementComponent::GetMaxSpeed() const
{
	if (UAbilitySystemComponent* ASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(GetOwner()))
	{
		if (ASC->HasMatchingGameplayTag(FBattleGameplayTags::Get().Block_Movement))
		{
			return 0;
		}
	}

	return Super::GetMaxSpeed();
}

void UBattleCharacterMovementComponent::ReplicateMoveToServer(float DeltaTime, const FVector& NewAcceleration)
{
	FNetworkPredictionData_Client_Character* ClientData = GetPredictionData_Client_Character();
	if (!ClientData)
	{
		return;
	}
	
	// Update our delta time for physics simulation.
	
	float CurrentTimeStamp = ClientData->CurrentTimeStamp;
	
	if( CurrentTimeStamp > MinTimeBetweenTimeStampResets )
	{
		CurrentTimeStamp -= MinTimeBetweenTimeStampResets;
	}
	
	float ClientDeltaTime = DeltaTime;
	
	if( ClientData->SavedMoves.Num() > 0 )
	{
		const FSavedMovePtr& PreviousMove = ClientData->SavedMoves.Last();
		if( !PreviousMove->bOldTimeStampBeforeReset )
		{
			// How server will calculate its deltatime to update physics.
			const float ServerDeltaTime = CurrentTimeStamp + DeltaTime - PreviousMove->TimeStamp;
			// Have client always use the Server's DeltaTime. Otherwise our physics simulation will differ and we'll trigger too many position corrections and increase our network traffic.
			ClientDeltaTime = ServerDeltaTime;
		}
	}
	
	float NewDeltaTime = FMath::Min(ClientDeltaTime, ClientData->MaxMoveDeltaTime * CharacterOwner->GetActorTimeDilation());
	float PendingMoveDelta = 0.0f;
	if (const FSavedMove_Character* PendingMove = ClientData->PendingMove.Get())
	{
		PendingMoveDelta = PendingMove->DeltaTime;
	}
	BA_DEFAULT_LOG(LogBattle, Log, TEXT("ReplicateMoveToServer BeforeDelta = %f, NewDelta = %f, PendingMoveDelta = %f"), DeltaTime, NewDeltaTime, PendingMoveDelta);
	
	
	Super::ReplicateMoveToServer(DeltaTime, NewAcceleration);
	
	if (ClientData->SavedMoves.Num() > 0)
	{
		const FSavedMovePtr& PreviousMove = ClientData->SavedMoves.Last();
		BA_DEFAULT_LOG(LogBattle, Log, TEXT("After ReplicateMoveToServer MoveDelta => %f"), PreviousMove->DeltaTime);
	}
}

