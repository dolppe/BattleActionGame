#pragma once
#include "ModularCharacter.h"

#include "BattleCharacter.generated.h"

class UBattlePawnExtensionComponent;
class UBattleCameraComponent;

UCLASS()
class ABattleCharacter : public AModularCharacter
{
	GENERATED_BODY()
public:
	ABattleCharacter(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());



protected:

	virtual void PossessedBy(AController* NewController) override;
	virtual void UnPossessed() override;

	
private:
	// PawnExtensionComponent는 SubObject로 붙여둠.
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Battle|Character", Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UBattlePawnExtensionComponent> PawnExtComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Battle|Character", Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UBattleCameraComponent> CameraComponent;
	
};