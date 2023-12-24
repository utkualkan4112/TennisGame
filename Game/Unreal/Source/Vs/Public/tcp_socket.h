// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "TcpSocketConnection.h"
#include "tcp_socket.generated.h"


UCLASS()
class VS_API Atcp_socket : public ATcpSocketConnection
{
	GENERATED_BODY()

public:

	FTcpSocketDisconnectDelegate OnDisconnect;
	FTcpSocketConnectDelegate OnConnect;
	FTcpSocketReceivedMessageDelegate OnMessage;

	UFUNCTION()
	void Disconnected(int32 ConnectionID);

	UFUNCTION()
	void Connected(int32 ConnectionID);
	UFUNCTION()
	void GetMessage(int32 ConnectionId, TArray<uint8>& Message);

	void ConnectToServer(int32 &id);

	FVector ACC;

	FRotator GYRO;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
};
