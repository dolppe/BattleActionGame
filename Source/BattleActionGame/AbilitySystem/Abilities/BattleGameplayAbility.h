#pragma once

#include "Abilities/GameplayAbility.h"
#include "BattleGameplayAbility.generated.h"

UENUM()
enum class EBattleAbilityActivationPolicy : uint8
{
	// Input이 Trigger 되었을 경우 실행 (Pressed/Released)
	OnInputTriggered,
	// Input이 Held 되어 있을 경우
	WhileInputActive,
	// Avatar가 생성되엇을 경우 바로 할당
	OnSpawn,
};

class UBattleAbilityCost;
class ABattleCharacter;
class AController;
class UBattleAbilitySystemComponent;
class ABattlePlayerController;
class IBattleAbilitySourceInterface;
class UBattleCameraMode;

UCLASS(Abstract)
class UBattleGameplayAbility : public UGameplayAbility
{
	GENERATED_BODY()

	friend class UBattleAbilitySystemComponent;
	
public:
	UBattleGameplayAbility(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	UFUNCTION(BlueprintCallable, Category = "Battle|Ability")
	UBattleAbilitySystemComponent* GetBattleAbilitySystemComponentFromActorInfo() const;

	UFUNCTION(BlueprintCallable, Category = "Battle|Ability")
	ABattlePlayerController* GetBattlePlayerControllerFromActorInfo() const;
	
	UFUNCTION(BlueprintCallable, Category = "Battle|Ability")
	AController* GetControllerFromActorInfo() const;
	
	UFUNCTION(BlueprintCallable, Category = "Battle|Ability")
	ABattleCharacter* GetBattleCharacterFromActorInfo() const;

	UFUNCTION(BlueprintCallable, Category = "Battle|Ability")
	UBattleHeroComponent* GetHeroComponentFromActorInfo() const;
	
	EBattleAbilityActivationPolicy GetActivationPolicy() const {return ActivationPolicy;}

	void TryActivateAbilityOnSpawn(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec) const;

	UFUNCTION(BlueprintCallable, Category = "Battle|Ability")
	void SetCameraMode(TSubclassOf<UBattleCameraMode> CameraMode);

	UFUNCTION(BlueprintCallable, Category="Battle|Ability")
	void ClearCameraMode();

	//virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
protected:

	virtual void OnGiveAbility(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec) override;
	virtual void OnRemoveAbility(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec) override;
	virtual bool CheckCost(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, FGameplayTagContainer* OptionalRelevantTags) const override;
	virtual void ApplyCost(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo) const override;
	virtual bool DoesAbilitySatisfyTagRequirements(const UAbilitySystemComponent& AbilitySystemComponent, const FGameplayTagContainer* SourceTags, const FGameplayTagContainer* TargetTags, FGameplayTagContainer* OptionalRelevantTags) const override;
	virtual FGameplayEffectContextHandle MakeEffectContext(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo) const override;

	virtual void OnPawnAvatarSet();
	
	virtual void GetAbilitySource(FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, float& OutSourceLevel, const IBattleAbilitySourceInterface*& OutAbilitySource, AActor*& OutEffectCauser) const;
	
	UFUNCTION(BlueprintImplementableEvent, Category=Ability, DisplayName="OnAbilityAdded")
	void K2_OnAbilityAdded();

	UFUNCTION(BlueprintImplementableEvent, Category=Ability, DisplayName="OnAbilityRemoved")
	void K2_OnAbilityRemoved();

	UFUNCTION(BlueprintImplementableEvent, Category=Ability, DisplayName="OnPawnAvatarSet")
	void K2_OnPawnAvatarSet();
	
protected:
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Battle|AbilityActivation")
	EBattleAbilityActivationPolicy ActivationPolicy;

	UPROPERTY(EditDefaultsOnly, Instanced, Category=Costs)
	TArray<TObjectPtr<UBattleAbilityCost>> AdditionalCosts;


	TSubclassOf<UBattleCameraMode> ActiveCameraMode;
};
