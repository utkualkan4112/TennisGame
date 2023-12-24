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
	
}

// Called when the game starts or when spawned
void ARacket::BeginPlay()
{
	Super::BeginPlay();
	FActorSpawnParameters parameters;
	
	//TCP = GetWorld()->SpawnActor<Atcp_socket>(Atcp_socket::StaticClass(), parameters);
	//TCP->ConnectToServer(ConnectionID);

	UDP = GetWorld()->SpawnActor<Audp_module>(Audp_module::StaticClass(), parameters);
	if (UDP)
	{
		// Now that the actor is spawned, you can call methods on it
		UDP->sendMessage("Hello");
	}
	else
	{
		// Handle failed spawn
		UE_LOG(LogTemp, Warning, TEXT("Failed to spawn UDP actor"));
	}

}

// Called every frame
void ARacket::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	// Setting location
	//SetActorLocation(UDP->ACC);

	// Setting rotation
	SetActorRotation(UDP->GYRO);
	

}

// Called to bind functionality to input
void ARacket::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}



