#include "udp_module.h"
#include <string>


Audp_module::Audp_module()
{
	PrimaryActorTick.bCanEverTick = true;
	Socket = nullptr;
}

Audp_module::~Audp_module()
{
}

void Audp_module::BeginPlay()
{
	Super::BeginPlay();
	UE_LOG(LogTemp, Log, TEXT("Initialized!!"));

	// Ensure the SocketSubsystem is valid
	SocketSubsystem = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM);
	if (!SocketSubsystem)
	{
		UE_LOG(LogTemp, Error, TEXT("Socket Subsystem is null!"));
		return;
	}

	// Setup the local endpoint to bind to any available address and port
	LocalEndpoint = FIPv4Endpoint(FIPv4Address::Any, 0);

	// Setup the remote endpoint with the server's IP and port
	FIPv4Address::Parse(IP, RemoteAddress);
	RemoteEndpoint = FIPv4Endpoint(RemoteAddress, RemotePort);

	// Create the socket
	Socket = FUdpSocketBuilder(TEXT("UDPClientSocket"))
		.AsNonBlocking()
		.AsReusable()
		.BoundToEndpoint(LocalEndpoint)
		.WithReceiveBufferSize(BufferSize)
		.WithSendBufferSize(SendSize)
		.WithBroadcast();

	if (!Socket)
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to create the socket!"));
	}
	else
	{
		UE_LOG(LogTemp, Log, TEXT("Socket created and bound successfully!"));
	}
}

void Audp_module::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
	SocketSubsystem->DestroySocket(Socket);
	Socket = nullptr;
	SocketSubsystem = nullptr;
}

void Audp_module::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	Listen(); // Listen for messages

	//FString t = "test222";
	//sendMessage("t"); // Send Message Test
}

void Audp_module::Listen()
{
	TSharedRef<FInternetAddr> targetAddr = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->CreateInternetAddr();
	uint32 Size;
	if (Socket) {
		while (Socket->HasPendingData(Size))
		{
			//IsRecieve = true;
			uint8* Recv = new uint8[Size];
			int32 BytesRead = 0;

			ReceivedData.SetNumUninitialized(FMath::Min(Size, 65507u));
			Socket->RecvFrom(ReceivedData.GetData(), ReceivedData.Num(), BytesRead, *targetAddr);

			char ansiiData[1024];
			memcpy(ansiiData, ReceivedData.GetData(), BytesRead);
			ansiiData[BytesRead] = 0;

			FString data = ANSI_TO_TCHAR(ansiiData);
			//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, "Message by UDP: " + data);

			FString left;
			FString right;
			TArray<FString> Values;
			data.Split(TEXT(":"), &left, &right);
			right.ParseIntoArray(Values, TEXT(" "));
			if (left == "GYRO") {
				GYRO.X = FCString::Atof(*Values[0]);
				GYRO.Y = FCString::Atof(*Values[1]);
				GYRO.Z = FCString::Atof(*Values[2]);
			}
			else if (left == "ACC") {
				ACC.X = FCString::Atof(*Values[0]);
				ACC.Y = FCString::Atof(*Values[1]);
				ACC.Z = FCString::Atof(*Values[2]);
			}
			else {
				MAG.X = FCString::Atof(*Values[0]);
				MAG.Y = FCString::Atof(*Values[1]);
				MAG.Z = FCString::Atof(*Values[2]);
			}
		}
	}
	else {
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, "Not Connected");
		Destroy();
	}
}

bool Audp_module::sendMessage(FString Message)
{
	if (!Socket) return false;
	int32 BytesSent;

	FTimespan waitTime = FTimespan(10);

	TCHAR* serializedChar = Message.GetCharArray().GetData();
	int32 size = FCString::Strlen(serializedChar);

	bool success = Socket->SendTo((uint8*)TCHAR_TO_UTF8(serializedChar), size, BytesSent, *RemoteEndpoint.ToInternetAddr());
	UE_LOG(LogTemp, Warning, TEXT("Sent message: %s : %s : Address - %s : BytesSent - %d"), *Message, (success ? TEXT("true") : TEXT("false")), *RemoteEndpoint.ToString(), BytesSent);

	if (success && BytesSent > 0) return true;
	else return false;
}

void Audp_module::messageQueue()
{
}