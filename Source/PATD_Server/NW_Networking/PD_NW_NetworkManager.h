// Fill out your copyright notice in the Description page of Project Settings.

#include "SR_Serializer/PD_SR_UStruct.h"
#pragma once

//forward declarations
class PD_NW_SocketManager;
class PD_NW_EventManager;
class PD_NW_iEventObserver;
class PD_SR_SerializerManager;
/**
 * 
 */
class PATD_SERVER_API PD_NW_NetworkManager
{
private:
	//PD_NW_CompressionManager* compressionManager;
	PD_NW_SocketManager* socketManager;
	PD_NW_EventManager* eventManager;
	PD_SR_SerializerManager* serializerManager;

	TArray<FStructGenericList>* sendGenericStruct;
	TArray<FStructGenericList>* reciveGenericStruct;

	//Punto de entrada de los eventos del socketManager
	void HandleNewSocketData(TArray<uint8>* data, int socketIndex);
	//Por si lo necesitamos para meter en el array de compressionManager
	//void HangleNewConnection(int playeR); 


public:
	PD_NW_NetworkManager();
	~PD_NW_NetworkManager();
	
	//Getters de Managers internos
	//PD_NW_CompressionManager* GetCompressionManager();
	PD_NW_SocketManager* GetSocketManager();
	PD_NW_EventManager* GetEventManager();
	PD_SR_SerializerManager* GetSerializerManager();

	//Funciones publicas para enviar
	bool SendNow(FStructGeneric* st,int player=-1);
	bool SendNow(int player=-1);
	bool AddToSendList(FStructGeneric* st, int player=-1);

	//Funciones publicas para recibir (Registrar observer);
	bool RegisterObserver(PD_NW_iEventObserver* observer);
	bool UnregisterObserver(PD_NW_iEventObserver* observer);
	
};
