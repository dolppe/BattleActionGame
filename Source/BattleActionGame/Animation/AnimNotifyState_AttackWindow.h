#pragma once
#include "Animation/AnimNotifies/AnimNotifyState.h"

#include "AnimNotifyState_AttackWindow.generated.h"

class UAttackCollisionData;
class UBattleCombatManagerComponent;
class ABattleCharacterBase;

UCLASS()
class UAnimNotifyState_AttackWindow : public UAnimNotifyState
{
	GENERATED_BODY()

public:

	UAnimNotifyState_AttackWindow();

	virtual void NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference) override;
	virtual void NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) override;


	virtual void NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float FrameDeltaTime, const FAnimNotifyEventReference& EventReference) override;

protected:

	UPROPERTY(EditAnywhere)
	int AttackIdx;

	UPROPERTY(EditAnywhere, Instanced, BlueprintReadOnly)
	TArray<UAttackCollisionData*> CollisionDatas;

private:

	
	UBattleCombatManagerComponent* CachedCombatManager = nullptr;
	ABattleCharacterBase* CachedOwnerCharacter = nullptr;
	
};
