// Fill out your copyright notice in the Description page of Project Settings.

#include "PATD_Server.h"
#include "PD_NW_ServerActor.h"

//Includes of forward declaration
#include "NW_Networking/Socket/PD_NW_SocketManager.h"
#include "PD_ServerGameInstance.h"

//Includes de prueba
#include "SR_Serializer/PD_SR_ListUStructs.h"


// Sets default values
APD_NW_ServerActor::APD_NW_ServerActor()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void APD_NW_ServerActor::BeginPlay()
{
	Super::BeginPlay();

}

// Called every frame
void APD_NW_ServerActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void APD_NW_ServerActor::InitTimerActor()
{
	GetWorldTimerManager().SetTimer(TimerHandleActor, this, &APD_NW_ServerActor::CheckForReceivedData, 0.01f, true);
	
	//Timers de prueba
	GetWorldTimerManager().SetTimer(TimerHandleActor2, this, &APD_NW_ServerActor::ChangeLevelMap, 40.0f, false);
	GetWorldTimerManager().SetTimer(TimerHandleActor3, this, &APD_NW_ServerActor::SendPruebaSockets, 5.0f, true);
}


void APD_NW_ServerActor::CheckForReceivedData()
{
	SocketManager->TimerRefreshFunction();
}


void APD_NW_ServerActor::SetSocketManager(PD_NW_SocketManager* InSocketManager)
{
	SocketManager = InSocketManager;
}

bool APD_NW_ServerActor::isTimerActive() {
	return GetWorldTimerManager().IsTimerActive(TimerHandleActor);
}





//Funciones de prueba

void APD_NW_ServerActor::ChangeLevelMap()
{
	UPD_ServerGameInstance* SGI = Cast<UPD_ServerGameInstance>(GetGameInstance());
	if (SGI)
	{
		SGI->LoadMap();
	}
}


void APD_NW_ServerActor::SendPruebaSockets()
{
	//PRUEBA


	/*
	UStruct* MyStruct = FStructMap::StaticStruct();
	FStructMap pruebaStruct;
	//pruebaStruct.arrayPruebaStrings
	
	UE_LOG(LogTemp, Warning, TEXT("Primera pos array a enviar:%s"),*pruebaStruct.arrayPruebaStrings[0]);

	TArray<uint8>* Storage = new TArray<uint8>();
	FMemoryWriter ArWriter(*Storage);
	MyStruct->SerializeBin(ArWriter, &pruebaStruct);


	bool successful = SocketManager->SendInfoTo(0, Storage);
	*/
/*
	UStruct* MyStruct = FpruebaUStruct::StaticStruct();

	FpruebaUStruct pruebaStruct;
	pruebaStruct = FpruebaUStruct();

	//GEngine->AddOnScreenDebugMessage(-1, 60.f, FColor::Blue, FString::Printf(TEXT("Enviando a cliente 0")));
	UE_LOG(LogTemp, Warning, TEXT("Enviando a cliente 0"));

	UE_LOG(LogTemp, Warning, TEXT("%s"), *SocketManager->StateString());

	pruebaStruct.stringPrueba = "Texto de prueba en server enviado a cliente 0";



	TArray<uint8>* Storage = new TArray<uint8>();
	FMemoryWriter ArWriter(*Storage);
	MyStruct->SerializeBin(ArWriter, &pruebaStruct);


	bool successful = SocketManager->SendInfoTo(0, Storage);






	UStruct* MyStruct2 = FpruebaUStruct::StaticStruct();

	FpruebaUStruct pruebaStruct2;
	pruebaStruct2 = FpruebaUStruct();

	UE_LOG(LogTemp, Warning, TEXT("Enviando a cliente 1"));

	UE_LOG(LogTemp, Warning, TEXT("%s"), *SocketManager->StateString());

	
	pruebaStruct2.stringPrueba = "Texto de prueba en server enviado a cliente 1";



	TArray<uint8>* Storage2 = new TArray<uint8>();
	FMemoryWriter ArWriter2(*Storage2);
	MyStruct2->SerializeBin(ArWriter2, &pruebaStruct2);


	 successful = SocketManager->SendInfoTo(1, Storage2);
	 */
}