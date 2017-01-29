// Fill out your copyright notice in the Description page of Project Settings.

#include "PATD_Server.h"
#include "PD_ServerGameInstance.h"
#include "NW_NetWorking/PD_NW_ServerActor.h"
#include "NW_NetWorking/Socket/PD_NW_SocketManager.h"



void UPD_ServerGameInstance::Init()
{
	Super::Init();
	UE_LOG(LogTemp, Warning, TEXT("Init GameInstance ~> "));
	InitializeNetworking();
}

void UPD_ServerGameInstance::Shutdown()
{
	Super::Shutdown();

	if (socketManager) {
		delete socketManager;
	}
}

void UPD_ServerGameInstance::InitializeNetworking()
{
	socketManager = new PD_NW_SocketManager();
	socketManager->SetIsServer(true);

	APD_NW_ServerActor* ServerActorSpawned = (APD_NW_ServerActor*)GetWorld()->SpawnActor(APD_NW_ServerActor::StaticClass());

	//Donde se pone el puerto que es como una constante global?
	socketManager->Init(ServerActorSpawned, "", defaultServerPort);//Con esto empezaria el timer, quizas no lo queremos llamar aqui o queremos separarlo entre init y start
	

}

PD_NW_SocketManager* UPD_ServerGameInstance::GetSocketManager()
{
	return socketManager;
}


void UPD_ServerGameInstance::LoadMap()
{
	UGameplayStatics::OpenLevel((UObject*)this, FName(TEXT("Level_Test_Travel_2")));


}

void UPD_ServerGameInstance::InitServerActoWhenLoadMap()
{
	APD_NW_ServerActor* ServerActorSpawned = (APD_NW_ServerActor*)GetWorld()->SpawnActor(APD_NW_ServerActor::StaticClass());

	socketManager->InitServerActor(ServerActorSpawned);
}