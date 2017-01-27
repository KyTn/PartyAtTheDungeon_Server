// Fill out your copyright notice in the Description page of Project Settings.

#include "PATD_Server.h"
#include "PD_NW_Socket.h"
#include "Networking.h"

//Includes of forward declaration
#include "PD_NW_SocketManager.h" 


PD_NW_Socket::PD_NW_Socket()
{
//	GetWorld()->S
}

PD_NW_Socket::PD_NW_Socket(FSocket* socketIn)
{
	this->socket = socketIn;
}

PD_NW_Socket::~PD_NW_Socket()
{
}




TArray<uint8>* PD_NW_Socket::receiveData() {
	//Aqui deberia ir la funcion que hiciese lo necesario cuando hay un envio pendiente. 


	//Ahora mismo, al no tener datos para recibir y el que haya un error se devuelve lo mismo, null.
	// ERROR!
	if (!socket) return nullptr;

	TArray<uint8>* receivedData = nullptr;

	uint32 Size;
	while (socket->HasPendingData(Size))
	{
		receivedData->Init(0, FMath::Min(Size, 65507u));

		int32 Read = 0;
		socket->Recv(receivedData->GetData(), receivedData->Num(), Read);

	}
	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	// ERROR!
	if (receivedData->Num() <= 0)return nullptr; //No Data Received

	
	return receivedData;



}


