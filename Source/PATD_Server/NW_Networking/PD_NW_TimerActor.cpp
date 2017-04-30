// Fill out your copyright notice in the Description page of Project Settings.

#include "PATD_Server.h"
#include "PD_NW_TimerActor.h"
#include "NW_Networking/PD_NW_NetworkManager.h"
//Includes of forward declaration
#include "NW_Networking/Socket/PD_NW_SocketManager.h"

//Includes de prueba



// Sets default values
APD_NW_TimerActor::APD_NW_TimerActor()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void APD_NW_TimerActor::BeginPlay()
{
	Super::BeginPlay();

}

// Called every frame
void APD_NW_TimerActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void APD_NW_TimerActor::InitTimerActor()
{
	GetWorldTimerManager().SetTimer(TimerHandleActor, this, &APD_NW_TimerActor::CheckForReceivedData, 0.01f, true);
	

	//Mandar un ping cada 10 s (tiempo configurable) 
	FTimerHandle handleForPing;
	GetWorldTimerManager().SetTimer(handleForPing, this, &APD_NW_TimerActor::PingFunctionCallbyNetworkManager, 60.00f, true);


}


void APD_NW_TimerActor::CheckForReceivedData()
{
	SocketManager->TimerRefreshFunction();
}


void APD_NW_TimerActor::SetSocketManager(PD_NW_SocketManager* InSocketManager)
{
	SocketManager = InSocketManager;
}

bool APD_NW_TimerActor::isTimerActive() {
	return GetWorldTimerManager().IsTimerActive(TimerHandleActor);
}

void APD_NW_TimerActor::CheckForClientsPong()
{
	FTimerHandle handleForPong;
	GetWorldTimerManager().SetTimer(handleForPong, this, &APD_NW_TimerActor::PongFunctionCallbyNetworkManager, 10.00f, false);

}


void APD_NW_TimerActor::PongFunctionCallbyNetworkManager()
{
	//A los dos segundos se ejecutara esta funcion del NetWorkManager para la comprobacion de los pong
	SocketManager->GetNetworkManager()->CheckForPongAllClients();
}

void APD_NW_TimerActor::PingFunctionCallbyNetworkManager()
{
	SocketManager->GetNetworkManager()->SendBroadCastPingAllClients();
}

//Funciones de prueba




