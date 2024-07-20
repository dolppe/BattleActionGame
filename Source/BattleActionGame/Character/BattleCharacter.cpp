#include "BattleCharacter.h"

#include "BattlePawnExtensionComponent.h"
#include "BattleActionGame/Camera/BattleCameraComponent.h"
#include "Components/CapsuleComponent.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(BattleCharacter)

ABattleCharacter::ABattleCharacter(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = false;
	PrimaryActorTick.bStartWithTickEnabled = false;
	
	UCapsuleComponent* CapsuleComp = GetCapsuleComponent();
	check(CapsuleComp);
	CapsuleComp->InitCapsuleSize(40.0f, 90.0f);
	
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = true;
	bUseControllerRotationRoll = false;


	// PawnExtComponent 생성
	PawnExtComponent = CreateDefaultSubobject<UBattlePawnExtensionComponent>(TEXT("PawnExtensionComponent"));
	{
		
	}
	// CameraComponent 생성
	{
		CameraComponent = CreateDefaultSubobject<UBattleCameraComponent>(TEXT("CameraComponent"));
		CameraComponent->SetRelativeLocation(FVector(-300.0f, 0.0f, 75.0f));
	}
	
}


void ABattleCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	PawnExtComponent->HandleControllerChanged();
}

void ABattleCharacter::UnPossessed()
{
	Super::UnPossessed();

	PawnExtComponent->HandleControllerChanged();
}
