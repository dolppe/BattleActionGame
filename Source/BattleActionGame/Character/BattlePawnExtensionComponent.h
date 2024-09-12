#pragma once

#include "Components/GameFrameworkInitStateInterface.h"
#include "Components/PawnComponent.h"
#include "BattlePawnData.h"
#include "BattlePawnExtensionComponent.generated.h"

class UBattleAbilitySystemComponent;

UCLASS()
class UBattlePawnExtensionComponent : public UPawnComponent, public IGameFrameworkInitStateInterface
{
	GENERATED_BODY()
public:
	UBattlePawnExtensionComponent(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	// FeatureName 정의
	static const FName NAME_ActorFeatureName;

	// UPawnComponent Interface
	virtual void OnRegister() final;
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) final;

	// IGameFrameworkInitStateInterface Interface
	virtual FName GetFeatureName() const override { return NAME_ActorFeatureName;}
	virtual void OnActorInitStateChanged(const FActorInitStateChangedParams& Params) final;
	virtual bool CanChangeInitState(UGameFrameworkComponentManager* Manager, FGameplayTag CurrentState, FGameplayTag DesiredState) const final;
	virtual void CheckDefaultInitialization() final;

	// Member Methods
	template <class T>
	const T* GetPawnData() const {return Cast<T>(PawnData);}
	void SetPawnData(const UBattlePawnData* InPawnData);
	static UBattlePawnExtensionComponent* FindPawnExtensionComponent(const AActor* Actor) {return (Actor ? Actor->FindComponentByClass<UBattlePawnExtensionComponent>() : nullptr);}
	void SetupPlayerInputComponent();

	void HandleControllerChanged();

	void InitializeAbilitySystem(UBattleAbilitySystemComponent* InASC, AActor* InOwnerActor);
	void UnInitializeAbilitySystem();
	UBattleAbilitySystemComponent* GetAbilitySystemComponent() const {return AbilitySystemComponent;}

	void OnAbilitySystemInitialized_RegisterAndCall(FSimpleMulticastDelegate::FDelegate Delegate);
	void OnAbilitySystemUninitialized_Register(FSimpleMulticastDelegate::FDelegate Delegate);

	UFUNCTION()
	void OnRep_PawnData();
	
	UPROPERTY(EditInstanceOnly, ReplicatedUsing = OnRep_PawnData, Category= "Battle|Pawn")
	TObjectPtr<const UBattlePawnData> PawnData;

	UPROPERTY()
	TObjectPtr<UBattleAbilitySystemComponent> AbilitySystemComponent;

	// ASC의 Init, Uninit Delegate
	FSimpleMulticastDelegate OnAbilitySystemInitialized;
	FSimpleMulticastDelegate OnAbilitySystemUninitialized;
	
};
