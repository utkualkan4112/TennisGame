// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
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

	

	UPROPERTY(EditAnywhere)
	UStaticMeshComponent* mesh;

	//UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Network")
	//Atcp_socket* TCP;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Network")
	Audp_module* UDP;

	UPROPERTY(VisibleAnywhere)
	int32 ConnectionID;

	UPROPERTY(VisibleAnywhere)
	FTransform transform;

	UPROPERTY(VisibleAnywhere)
	FVector Vector;

	UPROPERTY(VisibleAnywhere)
	FRotator Rotation;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

};
