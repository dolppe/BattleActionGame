#include "BattlePlayerState.h"

#include "GameplayMessageSubsystem.h"
#include "Abilities/GameplayAbilityTypes.h"
#include "BattleActionGame/BattleGameplayTags.h"
#include "BattleActionGame/BattleLogChannels.h"
#include "BattleActionGame/AbilitySystem/BattleAbilitySet.h"
#include "BattleActionGame/AbilitySystem/BattleAbilitySystemComponent.h"
#include "BattleActionGame/AbilitySystem/Attributes/BattleCombatSet.h"
#include "BattleActionGame/AbilitySystem/Attributes/BattleHealthSet.h"
#include "BattleActionGame/Character/BattlePawnData.h"
#include "BattleActionGame/GameModes/BattleExperienceManagerComponent.h"
#include "BattleActionGame/GameModes/BattleExperienceDefinition.h"
#include "BattleActionGame/GameModes/BattleGameMode.h"
#include "BattleActionGame/Messages/BattleVerbMessage.h"
#include "Components/GameFrameworkComponentManager.h"
#include "Net/UnrealNetwork.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(BattlePlayerState)

const FName ABattlePlayerState::NAME_BattleAbilityReady("BattleAbilitiesReady");

ABattlePlayerState::ABattlePlayerState(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	AbilitySystemComponent = ObjectInitializer.CreateDefaultSubobject<UBattleAbilitySystemComponent>(this, TEXT("AbilitySystemcomponent"));
	AbilitySystemComponent->SetIsReplicated(true);
	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Mixed);

	//CreateDefaultSubobject<UBattleHealthSet>(TEXT("HealthSet"));
	//CreateDefaultSubobject<UBattleCombatSet>(TEXT("CombatSet"));

	NetUpdateFrequency = 100.0f;
	
}

void ABattlePlayerState::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	check(AbilitySystemComponent);
	{
		FGameplayAbilityActorInfo* ActorInfo = AbilitySystemComponent->AbilityActorInfo.Get();
		check(ActorInfo->OwnerActor == this);
		check(ActorInfo->OwnerActor == ActorInfo->AvatarActor);		
	}
	// 아직 Pawn이 안붙었기에 nullptr이긴 함. 초기화 느낌으로 nullptr로 초기화.
	AbilitySystemComponent->InitAbilityActorInfo(this, GetPawn());

	if (GetNetMode() != NM_Client)
	{
		AGameStateBase* GameState = GetWorld()->GetGameState();
		check(GameState);
		
		UBattleExperienceManagerComponent* ExperienceManagerComponent = GameState->FindComponentByClass<UBattleExperienceManagerComponent>();
		check(ExperienceManagerComponent);

		// Experience가 로드되면 PawnData를 설정하기 위해 델리게이트 걸어둠.
		ExperienceManagerComponent->CallOrRegister_OnExperienceLoaded(FOnBattleExperienceLoaded::FDelegate::CreateUObject(this, &ThisClass::OnExperienceLoaded));
	}
	
}

void ABattlePlayerState::SetPawnData(const UBattlePawnData* InPawnData)
{
	check(InPawnData);
	
	if (GetLocalRole() != ROLE_Authority)
	{
		return;
	}

	// PawnData가 두번 생성되는 것을 막음.
	check(!PawnData);
	
	PawnData = InPawnData;

	for (UBattleAbilitySet* AbilitySet : PawnData->AbilitySets)
	{
		if (AbilitySet)
		{
			AbilitySet->GiveToAbilitySystem(AbilitySystemComponent, nullptr);
		}
	}

	UGameFrameworkComponentManager::SendGameFrameworkComponentExtensionEvent(this, NAME_BattleAbilityReady);

	ForceNetUpdate();
}

void ABattlePlayerState::OnExperienceLoaded(const UBattleExperienceDefinition* CurrentExperience)
{
	if (ABattleGameMode* GameMode = GetWorld()->GetAuthGameMode<ABattleGameMode>())
	{
		const UBattlePawnData* NewPawnData = GameMode->GetPawnDataForController(GetOwningController());
		check(NewPawnData);

		SetPawnData(NewPawnData);
	}
}

void ABattlePlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	FDoRepLifetimeParams SharedParams;
	SharedParams.bIsPushBased = true;

	DOREPLIFETIME_WITH_PARAMS_FAST(ThisClass, PawnData, SharedParams);

	DOREPLIFETIME(ThisClass, StatTags);
}

UAbilitySystemComponent* ABattlePlayerState::GetAbilitySystemComponent() const
{
	return GetBattleAbilitySystemComponent();
}

void ABattlePlayerState::AddStatTagStack(FGameplayTag Tag, int32 StackCount)
{
	StatTags.AddStack(Tag, StackCount);
}

void ABattlePlayerState::RemoveStatTagStack(FGameplayTag Tag, int32 StackCount)
{
	StatTags.RemoveStack(Tag, StackCount);
}

int32 ABattlePlayerState::GetStatTagStack(FGameplayTag Tag)
{
	return StatTags.GetStackCount(Tag);
}

bool ABattlePlayerState::HasStatTag(FGameplayTag Tag) const
{
	return StatTags.ContainsTag(Tag);
}

void ABattlePlayerState::ToggleReady()
{
	bIsReady = !bIsReady;
	
	FBattleVerbMessage Message;
	Message.Instigator = this;
	
	if (bIsReady)
	{
		Message.Verb = FBattleGameplayTags::Get().Gameplay_Message_Ready;	
	}
	else
	{
		Message.Verb = FBattleGameplayTags::Get().Gameplay_Message_Unready;	
	}

	UGameplayMessageSubsystem& MessageSubsystem = UGameplayMessageSubsystem::Get(GetWorld());
	MessageSubsystem.BroadcastMessage(Message.Verb, Message);
	
	if (!HasAuthority())
	{
		Server_ToggleReady();
	}
}

void ABattlePlayerState::Server_ToggleReady_Implementation()
{
	BA_LOG(LogBattle,Log,TEXT("%s"), bIsReady?TEXT("IsReady"):TEXT("IsNotReady"));
	bIsReady = !bIsReady;
	
	FBattleVerbMessage Message;
	Message.Instigator = this;
	
	if (bIsReady)
	{
		Message.Verb = FBattleGameplayTags::Get().Gameplay_Message_Ready;	
	}
	else
	{
		Message.Verb = FBattleGameplayTags::Get().Gameplay_Message_Unready;	
	}

	UGameplayMessageSubsystem& MessageSubsystem = UGameplayMessageSubsystem::Get(GetWorld());
	MessageSubsystem.BroadcastMessage(Message.Verb, Message);
	
}

void ABattlePlayerState::OnRep_PawnData()
{
}


