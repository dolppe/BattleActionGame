#include "BattleCharacter.h"

#include "BattleCharacterMovementComponent.h"
#include "BattleHealthComponent.h"
#include "BattleHeroComponent.h"
#include "BattlePawnExtensionComponent.h"
#include "BattleActionGame/BattleLogChannels.h"
#include "BattleActionGame/AbilitySystem/BattleAbilitySystemComponent.h"
#include "BattleActionGame/AbilitySystem/Attributes/BattleCombatSet.h"
#include "BattleActionGame/Camera/BattleCameraComponent.h"
#include "BattleActionGame/Camera/BattleCameraMode.h"
#include "BattleActionGame/Camera/BattleCameraMode_DesiredViewPoint.h"
#include "Components/CapsuleComponent.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(BattleCharacter)

ABattleCharacter::ABattleCharacter(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer.SetDefaultSubobjectClass<UBattleCharacterMovementComponent>(ACharacter::CharacterMovementComponentName))
{
	PrimaryActorTick.bCanEverTick = false;
	PrimaryActorTick.bStartWithTickEnabled = false;

	NetCullDistanceSquared = 900000000.0f;
	
	UCapsuleComponent* CapsuleComp = GetCapsuleComponent();
	check(CapsuleComp);
	CapsuleComp->InitCapsuleSize(40.0f, 90.0f);
	
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;


	// PawnExtComponent 생성
	PawnExtComponent = CreateDefaultSubobject<UBattlePawnExtensionComponent>(TEXT("PawnExtensionComponent"));
	{
		PawnExtComponent->OnAbilitySystemInitialized_RegisterAndCall(FSimpleMulticastDelegate::FDelegate::CreateUObject(this, &ThisClass::OnAbilitySystemInitialized));
		PawnExtComponent->OnAbilitySystemUninitialized_Register(FSimpleMulticastDelegate::FDelegate::CreateUObject(this, &ThisClass::OnAbilitySystemUninitialized));
	}
	// CameraComponent 생성
	{
		CameraComponent = CreateDefaultSubobject<UBattleCameraComponent>(TEXT("CameraComponent"));
		CameraComponent->SetRelativeLocation(FVector(-300.0f, 0.0f, 75.0f));
	}
	{
		HealthComponent = CreateDefaultSubobject<UBattleHealthComponent>(TEXT("HealthComponent"));
		HealthComponent->OnDeathStarted.AddDynamic(this, &ThisClass::OnDeathStarted);
		HealthComponent->OnDeathFinished.AddDynamic(this, &ThisClass::OnDeathFinished);
	}
}

UBattleAbilitySystemComponent* ABattleCharacter::GetBattleAbilitySystemComponent() const
{
	return Cast<UBattleAbilitySystemComponent>(GetAbilitySystemComponent());
}

UAbilitySystemComponent* ABattleCharacter::GetAbilitySystemComponent() const
{
	return PawnExtComponent->GetAbilitySystemComponent();
}

UBattleHealthComponent* ABattleCharacter::GetHealthComponent() const
{
	return HealthComponent;
}

UBattleHeroComponent* ABattleCharacter::GetHeroComponent()
{
	return Cast<UBattleHeroComponent>(GetComponentByClass(UBattleHeroComponent::StaticClass()));
}

void ABattleCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (PawnExtComponent)
	{
		PawnExtComponent->SetupPlayerInputComponent();
	}
	if (GetHeroComponent())
	{
		GetHeroComponent()->InitilizePlayerInput(PlayerInputComponent);
	}
}

void ABattleCharacter::OnDeathStarted(AActor* OwningActor)
{
	DisableMovementAndCollision();
}

void ABattleCharacter::OnDeathFinished(AActor* OwningActor)
{
	GetWorld()->GetTimerManager().SetTimerForNextTick(this, &ThisClass::DestroyDueToDeath);
}

void ABattleCharacter::DisableMovementAndCollision()
{
	if (Controller)
	{
		Controller->SetIgnoreMoveInput(true);
	}

	UCapsuleComponent* CapsuleComp = GetCapsuleComponent();
	check(CapsuleComp);
	CapsuleComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	CapsuleComp->SetCollisionResponseToAllChannels(ECR_Ignore);

	UCharacterMovementComponent* MovementComponent = GetCharacterMovement();
	MovementComponent->StopMovementImmediately();
	MovementComponent->DisableMovement();
}

void ABattleCharacter::DestroyDueToDeath()
{
	K2_OnDeathFinished();

	UninitAndDestroy();
}

void ABattleCharacter::UninitAndDestroy()
{
	if (GetLocalRole() == ROLE_Authority)
	{
		DetachFromControllerPendingDestroy();
		SetLifeSpan(0.1f);
	}

	if (UBattleAbilitySystemComponent* BattleASC = GetBattleAbilitySystemComponent())
	{
		if (BattleASC->GetAvatarActor() == this)
		{
			PawnExtComponent->UnInitializeAbilitySystem();
		}
	}

	SetActorHiddenInGame(true);
}

void ABattleCharacter::Reset()
{
	DisableMovementAndCollision();

	K2_OnReset();

	UninitAndDestroy();
}

void ABattleCharacter::LaunchCharacter(FVector LaunchVelocity, bool bXYOverride, bool bZOverride)
{
	Super::LaunchCharacter(LaunchVelocity, bXYOverride, bZOverride);
}

void ABattleCharacter::StartCriticalHit(FVector ImpactPoint, ABattleCharacterBase* TargetActor)
{
	BeforeRotation = GetViewRotation();
	
	TargetActor->PerformGroggy();
	
	NetStopMotion(0.7f, 0.01f);
	TargetActor->NetStopMotion(0.7f, 0.01f);
	
	FVector HitDir = (ImpactPoint - GetActorLocation()).GetSafeNormal();
	float ImpactPointLength = FVector::Dist2D(ImpactPoint, GetActorLocation());
	
	FVector SideDir = FVector::CrossProduct(HitDir, FVector::UpVector);
	SideDir = SideDir.GetSafeNormal();
	
	if (ImpactPointLength < 500.f)
	{
		ImpactPointLength = 500.f;
	}
	
	FVector DesiredViewPointLocation1 = GetActorLocation() + HitDir*ImpactPointLength*0.7f + SideDir*200;
	FVector DesiredViewPointLocation2 = GetActorLocation() + HitDir*ImpactPointLength*0.7f - (SideDir*200);
	
	float Distance1 = (DesiredViewPointLocation1 - TargetActor->GetActorLocation()).SizeSquared();
	float Distance2 = (DesiredViewPointLocation2 - TargetActor->GetActorLocation()).SizeSquared();
	
	FVector DesiredViewPointLocation = (Distance1 > Distance2) ? DesiredViewPointLocation1 : DesiredViewPointLocation2;
	DesiredViewPointLocation.Z = GetActorLocation().Z + 300.f;
	FRotator DesiredViewPointRotation = ((ImpactPoint - DesiredViewPointLocation).GetSafeNormal()).Rotation();
	
	
	UBattleHeroComponent* HeroComponent = GetHeroComponent();
	if (HeroComponent != nullptr)
	{
		HeroComponent->SetDesiredCameraMode(CriticalCameraMode, DesiredViewPointLocation, DesiredViewPointRotation);
	}

	GetWorld()->GetTimerManager().SetTimer(CriticalHandle, this, &ThisClass::EndCriticalHit, 0.7f, false);
}

void ABattleCharacter::EndCriticalHit()
{
	UBattleHeroComponent* HeroComponent = GetHeroComponent();
	
	NetSetControlRotation(BeforeRotation);
	
	if (HeroComponent != nullptr)
	{
		HeroComponent->ClearDesiredCameraMode();
	}


}

void ABattleCharacter::PawnClientRestart()
{
	Super::PawnClientRestart();
	
	if (IsLocallyControlled() && InputComponent)
	{
		if (UBattleHeroComponent* HeroComponent = GetHeroComponent())
		{
			HeroComponent->InitilizePlayerInput(InputComponent);
		}
	}
}

void ABattleCharacter::OnAbilitySystemInitialized()
{
	UBattleAbilitySystemComponent* ASC = GetBattleAbilitySystemComponent();
	check(ASC);

	HealthComponent->InitializeWithAbilitySystem(ASC);

	if (const UBattleCombatSet* CombatSet = ASC->GetSet<UBattleCombatSet>())
	{
		CombatSet->OnReceivedImpactDamage.AddUObject(this, &ThisClass::HandleImpactDamage);
	}
}

void ABattleCharacter::OnAbilitySystemUninitialized()
{
	HealthComponent->UninitializeFromAbilitySystem();
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
	GetHeroComponent()->OnUnpossessed();
}

void ABattleCharacter::HandleImpactDamage(AActor* DamageInstigator, AActor* DamageCauser,
	const FGameplayEffectSpec& DamageEffectSpec, float DamageMagnitude)
{
	if (DamageMagnitude >= GroggyThreshold)
	{
		PerformGroggy();
	}
	else if (DamageMagnitude >= PoiseBreakThreshold)
	{
		PerformPoiseBreak();
	}
	
}
