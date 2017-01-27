// Fill out your copyright notice in the Description page of Project Settings.

#include "PATD_Server.h"
#include "PD_NW_SocketManager.h"

//Includes of forward declaration
#include "PD_NW_Socket.h" 
#include "Networking.h"


PD_NW_SocketManager::PD_NW_SocketManager()
{
	socketArray = TArray<PD_NW_Socket*>();
}

PD_NW_SocketManager::~PD_NW_SocketManager()
{
	//delete socketArray;
}


void PD_NW_SocketManager::socketHasReceivedData(TArray<uint8> data, int socketIndex) {



	//No necesitamos esto porque la funcion del timer ya tendra un bucle
	/*//Busqueda del socket que lo envia para poder guardar la variable int socketIndex .
	int index = socketArray->IndexOfByKey(socket) != -1; //-1 == INDEX_NONE
	if (index != -1) {
		receivedDataAtSocket(data, index);
	}
	else {
		//ERROR!
	}*/

};

void PD_NW_SocketManager::listenerNewConexion(PD_NW_Socket* newSocket) {

	//TO-DO Aqui habria que hacer el control de maximas conexiones, y en caso de no aceptar mas, notificarlo a la conexion con el listener, o cerrar el newSocket.

	//Se deberia aceptar la conexion y despues enviar el mensaje a los objetos registrados? o al reves?
	//O separar esto en dos pasos?
	socketArray.Add(newSocket);


}

bool PD_NW_SocketManager::initListener(int port) {
	
	//FIPv4Endpoint Endpoint(FIPv4Address(127,0,0,0), port);
	//PD_NW_Socket* ListenSocket = FTcpSocketBuilder("prueba").AsReusable().BoundToEndpoint(Endpoint).Listening(8);
	PD_NW_Socket ListenSocket;
	ListenSocket.listenerSocket(port);
	listenerNewConexion(&ListenSocket);
	return true;
}


void PD_NW_SocketManager::timerRefreshFunction() {
	for (int i = 0; i < socketArray.Num(); i++) {
		//Preguntar si hay data y en caso de haberla llamar a la funcion void socketHasReceivedData(TArray<uint8> data, int socketIndex);
		socketArray[i]->receiveData();
	}
}
