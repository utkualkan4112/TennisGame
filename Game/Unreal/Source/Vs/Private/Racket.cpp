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
	SetPivotOffset(FVector(-6.194269, 256.726768, -87.643808));
	mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	SetRootComponent(mesh);
	
}

// Called when the game starts or when spawned
void ARacket::BeginPlay()
{
	Super::BeginPlay();
	FActorSpawnParameters parameters;
	// Create and initialize the Madgwick filter
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

}


// Called every frame
void ARacket::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	Accelerometer = UDP->ACC; // set the accelerometer from udp connection
	Gyrascope = UDP->GYRO; // set the gyrascope from udp connection
	Magnetometer = FVector::ZeroVector;
	UpdateFilter(Gyrascope, Accelerometer, Magnetometer);

}

// Called to bind functionality to input
void ARacket::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void ARacket::UpdateFilter(FVector Gyro, FVector Acc, FVector Mag)
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

		// Set the actor's orientation using the quaternion
		SetActorRotation(QuatOrientation);
	}
}



