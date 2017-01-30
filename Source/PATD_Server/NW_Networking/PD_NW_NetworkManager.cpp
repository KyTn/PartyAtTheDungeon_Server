// Fill out your copyright notice in the Description page of Project Settings.

#include "PATD_Server.h"
#include "PD_NW_NetworkManager.h"

//Includes of forward declaration
#include "NW_NetWorking/Socket/PD_NW_SocketManager.h"
#include "NW_NetWorking/EventLayer/PD_NW_EventManager.h"
#include "SR_Serializer/PD_SR_SerializerManager.h"

PD_NW_NetworkManager::PD_NW_NetworkManager()
{
	// compressionManager= new PD_NW_CompressionManager(); //quizas un array de estos??
	 socketManager= new PD_NW_SocketManager();
	 eventManager = new PD_NW_EventManager();
	 serializerManager= new PD_SR_SerializerManager();
}

PD_NW_NetworkManager::~PD_NW_NetworkManager()
{
	delete socketManager;
	delete eventManager;
}

void PD_NW_NetworkManager::HandleNewSocketData(TArray<uint8>* data, int socketIndex){
	bool serializeOk;
	//No necesitamos pasar por el compresor aqui
	
	//Creo que esto deberia devolver puntero, para asi asegurarnos que la lista este en el HEAP
	FStructGenericList structList=serializerManager->DeserializeData(data, serializeOk);
	
	for (int i = 0; i < structList.list.Num(); i++)
	{ //Lo pasamos como referencia para tener puntero
		eventManager->GenerateEvent(&structList.list[i], socketIndex);

	}


}


PD_NW_SocketManager* PD_NW_NetworkManager::GetSocketManager() {
	return socketManager;
}
PD_NW_EventManager* PD_NW_NetworkManager::GetEventManager() {
	return eventManager;
}

PD_SR_SerializerManager* PD_NW_NetworkManager::GetSerializerManager() {
	return serializerManager;
}

//Funciones publicas para enviar
bool PD_NW_NetworkManager::SendNow(FStructGeneric* st,int player) {
	//Para hacer broadcast con -1?
	AddToSendList(st, player);
	SendNow(player);
	return true;
}
bool PD_NW_NetworkManager::SendNow(int player) {
//	bool serializeOk;
	//Para enviar todo lo pendiente con -1?
//	FStructGenericList* sendList;
	//FStructGenericList* = compressionManager.GetSendList(player);
//	socketManager->SendInfoTo(player,serializerManager->SerializeData(*sendList, serializeOk));
	

	return true;
}
bool PD_NW_NetworkManager::AddToSendList(FStructGeneric* st, int player) {

	//compressionManager.AddToSendList(st, player);
	return true;
}

//Funciones publicas para recibir (Registrar observer);
bool PD_NW_NetworkManager::RegisterObserver(PD_NW_iEventObserver* observer) {
	return eventManager->RegisterObserver(observer);
}

bool PD_NW_NetworkManager::UnregisterObserver(PD_NW_iEventObserver* observer) {
	return eventManager->UnregisterObserver(observer);
}