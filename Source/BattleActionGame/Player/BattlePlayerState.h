#pragma once

#include "GameFramework/PlayerState.h"
#include "BattlePlayerState.generated.h"

class UBattlePawnData;
class UBattleExperienceDefinition;

UCLASS()
class ABattlePlayerState : public APlayerState
{
	GENERATED_BODY()
public:
	
	ABattlePlayerState(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	virtual void PostInitializeComponents() override;
	
	template <class T>
	const T* GetPawnData() const {return Cast<T>(PawnData);}
	void SetPawnData(const UBattlePawnData* InPawnData);
	void OnExperienceLoaded(const UBattleExperienceDefinition* CurrentExperience);

protected:

	UPROPERTY()
	TObjectPtr<const UBattlePawnData> PawnData;
	
};
