#pragma once

#include "AbilitySystemInterface.h"
#include "BattleActionGame/System/BattleGameplayTagStack.h"
#include "GameFramework/PlayerState.h"
#include "BattlePlayerState.generated.h"

class UBattlePawnData;
class UBattleExperienceDefinition;
class UBattleAbilitySystemComponent;

UENUM(BlueprintType)
enum class ECombatStatType : uint8
{
	BossDamage UMETA(DisplayName = "BossDamage"),
	ReceivedDamage UMETA(DisplayName = "ReceivedDamage"),
	ItemCount UMETA(DisplayName = "ItemCount"),
	JustDashCount UMETA(DisplayName = "JustDashCount"),
	JustClashCount UMETA(DisplayName = "JustClashCount"),
	MAX UMETA(DisplayName = "MAX"),
	
};

USTRUCT(BlueprintType)
struct FCombatStat
{
	GENERATED_BODY()

	float GetValue(ECombatStatType StatType) const
	{
		switch (StatType)
		{
		case ECombatStatType::BossDamage:
			return BossDamage;
		case ECombatStatType::ReceivedDamage:
			return ReceivedDamage;
		case ECombatStatType::ItemCount:
			return ItemCount;
		case ECombatStatType::JustDashCount:
			return JustDashCount;
		case ECombatStatType::JustClashCount:
			return JustClashCount;
			default:
				return 0.0f;
		}
	}

	void SetValue(ECombatStatType StatType, float Value)
	{
		switch (StatType)
		{
		case ECombatStatType::BossDamage:
			BossDamage = Value;
			break;
		case ECombatStatType::ReceivedDamage:
			ReceivedDamage = Value;
			break;
		case ECombatStatType::ItemCount:
			ItemCount = Value;
			break;
		case ECombatStatType::JustDashCount:
			JustDashCount = Value;
			break;
		case ECombatStatType::JustClashCount:
			JustClashCount = Value;
			break;
		}
	}
	

	UPROPERTY(BlueprintReadWrite)
	float BossDamage = 0.0f;
	
	UPROPERTY(BlueprintReadWrite)
	float ReceivedDamage = 0.0f;
	
	UPROPERTY(BlueprintReadWrite)
	int ItemCount = 0;
	
	UPROPERTY(BlueprintReadWrite)
	int JustDashCount = 0;
	
	UPROPERTY(BlueprintReadWrite)
	int JustClashCount = 0;
};



UCLASS()
class ABattlePlayerState : public APlayerState, public IAbilitySystemInterface
{
	GENERATED_BODY()
public:
	
	ABattlePlayerState(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	virtual void PostInitializeComponents() override;
	
	template <class T>
	const T* GetPawnData() const {return Cast<T>(PawnData);}
	void SetPawnData(const UBattlePawnData* InPawnData);
	void OnExperienceLoaded(const UBattleExperienceDefinition* CurrentExperience);

	UBattleAbilitySystemComponent* GetBattleAbilitySystemComponent() const {return AbilitySystemComponent;}
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly,Category=Score)
	void AddStatTagStack(FGameplayTag Tag,int32 StackCount);

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly,Category=Score)
	void RemoveStatTagStack(FGameplayTag Tag,int32 StackCount);

	UFUNCTION(BlueprintCallable, Category=Score)
	int32 GetStatTagStack(FGameplayTag Tag);

	UFUNCTION(BlueprintCallable, Category=Score)
	bool HasStatTag(FGameplayTag Tag) const;

	static const FName NAME_BattleAbilityReady;

	UFUNCTION()
	void ToggleReady();

	UFUNCTION(Server, Reliable)
	void Server_ToggleReady();

	UFUNCTION()
	void OnRep_CombatStat();

	UPROPERTY(ReplicatedUsing = OnRep_CombatStat, BlueprintReadWrite)
	FCombatStat CombatStat;
	
protected:

	UFUNCTION()
	void OnRep_PawnData();

	UPROPERTY(ReplicatedUsing = OnRep_PawnData)
	TObjectPtr<const UBattlePawnData> PawnData;

	

private:
	
	UPROPERTY(VisibleAnywhere, Category="Battle|PlayerState")
	TObjectPtr<UBattleAbilitySystemComponent> AbilitySystemComponent;

	UPROPERTY(Replicated)
	FBattleGameplayTagStackContainer StatTags;
	
	bool bIsReady = false;


};
