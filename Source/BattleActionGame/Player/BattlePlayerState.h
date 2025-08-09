#pragma once

#include "AbilitySystemInterface.h"
#include "BattleActionGame/System/BattleGameplayTagStack.h"
#include "GameFramework/PlayerState.h"
#include "BattlePlayerState.generated.h"

class UBattlePawnData;
class UBattleExperienceDefinition;
class UBattleAbilitySystemComponent;


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

	UFUNCTION(Server, Reliable)
	void Server_ToggleReady();
	
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
