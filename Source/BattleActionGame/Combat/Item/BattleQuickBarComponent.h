#pragma once

#include "Components/ControllerComponent.h"
#include "BattleQuickBarComponent.generated.h"

class UBattleItemData;

USTRUCT(BlueprintType)
struct FBattleItemInfo
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite)
	TObjectPtr<UBattleItemData> ItemDef = nullptr;

	UPROPERTY(BlueprintReadWrite)
	int Num = 0;
	
};

UCLASS(Blueprintable, meta=(BlueprintSpawnableComponent))
class UBattleQuickBarComponent : public UControllerComponent
{
	GENERATED_BODY()
public:

	UBattleQuickBarComponent(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	UFUNCTION(BlueprintCallable, Category="Battle|Item")
	void CycleActiveSlotForward();

	UFUNCTION(BlueprintCallable, Category="Battle|Item")
	void CycleActiveSlotBackward();

	UFUNCTION(Server, Reliable, BlueprintCallable, Category="Battle")
	void SetActiveSlotIndex(int NewIndex);

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly)
	void AddItemToSlot(int SlotIndex, UBattleItemData* Item, int Quantity);

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly)
	void RemoveItemFromSlot(int SlotIndex);

	FBattleItemInfo* GetActiveSlotIndex();

	bool AddItemQuantity(UBattleItemData* Item, int Quantity);

	bool UseItemQuantity(UBattleItemData* Item, int Quantity);

	int GetItemSlotIndex(UBattleItemData* Item);
	
protected:

	UFUNCTION()
	void OnRep_Slots();

	UFUNCTION()
	void OnRep_ActiveSlotIndex();

	UPROPERTY()
	int NumSlots = 4;

	virtual void BeginPlay() override;
	
private:

	UPROPERTY(ReplicatedUsing=OnRep_Slots)
	TArray<FBattleItemInfo> Slots;

	UPROPERTY(ReplicatedUsing=OnRep_ActiveSlotIndex)
	int ActiveSlotIndex = 0;
	
};

USTRUCT(BlueprintType)
struct FBattleQuickBarSlotsChangedMessage
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, Category="Item")
	TObjectPtr<AActor> Owner = nullptr;

	UPROPERTY(BlueprintReadOnly, Category="Item")
	TArray<FBattleItemInfo> Slots;

	UPROPERTY(BlueprintReadOnly, Category="Item")
	int ActiveIndex = 0;
};

