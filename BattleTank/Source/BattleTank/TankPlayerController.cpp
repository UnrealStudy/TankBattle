// Fill out your copyright notice in the Description page of Project Settings.

#include "TankPlayerController.h"

void ATankPlayerController::BeginPlay()
{
	Super::BeginPlay();

	auto ControlledTank = GetControlledTank();
	
	if (!ControlledTank)
	{ 
		UE_LOG(LogTemp, Warning, TEXT("Player Controller did not possess a tank"))
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Player Controller possessed: %s"), *(ControlledTank->GetName()))
	}

}

void ATankPlayerController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// Calculates the relative position of crosshair
	GetViewportSize(ViewportSizeX, ViewportSizeY);
	ScreenLocation.X = ViewportSizeX * CrosshairXLocation;
	ScreenLocation.Y = ViewportSizeY * CrosshairYLocation;

	AimTowardsCrosshair();
}

ATank* ATankPlayerController::GetControlledTank() const
{
	return Cast<ATank>(GetPawn());
}

void ATankPlayerController::AimTowardsCrosshair()
{
	FVector HitLocation;

	if (!GetControlledTank()) { return;  }
	if (GetSightRayHitLocation(HitLocation))
	{
		GetControlledTank()->AimAt(HitLocation);
	}
}

// Get world location of line trace through crosshair, if it intersects with landscape
bool ATankPlayerController::GetSightRayHitLocation(FVector &HitLocation) const
{
	FVector LookDirection;
	//UE_LOG(LogTemp, Warning, TEXT("Crosshair pos: %s"), *LookDirection.ToString())
	
	if (GetLookDirection(ScreenLocation, LookDirection))
	{
		GetLookVectorHitLocation(LookDirection, HitLocation);
	}
	
	return true;
}

bool ATankPlayerController::GetLookDirection(FVector2D ScreenLocation, FVector &LookDirection) const
{
	FVector CameraWorldLocation; // to be discarded

	return DeprojectScreenPositionToWorld(
		ScreenLocation.X,
		ScreenLocation.Y, 
		CameraWorldLocation,
		LookDirection
	);
}

bool ATankPlayerController::GetLookVectorHitLocation(FVector LookDirection, FVector &HitLocation) const
{
	FHitResult OutHit;
	FVector LineTraceStart = PlayerCameraManager->GetCameraLocation();
	FVector LineTraceEnd = LineTraceStart + LookDirection * LineTraceRange;
	bool isSuccess = false;

	isSuccess =  GetWorld()->LineTraceSingleByChannel(
		OutHit,
		LineTraceStart,
		LineTraceEnd,
		ECollisionChannel(ECC_Visibility)
	);
	
	if (isSuccess)
	{
		HitLocation = OutHit.Location;
	}
	else 
	{
		HitLocation = FVector(0.f);
	}
	return isSuccess;
}
