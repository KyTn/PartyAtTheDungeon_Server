// Fill out your copyright notice in the Description page of Project Settings.

#include "PATD_Server.h"
#include "PD_ServerGameInstance.h"
#include "NW_NetWorking/PD_NW_ServerActor.h"
#include "NW_NetWorking/Socket/PD_NW_SocketManager.h"



void UPD_ServerGameInstance::Init()
{
	Super::Init();
	UE_LOG(LogTemp, Warning, TEXT("Init GameInstance ~> "));
	//initializeNetworking();
}


void UPD_ServerGameInstance::initializeNetworking()
{
	socketManager = new PD_NW_SocketManager();
	socketManager->SetIsServer(true);

	APD_NW_ServerActor* ServerActorSpawned = (APD_NW_ServerActor*)GetWorld()->SpawnActor(APD_NW_ServerActor::StaticClass());

	//Donde se pone el puerto que es como una constante global?
	socketManager->Init(ServerActorSpawned, "", 8891);//Con esto empezaria el timer, quizas no lo queremos llamar aqui o queremos separarlo entre init y start

}

PD_NW_SocketManager* UPD_ServerGameInstance::GetSocketManager()
{
	return socketManager;
}



