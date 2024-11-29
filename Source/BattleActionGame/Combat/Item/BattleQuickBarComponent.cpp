#include "BattleQuickBarComponent.h"

#include "GameplayMessageSubsystem.h"
#include "Net/UnrealNetwork.h"
#include "NativeGameplayTags.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(BattleQuickBarComponent)


UE_DEFINE_GAMEPLAY_TAG_STATIC(TAG_Battle_QuickBar_Message_SlotsChanged, "Battle.QuickBar.Message.SlotsChanged");

UBattleQuickBarComponent::UBattleQuickBarComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	SetIsReplicatedByDefault(true);
}

void UBattleQuickBarComponent::CycleActiveSlotForward()
{
	if (Slots.Num() <2)
	{
		return;
	}

	const int8 OldIndex = (ActiveSlotIndex < 0 ? Slots.Num()-1 : ActiveSlotIndex);
	int32 NewIndex = ActiveSlotIndex;
	do
	{
		NewIndex = (NewIndex+1) %Slots.Num();
		if (Slots[NewIndex].ItemDef != nullptr)
		{
			SetActiveSlotIndex(NewIndex);
			return;
		}
	} while (NewIndex != OldIndex);
}

void UBattleQuickBarComponent::CycleActiveSlotBackward()
{
	if (Slots.Num() <2)
	{
		return;
	}

	const int8 OldIndex = (ActiveSlotIndex < 0 ? Slots.Num()-1 : ActiveSlotIndex);
	int32 NewIndex = ActiveSlotIndex;
	do
	{
		NewIndex = (NewIndex-1 + Slots.Num()) %Slots.Num();
		if (Slots[NewIndex].ItemDef != nullptr)
		{
			SetActiveSlotIndex(NewIndex);
			return;
		}
	} while (NewIndex != OldIndex);
}

void UBattleQuickBarComponent::SetActiveSlotIndex_Implementation(int NewIndex)
{
	if (Slots.IsValidIndex(NewIndex) && (ActiveSlotIndex != NewIndex))
	{
		ActiveSlotIndex = NewIndex;

		OnRep_ActiveSlotIndex();
	}
}

void UBattleQuickBarComponent::AddItemToSlot(int SlotIndex, UBattleItemData* Item, int Quantity)
{
	if (Slots.IsValidIndex(SlotIndex) && (Item != nullptr))
	{
		if (Slots[SlotIndex].ItemDef == nullptr)
		{
			Slots[SlotIndex].ItemDef = Item;
			Slots[SlotIndex].Num = Quantity;
			OnRep_Slots();
		}
	}
}

void UBattleQuickBarComponent::RemoveItemFromSlot(int SlotIndex)
{
	if (ActiveSlotIndex == SlotIndex)
	{
		ActiveSlotIndex = 0;
	}

	if (Slots.IsValidIndex(SlotIndex))
	{
		if (Slots[SlotIndex].ItemDef != nullptr)
		{
			Slots[SlotIndex].ItemDef = nullptr;
			Slots[SlotIndex].Num = 0;
			OnRep_Slots();
		}
	}
}

void UBattleQuickBarComponent::OnRep_Slots()
{
	FBattleQuickBarSlotsChangedMessage Message;
	Message.Owner = GetOwner();
	Message.Slots = Slots;
	Message.ActiveIndex = ActiveSlotIndex;

	UGameplayMessageSubsystem& MessageSystem = UGameplayMessageSubsystem::Get(this);
	MessageSystem.BroadcastMessage(TAG_Battle_QuickBar_Message_SlotsChanged, Message);
}

void UBattleQuickBarComponent::OnRep_ActiveSlotIndex()
{
	FBattleQuickBarSlotsChangedMessage Message;
	Message.Owner = GetOwner();
	Message.Slots = Slots;
	Message.ActiveIndex = ActiveSlotIndex;

	UGameplayMessageSubsystem& MessageSystem = UGameplayMessageSubsystem::Get(this);
	MessageSystem.BroadcastMessage(TAG_Battle_QuickBar_Message_SlotsChanged, Message);
}

void UBattleQuickBarComponent::BeginPlay()
{
	if (Slots.Num() < NumSlots)
	{
		Slots.AddDefaulted(NumSlots - Slots.Num());
	}
	
	Super::BeginPlay();
}

void UBattleQuickBarComponent::GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass, Slots);
	DOREPLIFETIME(ThisClass, ActiveSlotIndex);
}
