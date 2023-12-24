// Fill out your copyright notice in the Description page of Project Settings.


#include "tcp_socket.h"

void Atcp_socket::Disconnected(int32 ConnectionID)
{
    UE_LOG(LogTemp, Warning, TEXT("Disconnected! Connection ID: %d"), ConnectionID);
}

void Atcp_socket::Connected(int32 ConnectionID)
{
    UE_LOG(LogTemp, Warning, TEXT("Connected! Connection ID: %d"), ConnectionID);
}

void Atcp_socket::GetMessage(int32 ConnectionId, TArray<uint8>& Message)
{
    FString Smessage = Message_ReadString(Message, Message.Num());
    FString left;
    FString right;
    TArray<FString> Values;
    Smessage.Split(TEXT(":"),&left, &right);
    right.ParseIntoArray(Values, TEXT(" "));
    if (left == "GYRO") {
        GYRO.Roll -= FCString::Atof(*Values[0]) + 5;
        GYRO.Pitch += FCString::Atof(*Values[1]) + 5;
        GYRO.Yaw += FCString::Atof(*Values[2]) + 5;
    }
    else {
        ACC.X += FCString::Atof(*Values[0]) + 5;
        ACC.Y += FCString::Atof(*Values[1]) + 5;
        ACC.Z += FCString::Atof(*Values[2]) + 5;
    }
}

void Atcp_socket::ConnectToServer(int32 &id)
{
    Connect("192.168.1.2", 2020, OnDisconnect, OnConnect, OnMessage, id);
}

void Atcp_socket::BeginPlay()
{
    Super::BeginPlay();
    // Bind the delegates to the respective functions
    OnDisconnect.BindUFunction(this, "Disconnected");
    OnConnect.BindUFunction(this, "Connected");
    OnMessage.BindUFunction(this, "GetMessage");

    GYRO = FRotator::ZeroRotator;
    ACC = FVector::ZeroVector;
}
