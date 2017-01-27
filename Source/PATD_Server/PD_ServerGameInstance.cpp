// Fill out your copyright notice in the Description page of Project Settings.

#include "PATD_Server.h"
#include "PD_ServerGameInstance.h"
#include "NW_NetWorking/Socket/PD_NW_SocketManager.h"

void UPD_ServerGameInstance::initializeNetworking(){
	socketManager = new PD_NW_SocketManager();
	socketManager->setFTimerManager(&this->GetTimerManager());
	socketManager->init();//Con esto empezaria el timer, quizas no lo queremos llamar aqui o queremos separarlo entre init y start
}

