#include "BattleAnimInstance.h"

#include "AbilitySystemGlobals.h"
#include "BattleActionGame/Character/BattleCharacterMovementComponent.h"
#include "GameFramework/Character.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(BattleAnimInstance)

UBattleAnimInstance::UBattleAnimInstance(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	RunThreshold = 3.0f;
	JumpingThreshold = 100.0f;
}

void UBattleAnimInstance::InitializeWithAbilitySystem(UAbilitySystemComponent* ASC)
{
	check(ASC);

	GameplayTagPropertyMap.Initialize(this, ASC);
}

void UBattleAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	if (AActor* OwningActor = GetOwningActor())
	{
		if (UAbilitySystemComponent* ASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(OwningActor))
		{
			InitializeWithAbilitySystem(ASC);
		}
		if (ACharacter* Character = Cast<ACharacter>(OwningActor))
		{
			Owner = Character;
			CharacterMovement = Cast<UBattleCharacterMovementComponent>(Owner->GetCharacterMovement());
		}
	}
}

void UBattleAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	if (CharacterMovement)
	{
		CharacterVelocity = CharacterMovement->Velocity;
		CharacterSpeed = CharacterVelocity.Size2D();
		bIsIdle = CharacterSpeed < RunThreshold;
		bIsFalling = CharacterMovement->IsFalling();
		bIsJumping = bIsFalling & (CharacterVelocity.Z > JumpingThreshold);
	}
	
}
