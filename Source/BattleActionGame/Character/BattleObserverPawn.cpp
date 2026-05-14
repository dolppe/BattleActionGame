#include "BattleObserverPawn.h"

#include "BattleCharacter.h"
#include "EnhancedInputSubsystems.h"
#include "BattleActionGame/BattleGameplayTags.h"

#include "BattleActionGame/Camera/BattleCameraComponent.h"
#include "BattleActionGame/Input/BattleInputComponent.h"
#include "Net/UnrealNetwork.h"


#include UE_INLINE_GENERATED_CPP_BY_NAME(BattleObserverPawn)

ABattleObserverPawn::ABattleObserverPawn(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;
	
	{
		SetRootComponent(CreateDefaultSubobject<USceneComponent>(TEXT("SceneComponent")));
	}
	
	{
		CameraComponent = CreateDefaultSubobject<UBattleCameraComponent>(TEXT("CameraComponent"));
		CameraComponent->SetRelativeLocation(FVector(-300.0f, 0.0f, 75.0f));
	}
	
	bReplicates = true;
	bOnlyRelevantToOwner = true;
}

void ABattleObserverPawn::SetDefaultData(const UBattlePawnData& PawnData)
{
	if (HasAuthority())
	{
		InputData = PawnData.ObserverInputData;
		InputConfigs = PawnData.ObserverInputConfigs;
		DefaultCameraMode = PawnData.ObserverDefaultCameraMode;		
	}
	
	
}

void ABattleObserverPawn::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);
	
	if (IsLocallyControlled())
	{
		
	}
	
}

void ABattleObserverPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	
	if (IsLocallyControlled())
	{
		InitializePlayerInput(PlayerInputComponent);
		CameraComponent->DetermineCameraModeDelegate.BindUObject(this, &ThisClass::DetermineCameraMode);
	}
}

PRAGMA_DISABLE_OPTIMIZATION

void ABattleObserverPawn::ServerReturnToCharacter_Implementation()
{
	if (ABattleCharacter* OriginCharacter = Cast<ABattleCharacter>(GetInstigator()))
	{
		// if (AController* AIController = OriginCharacter->GetController())
		// {
		// 	AIController->UnPossess();
		// }
		
		AController* PlayerController = GetController();
		
		//PlayerController->UnPossess();
		PlayerController->Possess(OriginCharacter);
	}
	
	
	
}

PRAGMA_ENABLE_OPTIMIZATION

void ABattleObserverPawn::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ABattleObserverPawn, DefaultCameraMode);
	DOREPLIFETIME(ABattleObserverPawn, InputData);
	DOREPLIFETIME(ABattleObserverPawn, InputConfigs);
}

TSubclassOf<UBattleCameraMode> ABattleObserverPawn::DetermineCameraMode() const
{
	return DefaultCameraMode;
}

void ABattleObserverPawn::InitializePlayerInput(UInputComponent* PlayerInputComponent)
{
	const APlayerController* PC = GetController<APlayerController>();
	check(PC);

	const ULocalPlayer* LP = PC->GetLocalPlayer();
	check(LP);

	UEnhancedInputLocalPlayerSubsystem* Subsystem = LP->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>();
	check(Subsystem); 

	// EnhancedInputLocalPlayerSubsystem에 매핑 컨텍스트을 초기화 시켜줌.
	Subsystem->ClearAllMappings();
	
	if (InputData)
	{
		for (const FBattleMappableConfigPair& Pair : InputConfigs)
		{
			if (Pair.bShouldActivateAutomatically)
			{
				FModifyContextOptions Options = {};
				Options.bIgnoreAllPressedKeysUntilRelease = false;

				// 내부적으로 Input Mapping Context를 추가함.
				// 이를 먼저 추가해야 바인딩을 제대로 진행할 수 있음.
				Subsystem->AddPlayerMappableConfig(Pair.Config.LoadSynchronous(), Options);
			}
		}
		if (UBattleInputComponent* BattleIC = CastChecked<UBattleInputComponent>(InputComponent))
		{
			const FBattleGameplayTags& GameplayTags = FBattleGameplayTags::Get();
			BattleIC->BindNativeAction(InputData, GameplayTags.InputTag_Look_Mouse, ETriggerEvent::Triggered, this,&ThisClass::Input_LookMouse, false);
			BattleIC->BindNativeAction(InputData, GameplayTags.InputTag_SpecialKey, ETriggerEvent::Triggered, this,&ThisClass::Input_ReturnToCharacter, false);
			
		}
	}
	
}

void ABattleObserverPawn::Input_LookMouse(const FInputActionValue& InputActionValue)
{

	const FVector2D Value = InputActionValue.Get<FVector2D>();
	if (Value.X != 0.0f)
	{
		// X에는 Yaw값이 있음
		// 카메라에 Yaw 적용.
		this->AddControllerYawInput(Value.X);
	}

	if (Value.Y != 0.0f)
	{
		// Y에는 Pitch 값.
		double AimInversionValue = -Value.Y;
		this->AddControllerPitchInput(AimInversionValue);
	}
}

void ABattleObserverPawn::Input_ReturnToCharacter(const FInputActionValue& InputActionValue)
{
	UE_LOG(LogTemp, Log, TEXT("Returning to Character"));
	ServerReturnToCharacter();
}

void ABattleObserverPawn::OnRep_DefaultCameraMode()
{
}

void ABattleObserverPawn::OnRep_InputData()
{
}

void ABattleObserverPawn::OnRep_InputConfigs()
{
}
