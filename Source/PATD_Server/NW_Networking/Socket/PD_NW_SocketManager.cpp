// Fill out your copyright notice in the Description page of Project Settings.

#include "PATD_Server.h"
#include "PD_NW_SocketManager.h"

//Includes of forward declaration
#include "PD_NW_Socket.h" 
#include "Networking.h"
#include "NW_NetWorking/PD_NW_ServerActor.h"

#include <string>

PD_NW_SocketManager::PD_NW_SocketManager()
{
	socketArray = TArray<PD_NW_Socket*>();
}

PD_NW_SocketManager::~PD_NW_SocketManager()
{
	//delete socketArray;
}

//Con el client mode lo que hara es no iniciar el listener
void PD_NW_SocketManager::Init(APD_NW_ServerActor* InmyServerActor, int port)
{
	UE_LOG(LogTemp, Warning, TEXT("SocketManager Init() "));
	//Inicializacion actor
	
	SetServerActor(InmyServerActor);
	myServerActor->IniciarTimer();

	//Inicializacion listener?
	InitListener(port);

	
}

void PD_NW_SocketManager::SetServerActor(APD_NW_ServerActor* InmyServerActor)
{
	myServerActor = InmyServerActor;
	myServerActor->SetSocketManager(this);
}

void PD_NW_SocketManager::HandleNewSocketData(TArray<uint8>* data, int socketIndex) {

	UE_LOG(LogTemp, Error, TEXT("HandleNewSocketData (socketIndex: %d)"), socketIndex);

	//Esto no va aqui, esto ya seria llamar al serializer o a los objetos suscritos

	//Create a string from a byte array!
	//
	std::string cstr(reinterpret_cast<const char*>(data->GetData()), data->Num());
	
	FString string= FString(cstr.c_str());

	UE_LOG(LogTemp, Error, TEXT("HandleNewSocketData Read: %s"), *string);

};

void PD_NW_SocketManager::HandleNewListenerConnection(PD_NW_Socket* newSocket) {


	socketArray.Add(newSocket);
	

}
//hay posibilidad de que esta funcion falle? quizas debe devolver void
bool PD_NW_SocketManager::InitListener(int port) {
	
	if (listenerSocket) { //Esto es necesario?
		//cerrar conexion del listener
		//deletear
		delete listenerSocket;
	}
	listenerSocket = new PD_NW_Socket();
	listenerSocket->InitAsListener(port);
	
	return true;

}


void PD_NW_SocketManager::TimerRefreshFunction() {
	//if()
	
	UE_LOG(LogTemp, Error, TEXT("Timer Working "));

	if (listenerSocket) {
		PD_NW_Socket* newSocket = listenerSocket->ReceiveNewConnection();
		if (newSocket) {
			HandleNewListenerConnection(newSocket);
		}
	}

	for (int i = 0; i < socketArray.Num(); i++) {
	//Preguntar si hay data y en caso de haberla llamar a la funcion void socketHasReceivedData(TArray<uint8> data, int socketIndex);
		TArray<uint8>* inData=socketArray[i]->ReceiveData();
		if (inData) {
			HandleNewSocketData(inData, i);
		}

	}
}
