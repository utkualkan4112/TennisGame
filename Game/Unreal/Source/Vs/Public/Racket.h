// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "Madgwick.h"
#include "Racket.generated.h"


class UStaticMeshComponent;
class Atcp_socket;
class Audp_module;


UCLASS()
class VS_API ARacket : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	ARacket();

	UPROPERTY()
	FMadgwickFilter Filter;  // Pointer to the filter

	UPROPERTY()
	UMadgwick* Madgwick;  // Pointer to the Unreal Engine wrapper of the filter

	UPROPERTY(EditAnywhere)
	float frequency = 40.0f;  // 40 Hz as calculated

	UPROPERTY(EditAnywhere)
	float beta = 0.1f;  // Example beta, may need tuning
	

	UPROPERTY(EditAnywhere)
	UStaticMeshComponent* mesh;

	//UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Network")
	//Atcp_socket* TCP;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Network")
	Audp_module* UDP;

	UPROPERTY(VisibleAnywhere)
	int32 ConnectionID;

	UPROPERTY(VisibleAnywhere)
	FVector Accelerometer;

	UPROPERTY(VisibleAnywhere)
	FVector Gyrascope;

	UPROPERTY(VisibleAnywhere)
	FVector Magnetometer;

	FRotator Orientation;



protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;


public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	void UpdateFilter(FVector Gyro, FVector Acc, FVector Mag);

};
