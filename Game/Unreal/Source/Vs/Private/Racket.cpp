// Fill out your copyright notice in the Description page of Project Settings.


#include "Racket.h"
#include "Components/StaticMeshComponent.h"
#include "tcp_socket.h"
#include "udp_module.h"



// Sets default values
ARacket::ARacket()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	SetRootComponent(mesh);
	PreviousAccReading = FVector::ZeroVector;
	PreviousGyroReading = FVector::ZeroVector;
	PreviousMagReading = FVector::ZeroVector;
	CurrentVelocity = FVector::ZeroVector;
	CurrentPosition = GetActorLocation();
}

// Called when the game starts or when spawned
void ARacket::BeginPlay()
{
	Super::BeginPlay();

	FActorSpawnParameters parameters;

	// Create and initialize the Madgwick filter
	Madgwick = NewObject<UMadgwick>();  // Assuming UMadgwick is a UObject
	Madgwick->mgos_imu_madgwick_set_params(&Filter, frequency, beta);  // Set the params
	Madgwick->mgos_imu_madgwick_reset(&Filter);  // Reset the filter

	UDP = GetWorld()->SpawnActor<Audp_module>(Audp_module::StaticClass(), parameters);
	if (!UDP)
	{
		// Handle failed spawn
		UE_LOG(LogTemp, Warning, TEXT("Failed to spawn UDP actor"));
	}
	else {
		UDP->sendMessage("Hello");
	}
	// Set up a timer to check for stationarity
	//GetWorldTimerManager().SetTimer(StationaryCheckTimer, this, &ARacket::CheckStationary, 0.01f, true);

}

void ARacket::CheckStationary()
{
	if (IsDeviceStationary()) {
		ApplyZeroUpdates();
	}

}


// Called every frame
void ARacket::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	switch (currentCalibrationState)
	{
	case NotCalibrated:
		StartCalibration();
		currentCalibrationState = Calibrating;
		break;

	case Calibrating:
		if (IsDeviceStationary()) // Implement logic to check if device is stationary
		{
			ApplyZeroUpdates();
			UpdateRollingAverages();
			if (sampleCount >= DesiredSampleCount)
			{
				FinishCalibration();
				currentCalibrationState = Calibrated;
			}
		}
		else
		{
			ResetCalibrationData();
		}
		break;

	case Calibrated:
		
		// Adjust sensor data by subtracting bias
		Accelerometer = UDP->ACC - AccBias;
		Gyrascope = UDP->GYRO - GyroBias;
		Magnetometer = UDP->MAG - MagBias;


		FQuat QuatOrientation = UpdateFilter(Gyrascope, Accelerometer, Magnetometer);

		if (IsDeviceStationary()) {
			ApplyZeroUpdates();
		}
		else {
			FVector CorrectedAcc = TransformAcceleration(Accelerometer, QuatOrientation); // Use orientation to correct acceleration
			CurrentVelocity += CorrectedAcc * DeltaTime;
			const float ScaleFactor = 200.0f;
			CurrentPosition += CurrentVelocity * DeltaTime * ScaleFactor;
		}

		
		SetActorLocation(CurrentPosition);
		break;
	}

	
	

}

// Called to bind functionality to input
void ARacket::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

FQuat ARacket::UpdateFilter(FVector Gyro, FVector Acc, FVector Mag)
{
	float GyroX = Gyro.X;
	float GyroY = Gyro.Y;
	float GyroZ = Gyro.Z;
	float AccX = Acc.X;
	float AccY = Acc.Y;
	float AccZ = Acc.Z;
	float MagX = Mag.X;
	float MagY = Mag.Y;
	float MagZ = Mag.Z;
	// Update the filter with new sensor data
	if (&Filter)
	{
		Madgwick->mgos_imu_madgwick_update(&Filter, GyroX, GyroY, GyroZ, AccX, AccY, AccZ, MagX, MagY, MagZ);

		// Retrieve the updated orientation
		float q0, q1, q2, q3;
		Madgwick->mgos_imu_madgwick_get_quaternion(&Filter, &q0, &q1, &q2, &q3);

		// Convert the quaternion to Unreal's FQuat
		FQuat QuatOrientation(q1, q2, q3, q0);  // Note the order might vary based on the filter's output
		FQuat Rotation = QuatOrientation.Inverse();
		Rotation.X = QuatOrientation.X;
		// Set the actor's orientation using the quaternion
		SetActorRotation(Rotation);
		return Rotation;
	}
	return FQuat();
}

bool ARacket::IsDeviceStationary()
{
	FVector CurrentAcc = UDP->ACC;
	FVector CurrentGyro = UDP->GYRO;
	FVector CurrentMag = UDP->MAG;

	// Calculate the change in readings
	float AccChange = FVector::Dist(PreviousAccReading, CurrentAcc);
	float GyroChange = FVector::Dist(PreviousGyroReading, CurrentGyro);
	float MagChange = FVector::Dist(PreviousMagReading, CurrentMag);

	// Debug messages
	//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Yellow, FString::Printf(TEXT("AccChange: %f, GyroChange: %f, MagChange: %f"), AccChange, GyroChange, MagChange));

	// Update previous readings for the next frame
	PreviousAccReading = CurrentAcc;
	PreviousGyroReading = CurrentGyro;
	PreviousMagReading = CurrentMag;

	// Check if the readings are below the thresholds
	return AccChange < AccThreshold && GyroChange < GyroThreshold && MagChange < MagThreshold;
}

void ARacket::UpdateRollingAverages()
{
	// Update rolling averages for Acc, Gyro, and Mag
	rollingAccAverage = (rollingAccAverage * sampleCount + UDP->ACC) / (sampleCount + 1);
	rollingGyroAverage = (rollingGyroAverage * sampleCount + UDP->GYRO) / (sampleCount + 1);
	rollingMagAverage = (rollingMagAverage * sampleCount + UDP->MAG) / (sampleCount + 1);
	sampleCount++;
}

void ARacket::FinishCalibration()
{
	// Set biases based on rolling averages
	AccBias = rollingAccAverage;
	GyroBias = rollingGyroAverage;
	MagBias = rollingMagAverage;

	// Reset position and velocity after calibration
	CurrentPosition = GetActorLocation();
	CurrentVelocity = FVector::ZeroVector;

	// Provide feedback to the user
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, TEXT("Calibration Complete"));

	currentCalibrationState = Calibrated;
}

void ARacket::ResetCalibrationData()
{
	// Reset rolling averages and sample count
	rollingAccAverage = FVector::ZeroVector;
	rollingGyroAverage = FVector::ZeroVector;
	rollingMagAverage = FVector::ZeroVector;
	sampleCount = 0;
}

void ARacket::StartCalibration()
{
	GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Red, "Stay still PLEASEE!!");
	AccBias = FVector::ZeroVector;
	GyroBias = FVector::ZeroVector;
	MagBias = FVector::ZeroVector;
}

void ARacket::ApplyZeroUpdates()
{
	CurrentVelocity = FVector::ZeroVector;
}

FVector ARacket::TransformAcceleration(FVector Acc, FQuat Orientation)
{
	// Transform the acceleration vector from the device's coordinate system to a fixed reference frame
	return Orientation.RotateVector(Acc);
}


