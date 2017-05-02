// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "Structs/PD_ServerEnums.h"
#include "PD_MatchConfig.h"
#include "PD_ServerGameInstance.h"

/**
 * 
 */
//class PD_MatchConfig;


class PATD_SERVER_API PD_MatchConfigManager
{
private:
	PD_MatchConfig* ActualMatchConfig;
	UPD_ServerGameInstance *SGI;

public:
	PD_MatchConfigManager(UPD_ServerGameInstance * sgi);
	~PD_MatchConfigManager();

	// estas funciones modifican los valores de la configuracion, Y LOS PROPAGA A LOS CLIENTES
	void SetAndSend_MapSize(MATCHCONFIG_MAPSIZE newSize) {
		ActualMatchConfig->SetMapSize(newSize);

		// propagar el cambio a los clientes 
		SGI->BroadcastMatchConfigUpdate(1, (int)newSize, "");


	}

	void SetAndSend_Difficulty(MATCHCONFIG_DIFFICULTY newDiff) {
		ActualMatchConfig->SetDifficulty(newDiff);

		// propagar el cambio a los clientes 
		SGI->BroadcastMatchConfigUpdate(2, (int)newDiff, "");
		


	}

	void SetAndSend_MissionType(MATCHCONFIG_MISSIONTYPE newMission) {
		ActualMatchConfig->SetMissionType(newMission);

		// propagar el cambio a los clientes 
		SGI->BroadcastMatchConfigUpdate(0, (int)newMission, "");


	}

	// estas funciones devuelven los valores de la configuracion
	MATCHCONFIG_MAPSIZE Get_MapSize() {
		return ActualMatchConfig->GetMapSize();
	}

	MATCHCONFIG_DIFFICULTY Get_Difficulty() {
		return ActualMatchConfig->GetDifficulty();
	}

	MATCHCONFIG_MISSIONTYPE Get_MissionType() {
		return ActualMatchConfig->GetMissionType();
	}

	// Esta funcion validará la configuración, terminará de rellenar aquellos valores que falten, 
	// y se propagara la informacion de la partida a aquellos jugadores que esten conectados.
	void Config_Ready() {

		SGI->BroadcastMatchConfigFullUpdate();

	}

};
