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

	PawnExtComponent->SetupPlayerInputComponent();
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
	UE_LOG(LogBattle, Log, TEXT("ACharacter::LaunchCharacter '%s' (%f,%f,%f)"), *GetName(), LaunchVelocity.X, LaunchVelocity.Y, LaunchVelocity.Z);
	Super::LaunchCharacter(LaunchVelocity, bXYOverride, bZOverride);
}

void ABattleCharacter::StartCriticalHit(FVector ImpactPoint, ABattleCharacterBase* TargetActor)
{
	BeforeRotation = GetViewRotation();
	
	TargetActor->PerformGroggy();
	
	NetStopMotion(0.7f, 0.1f);
	TargetActor->NetStopMotion(0.7f, 0.1f);

	FVector DesiredViewPointLocation;
	FRotator DesiredViewPointRotation;
	
	
	FVector HitDir = (ImpactPoint - GetActorLocation()).GetSafeNormal();
	float ImpactPointLength = HitDir.Length();
	
	//DesiredViewPointLocation = GetActorLocation() + HitDir*(ImpactPointLength*0.5f);
	// DesiredViewPointLocation = GetActorLocation();
	// DesiredViewPointLocation.Z = GetActorLocation().Z +300.f;
	// DesiredViewPointRotation = ((ImpactPoint - DesiredViewPointLocation).GetSafeNormal()).Rotation();
	
	
	FVector SideDir = FVector::CrossProduct(HitDir, FVector::UpVector);
	
	SideDir = SideDir.GetSafeNormal();
	
	if (ImpactPointLength < 500.f)
	{
		ImpactPointLength = 500.f;
	}
	
	FVector DesiredViewPointLocation1 = GetActorLocation() + HitDir*ImpactPointLength*0.7f + SideDir*ImpactPointLength;
	FVector DesiredViewPointLocation2 = GetActorLocation() + HitDir*ImpactPointLength*0.7f - (SideDir*ImpactPointLength);
	
	float Distance1 = (DesiredViewPointLocation1 - TargetActor->GetActorLocation()).SizeSquared();
	float Distance2 = (DesiredViewPointLocation2 - TargetActor->GetActorLocation()).SizeSquared();
	
	DesiredViewPointLocation = (Distance1 > Distance2) ? DesiredViewPointLocation1 : DesiredViewPointLocation2;
	
	DesiredViewPointLocation.Z = GetActorLocation().Z + 300.f;
	
	DesiredViewPointRotation = ((ImpactPoint - DesiredViewPointLocation).GetSafeNormal()).Rotation();
	
	
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

	UE_LOG(LogBattle, Log, TEXT("BeforeRotation: %s"), *BeforeRotation.ToString());
	UE_LOG(LogBattle, Log, TEXT("BeforeControl: %s"), *(GetViewRotation().ToString()));
	GetController()->SetControlRotation(BeforeRotation);
	UE_LOG(LogBattle, Log, TEXT("AfterControl: %s"), *(GetViewRotation().ToString()));
	
	
	if (HeroComponent != nullptr)
	{
		HeroComponent->ClearDesiredCameraMode();
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
}

void ABattleCharacter::HandleImpactDamage(AActor* DamageInstigator, AActor* DamageCauser,
	const FGameplayEffectSpec& DamageEffectSpec, float DamageMagnitude)
{
	BA_DEFAULT_LOG(LogBattle,Log,TEXT("HandleImpactDamage: %f"), DamageMagnitude);
	if (DamageMagnitude >= 50.0f)
	{
		PerformGroggy();
	}
	else if (DamageMagnitude >= 25.f)
	{
		PerformPoiseBreak();
	}
	
}
