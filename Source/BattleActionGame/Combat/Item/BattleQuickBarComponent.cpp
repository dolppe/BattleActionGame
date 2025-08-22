#include "BattleQuickBarComponent.h"

#include "BattleItemData.h"
#include "GameplayMessageSubsystem.h"
#include "Net/UnrealNetwork.h"
#include "NativeGameplayTags.h"
#include "BattleActionGame/BattleLogChannels.h"
#include "BattleActionGame/Character/BattleCharacterBase.h"
#include "BattleActionGame/Combat/BattleCombatManagerComponent.h"
#include "BattleActionGame/Player/BattlePlayerState.h"

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
	BA_SUBLOG(LogBattle, Log, TEXT("OnRep_Slots"));
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

FBattleItemInfo* UBattleQuickBarComponent::GetActiveSlotItem()
{
	if (Slots.IsValidIndex(ActiveSlotIndex))
	{
		if (Slots[ActiveSlotIndex].ItemDef != nullptr)
		{
			return &Slots[ActiveSlotIndex];
		}
	}

	return nullptr;
}

bool UBattleQuickBarComponent::CheckUseActiveSlot(int Quantity)
{
	if (Slots[ActiveSlotIndex].ItemDef != nullptr )
	{
		if (Slots[ActiveSlotIndex].Num >= Quantity)
		{
			return true;
		}
	}

	return false;
}

bool UBattleQuickBarComponent::AddItemQuantity(UBattleItemData* Item, int Quantity)
{
	
	int Idx = GetItemSlotIndex(Item);

	if (Idx == -1)
	{
		return false;
	}
	else
	{
		Slots[Idx].Num += Quantity;
		OnRep_Slots();
		return true;
	}
	
	return false;
}

bool UBattleQuickBarComponent::UseItemQuantity(int Quantity)
{	
	int Idx = ActiveSlotIndex;

	BA_SUBLOG(LogBattle, Log ,TEXT("UseItemQuantity Start: %d"), Slots[ActiveSlotIndex].Num);
	if (Idx == -1)
	{
		return false;
	}
	else if (Slots.IsValidIndex(ActiveSlotIndex))
	{
		if (Slots[Idx].Num < Quantity)
		{
			return false;
		}
		else if (Slots[Idx].Num == Quantity)
		{
			if (UBattleCombatManagerComponent* CombatManagerComponent = Cast<UBattleCombatManagerComponent>(GetPawn<ABattleCharacterBase>()->GetComponentByClass(UBattleCombatManagerComponent::StaticClass())))
			{
				CombatManagerComponent->CurrentUsedItemInfo = Slots[ActiveSlotIndex];
			}
			RemoveItemFromSlot(Idx);
		}
		else
		{
			if (UBattleCombatManagerComponent* CombatManagerComponent = Cast<UBattleCombatManagerComponent>(GetPawn<ABattleCharacterBase>()->GetComponentByClass(UBattleCombatManagerComponent::StaticClass())))
			{
				CombatManagerComponent->CurrentUsedItemInfo = Slots[ActiveSlotIndex];
			}
			OnRep_Slots();
			Slots[Idx].Num -= Quantity;
		}

		if (APlayerController* PC = Cast<APlayerController>(GetOwner()))
		{
			if (ABattlePlayerState* PS = PC->GetPlayerState<ABattlePlayerState>())
			{
				PS->CombatStat.ItemCount++;
			}
		}
		
		BA_SUBLOG(LogBattle, Log ,TEXT("UseItemQuantity End: %d"), Slots[ActiveSlotIndex].Num);
		return true;
	}
	return false;
	
}

int UBattleQuickBarComponent::GetItemSlotIndex(UBattleItemData* Item)
{
	int idx = 0;
	for (FBattleItemInfo& BattleItemInfo : Slots)
	{
		if (BattleItemInfo.ItemDef.GetClass() == Item->GetClass())
		{
			return idx; 
		}

		idx++;
	}
	return -1;
}

int UBattleQuickBarComponent::GetBeforeActiveSlotIdx()
{
	if (ActiveSlotIndex == -1 || Slots[ActiveSlotIndex].ItemDef == nullptr)
	{
		return -1;
	}
	else
	{
		int OriIdx = ActiveSlotIndex;

		int CurIdx = (OriIdx ==0 ? Slots.Num()-1 : OriIdx-1);

		while (CurIdx != OriIdx)
		{
			if (Slots[CurIdx].ItemDef != nullptr)
			{
				return CurIdx;
			}
			else
			{
				CurIdx = (CurIdx == 0 ? Slots.Num()-1 : CurIdx-1);
			}
		}
		return -1;
	}
}

int UBattleQuickBarComponent::GetActiveSlotIdx()
{
	if (Slots[ActiveSlotIndex].ItemDef != nullptr)
	{
		return ActiveSlotIndex;
	}
	return -1;
}

int UBattleQuickBarComponent::GetAfterActiveSlotIdx()
{
	if (ActiveSlotIndex == -1 || Slots[ActiveSlotIndex].ItemDef == nullptr)
	{
		return -1;
	}
	else
	{
		int OriIdx = ActiveSlotIndex;

		int CurIdx = (OriIdx ==Slots.Num()-1 ? 0 : OriIdx+1);

		while (CurIdx != OriIdx)
		{
			if (Slots[CurIdx].ItemDef != nullptr)
			{
				return CurIdx;
			}
			else
			{
				CurIdx = (CurIdx == Slots.Num()-1 ? 0 : CurIdx+1);
			}
		}
		return -1;
	}
}

FBattleItemInfo& UBattleQuickBarComponent::GetSlotsItem(int SlotIdx)
{
	return Slots[SlotIdx];
}

void UBattleQuickBarComponent::OnRep_Slots()
{
	BA_SUBLOG(LogBattle, Log, TEXT("OnRep_Slots, %s"), *Slots[ActiveSlotIndex].ItemDef.GetName());
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

