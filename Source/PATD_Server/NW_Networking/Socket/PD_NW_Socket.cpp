// Fill out your copyright notice in the Description page of Project Settings.

#include "PATD_Server.h"
#include "PD_NW_Socket.h"
#include "Networking.h"

//Includes of forward declaration
#include "PD_NW_SocketManager.h" 


PD_NW_Socket::PD_NW_Socket()
{
	socket = NULL;
//	GetWorld()->S
}


PD_NW_Socket::~PD_NW_Socket()
{
	UE_LOG(LogTemp, Error, TEXT("DELETE DE LOS SOCKET "));

	//Esto puede dar error al llamarse alguna vez sin que tenga nada?
	//if (socket) {
	socket->Close();
	delete socket; // con esto se supone que se borra la instancia de la clase (?)
	socket = NULL;
	//}

}


bool PD_NW_Socket::ConnectTo(FString ip, int port) {
	
	FIPv4Address  instanceFIPv4Addres;
	FIPv4Address::Parse(ip, instanceFIPv4Addres);
	
	TSharedRef<FInternetAddr> addr = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->CreateInternetAddr();
	
//	addr->SetIp(instanceFIPv4Addres.Value);

	addr->SetIp(instanceFIPv4Addres.Value);
	addr->SetPort(port);


	bool connected = socket->Connect(*addr);
	return connected;

	/*TSharedRef<FInternetAddr> addr = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->CreateInternetAddr();
	bool validIp;
	addr->SetIp(ip.GetCharArray(), validIp);
	addr->SetPort(port);
	
	*/
	
}

bool PD_NW_Socket::SendData(TArray<uint8>* sendData) {
	int32 bytesReceived;
	
	//Mirar si la el CountBytes funciona adecuadamente o esta metiendo bytes de mas para el array. (este serializando de mas)
	
	bool successful = socket->Send(sendData->GetData(), sendData->Num(), bytesReceived);
	return successful;
}

TArray<uint8>* PD_NW_Socket::ReceiveData() {

	//Ahora mismo, al no tener datos para recibir y el que haya un error se devuelve lo mismo, null.
	// ERROR!
	if (!socket)
	{
			UE_LOG(LogTemp, Error, TEXT(">>> No hay Socket Creado! "));
			return nullptr;
	}

	TArray<uint8>* receivedData = nullptr;

	uint32 Size;
	//El while nos come todos los pendings pero solo se queda con el ultimo. No tiene mucho sentido
	//Lo dejamos en un unico if, y la informacion que pueda seguir habiendo se quedara para el siguiente tick
	if (socket->HasPendingData(Size))
	{
		//Estamos creando los datos nuevos en el HEAP
		receivedData = new TArray<uint8>(); //Aqui creamos reserva de memoria en heap para el array.
		receivedData->Init(0, FMath::Min(Size, 65507u));

		int32 Read = 0;
		socket->Recv(receivedData->GetData(), receivedData->Num(), Read);

	}
	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	// ERROR!
	if (receivedData==nullptr || receivedData->Num()<=0)
	{
	//	UE_LOG(LogTemp, Error, TEXT(">>>> No se han enviado datos ! "));
		return nullptr; //No Data Received
	}
	else {
	//	UE_LOG(LogTemp, Warning, TEXT(">>>> Se van a enviar DATOS :) ! "));
	}
	return receivedData;

}




PD_NW_Socket* PD_NW_Socket::ReceiveNewConnection() {
	

//	UE_LOG(LogTemp, Warning, TEXT("Ha entrado a ReceiveNewConnection ! "));
	//~~~~~~~~~~~~~
	//Ahora mismo, al no tener datos para recibir y el que haya un error se devuelve lo mismo, null.
	// ERROR!
	if (!socket) return nullptr;
	//~~~~~~~~~~~~~

	//Esto se podria usar si la funcion de getAdress() del FSocket no funciona como pensamos.
	/*TSharedRef<FInternetAddr> RemoteAddress = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->CreateInternetAddr();
	//Global cache of current Remote Address
      RemoteAddressForConnection = FIPv4Endpoint(RemoteAddress);
	*/
	
	bool Pending;
	// handle incoming connections
	/*if (socket == NULL) {
		UE_LOG(LogTemp, Error, TEXT("Socket es null en Pending "));

	}
	else {
		UE_LOG(LogTemp, Error, TEXT("SocketNO NULL en Pending "));

	}*/
	if (socket->HasPendingConnection(Pending) && Pending)
	{
		UE_LOG(LogTemp, Warning, TEXT("Ha entrado a HasPendingConnection ! "));
		FSocket* newFSocket;
		PD_NW_Socket* newPD_NW_Socket;
		//En principio no necesitamos guardar la direccion aqui. (Accept permite guardarla)
		newFSocket = socket->Accept( TEXT("Data Socket created at Listener"));
		newPD_NW_Socket = new PD_NW_Socket();
		newPD_NW_Socket->SetFSocket(newFSocket);

		UE_LOG(LogTemp, Warning, TEXT("Ha creado el nuevo socket ! "));

		return newPD_NW_Socket;

	}
	
	return nullptr;
}

void PD_NW_Socket::InitAsListener(int port) {
	FIPv4Endpoint Endpoint(FIPv4Address(127, 0, 0, 1), port);
	this->socket = FTcpSocketBuilder("Listener Socket").AsReusable().BoundToEndpoint(Endpoint).Listening(8);
}

void PD_NW_Socket::InitAsDataSocket() {
	socket = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->CreateSocket(NAME_Stream, TEXT("Data Socket created from init"), false);
}


void PD_NW_Socket::SetFSocket(FSocket* inSocket) {
	socket = inSocket;
}

FSocket* PD_NW_Socket::GetFSocket() {
	return socket;
}