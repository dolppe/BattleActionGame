#pragma once

#include "BattleActionGame/Camera/BattleCameraMode.h"
#include "CameraTests.generated.h"

UCLASS()
class UMockTestCameraMode_A : public UBattleCameraMode
{
	GENERATED_BODY()
public:
	
	UMockTestCameraMode_A(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get())
		: Super(ObjectInitializer)
	{
		BlendTime = 1.f;
		BlendExponent = 1.0f;
		BlendFunction = EBattleCameraModeBlendFunction::Linear;
		
		View.Location = FVector(10.f);
	}
	
protected:
	
	virtual void UpdateView(float DeltaTime) override
	{
		
	}
};

UCLASS()
class UMockTestCameraMode_B : public UBattleCameraMode
{
	GENERATED_BODY()
public:
	
	UMockTestCameraMode_B(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get())
		: Super(ObjectInitializer)
	{
		BlendTime = 1.f;
		BlendExponent = 1.0f;
		BlendFunction = EBattleCameraModeBlendFunction::Linear;
		
		View.Location = FVector(100.f);
	}
	
protected:
	
	virtual void UpdateView(float DeltaTime) override
	{
		
	}
};
