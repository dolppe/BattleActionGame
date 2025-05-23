#pragma once

#include "CoreMinimal.h"
#include "Components/GameStateComponent.h"
#include "BattleExperienceDefinition.h"
#include "GameFeaturePluginOperationResult.h"
#include "BattleExperienceManagerComponent.generated.h"

DECLARE_MULTICAST_DELEGATE_OneParam(FOnBattleExperienceLoaded, const UBattleExperienceDefinition* );

enum class EBattleExperienceLoadState
{
	Unloaded,
	Loading,
	LoadingGameFeature,
	Loaded,
	ExecutingActions,
	Deactivating,
};

UCLASS()
class UBattleExperienceManagerComponent : public UGameStateComponent
{
	GENERATED_BODY()
public:
	UBattleExperienceManagerComponent(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	bool IsExperienceLoaded() const {return (LoadState == EBattleExperienceLoadState::Loaded) &&(CurrentExperience != nullptr);}

	// OnExperienceLoaded 델리게이트에 바인딩하거나 Experience가 로딩됐으면 바로 델리게이트를 실행시킴(Notify)
	void CallOrRegister_OnExperienceLoaded(FOnBattleExperienceLoaded::FDelegate&& Delegate);

#if WITH_SERVER_CODE
	void ServerSetCurrentExperience(FPrimaryAssetId ExperienceId);
#endif
	
	void StartExperienceLoad();
	void OnExperienceLoadComplete();
	void OnGameFeaturePluginLoadComplete(const UE::GameFeatures::FResult& Result);
	void OnExperienceFullLoadCompleted();
	const UBattleExperienceDefinition* GetCurrentExperienceChecked() const;
	
	virtual void BeginPlay() override;

private:

	UFUNCTION()
	void OnRep_CurrentExperience();
	
	void OnActionDeactivationCompleted();
	void OnAllActionsDeactivated();
	
	UPROPERTY(ReplicatedUsing=OnRep_CurrentExperience)
	TObjectPtr<const UBattleExperienceDefinition> CurrentExperience;
	
	// 로딩 상태 모니터링
	EBattleExperienceLoadState LoadState = EBattleExperienceLoadState::Unloaded;

	// Experience 로딩 완료 후 델리게이트
	FOnBattleExperienceLoaded OnExperienceLoaded;

	// 활성화된 GameFeature Plugin들
	int32 NumGameFeaturePluginsLoading = 0;
	TArray<FString> GameFeaturePluginURLs;

	// Deactivating 싱크를 위한 변수
	int32 NumObservedPausers = 0;
	int32 NumExpectedPausers = 0;
};