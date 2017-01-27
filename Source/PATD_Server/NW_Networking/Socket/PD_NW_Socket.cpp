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
	


	//Ahora mismo, al no tener datos para recibir y el que haya un error se devuelve lo mismo, null.
	// ERROR!
	if (!socket) return nullptr;

	TArray<uint8>* receivedData = nullptr;

	uint32 Size;
	//El while nos come todos los pendings pero solo se queda con el ultimo. No tiene mucho sentido
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


