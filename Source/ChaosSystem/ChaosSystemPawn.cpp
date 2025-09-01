// Copyright Epic Games, Inc. All Rights Reserved.

#include "ChaosSystemPawn.h"
#include "ChaosSystemWheelFront.h"
#include "ChaosSystemWheelRear.h"
#include "Components/SkeletalMeshComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"
#include "ChaosWheeledVehicleMovementComponent.h"
#include "ChaosSystem.h"

#define LOCTEXT_NAMESPACE "VehiclePawn"

AChaosSystemPawn::AChaosSystemPawn()
{
	// construct the front camera boom
	FrontSpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("Front Spring Arm"));
	FrontSpringArm->SetupAttachment(GetMesh());
	FrontSpringArm->TargetArmLength = 0.0f;
	FrontSpringArm->bDoCollisionTest = false;
	FrontSpringArm->bEnableCameraRotationLag = true;
	FrontSpringArm->CameraRotationLagSpeed = 15.0f;
	FrontSpringArm->SetRelativeLocation(FVector(30.0f, 0.0f, 120.0f));

	FrontCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("Front Camera"));
	FrontCamera->SetupAttachment(FrontSpringArm);
	FrontCamera->bAutoActivate = false;

	// construct the back camera boom
	BackSpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("Back Spring Arm"));
	BackSpringArm->SetupAttachment(GetMesh());
	BackSpringArm->TargetArmLength = 650.0f;
	BackSpringArm->SocketOffset.Z = 150.0f;
	BackSpringArm->bDoCollisionTest = false;
	BackSpringArm->bInheritPitch = false;
	BackSpringArm->bInheritRoll = false;
	BackSpringArm->bEnableCameraRotationLag = true;
	BackSpringArm->CameraRotationLagSpeed = 2.0f;
	BackSpringArm->CameraLagMaxDistance = 50.0f;

	BackCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("Back Camera"));
	BackCamera->SetupAttachment(BackSpringArm);

	// Configure the car mesh
	GetMesh()->SetSimulatePhysics(true);
	GetMesh()->SetCollisionProfileName(FName("Vehicle"));

	// get the Chaos Wheeled movement component
	ChaosVehicleMovement = CastChecked<UChaosWheeledVehicleMovementComponent>(GetVehicleMovement());

}

void AChaosSystemPawn::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		// steering 
		EnhancedInputComponent->BindAction(SteeringAction, ETriggerEvent::Triggered, this, &AChaosSystemPawn::Steering);
		EnhancedInputComponent->BindAction(SteeringAction, ETriggerEvent::Completed, this, &AChaosSystemPawn::Steering);

		// throttle 
		EnhancedInputComponent->BindAction(ThrottleAction, ETriggerEvent::Triggered, this, &AChaosSystemPawn::Throttle);
		EnhancedInputComponent->BindAction(ThrottleAction, ETriggerEvent::Completed, this, &AChaosSystemPawn::Throttle);

		// break 
		EnhancedInputComponent->BindAction(BrakeAction, ETriggerEvent::Triggered, this, &AChaosSystemPawn::Brake);
		EnhancedInputComponent->BindAction(BrakeAction, ETriggerEvent::Started, this, &AChaosSystemPawn::StartBrake);
		EnhancedInputComponent->BindAction(BrakeAction, ETriggerEvent::Completed, this, &AChaosSystemPawn::StopBrake);

		// handbrake 
		EnhancedInputComponent->BindAction(HandbrakeAction, ETriggerEvent::Started, this, &AChaosSystemPawn::StartHandbrake);
		EnhancedInputComponent->BindAction(HandbrakeAction, ETriggerEvent::Completed, this, &AChaosSystemPawn::StopHandbrake);

		// look around 
		EnhancedInputComponent->BindAction(LookAroundAction, ETriggerEvent::Triggered, this, &AChaosSystemPawn::LookAround);

		// toggle camera 
		EnhancedInputComponent->BindAction(ToggleCameraAction, ETriggerEvent::Triggered, this, &AChaosSystemPawn::ToggleCamera);

		// reset the vehicle 
		EnhancedInputComponent->BindAction(ResetVehicleAction, ETriggerEvent::Triggered, this, &AChaosSystemPawn::ResetVehicle);
	}
	else
	{
		UE_LOG(LogChaosSystem, Error, TEXT("'%s' Failed to find an Enhanced Input component! This template is built to use the Enhanced Input system. If you intend to use the legacy system, then you will need to update this C++ file."), *GetNameSafe(this));
	}
}

void AChaosSystemPawn::Tick(float Delta)
{
	Super::Tick(Delta);

	// add some angular damping if the vehicle is in midair
	bool bMovingOnGround = ChaosVehicleMovement->IsMovingOnGround();
	GetMesh()->SetAngularDamping(bMovingOnGround ? 0.0f : 3.0f);

	// realign the camera yaw to face front
	float CameraYaw = BackSpringArm->GetRelativeRotation().Yaw;
	CameraYaw = FMath::FInterpTo(CameraYaw, 0.0f, Delta, 1.0f);

	BackSpringArm->SetRelativeRotation(FRotator(0.0f, CameraYaw, 0.0f));
}

void AChaosSystemPawn::Steering(const FInputActionValue& Value)
{
	// route the input
	DoSteering(Value.Get<float>());
}

void AChaosSystemPawn::Throttle(const FInputActionValue& Value)
{
	// route the input
	DoThrottle(Value.Get<float>());
}

void AChaosSystemPawn::Brake(const FInputActionValue& Value)
{
	// route the input
	DoBrake(Value.Get<float>());
}

void AChaosSystemPawn::StartBrake(const FInputActionValue& Value)
{
	// route the input
	DoBrakeStart();
}

void AChaosSystemPawn::StopBrake(const FInputActionValue& Value)
{
	// route the input
	DoBrakeStop();
}

void AChaosSystemPawn::StartHandbrake(const FInputActionValue& Value)
{
	// route the input
	DoHandbrakeStart();
}

void AChaosSystemPawn::StopHandbrake(const FInputActionValue& Value)
{
	// route the input
	DoHandbrakeStop();
}

void AChaosSystemPawn::LookAround(const FInputActionValue& Value)
{
	// route the input
	DoLookAround(Value.Get<float>());
}

void AChaosSystemPawn::ToggleCamera(const FInputActionValue& Value)
{
	// route the input
	DoToggleCamera();
}

void AChaosSystemPawn::ResetVehicle(const FInputActionValue& Value)
{
	// route the input
	DoResetVehicle();
}

void AChaosSystemPawn::DoSteering(float SteeringValue)
{
	// add the input
	ChaosVehicleMovement->SetSteeringInput(SteeringValue);
}

void AChaosSystemPawn::DoThrottle(float ThrottleValue)
{
	// add the input
	ChaosVehicleMovement->SetThrottleInput(ThrottleValue);

	// reset the brake input
	ChaosVehicleMovement->SetBrakeInput(0.0f);
}

void AChaosSystemPawn::DoBrake(float BrakeValue)
{
	// add the input
	ChaosVehicleMovement->SetBrakeInput(BrakeValue);

	// reset the throttle input
	ChaosVehicleMovement->SetThrottleInput(0.0f);
}

void AChaosSystemPawn::DoBrakeStart()
{
	// call the Blueprint hook for the brake lights
	BrakeLights(true);
}

void AChaosSystemPawn::DoBrakeStop()
{
	// call the Blueprint hook for the brake lights
	BrakeLights(false);

	// reset brake input to zero
	ChaosVehicleMovement->SetBrakeInput(0.0f);
}

void AChaosSystemPawn::DoHandbrakeStart()
{
	// add the input
	ChaosVehicleMovement->SetHandbrakeInput(true);

	// call the Blueprint hook for the break lights
	BrakeLights(true);
}

void AChaosSystemPawn::DoHandbrakeStop()
{
	// add the input
	ChaosVehicleMovement->SetHandbrakeInput(false);

	// call the Blueprint hook for the break lights
	BrakeLights(false);
}

void AChaosSystemPawn::DoLookAround(float YawDelta)
{
	// rotate the spring arm
	BackSpringArm->AddLocalRotation(FRotator(0.0f, YawDelta, 0.0f));
}

void AChaosSystemPawn::DoToggleCamera()
{
	// toggle the active camera flag
	bFrontCameraActive = !bFrontCameraActive;

	FrontCamera->SetActive(bFrontCameraActive);
	BackCamera->SetActive(!bFrontCameraActive);
}

void AChaosSystemPawn::DoResetVehicle()
{
	// reset to a location slightly above our current one
	FVector ResetLocation = GetActorLocation() + FVector(0.0f, 0.0f, 50.0f);

	// reset to our yaw. Ignore pitch and roll
	FRotator ResetRotation = GetActorRotation();
	ResetRotation.Pitch = 0.0f;
	ResetRotation.Roll = 0.0f;

	// teleport the actor to the reset spot and reset physics
	SetActorTransform(FTransform(ResetRotation, ResetLocation, FVector::OneVector), false, nullptr, ETeleportType::TeleportPhysics);

	GetMesh()->SetPhysicsAngularVelocityInDegrees(FVector::ZeroVector);
	GetMesh()->SetPhysicsLinearVelocity(FVector::ZeroVector);
}

#undef LOCTEXT_NAMESPACE