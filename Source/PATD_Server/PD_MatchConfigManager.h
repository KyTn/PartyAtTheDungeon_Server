// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "Structs/PD_ServerEnums.h"
/**
 * 
 */
class PD_MatchConfig;


class PATD_SERVER_API PD_MatchConfigManager
{
private:
	PD_MatchConfig* ActualMatchConfig;

public:
	PD_MatchConfigManager();
	~PD_MatchConfigManager();

	// estas funciones modifican los valores de la configuracion, PERO NO LOS PROPAGA A LOS CLIENTES
	void Set_MapSize(MATCHCONFIG_MAPSIZE newSize) {
		ActualMatchConfig->SetMapSize(newSize);

		// Llamada a la UI


	}

	void Set_Difficulty(MATCHCONFIG_DIFFICULTY newDiff) {
		ActualMatchConfig->SetDifficulty(newDiff);

		// Llamada a la UI


	}

	void Set_MissionType(MATCHCONFIG_MISSIONTYPE newMission) {
		ActualMatchConfig->SetMissionType(newMission);

		// Llamada a la UI


	}

	// Esta funcion validará la configuración, terminará de rellenar aquellos valores que falten, 
	// y se propagara la informacion de la partida a aquellos jugadores que esten conectados.
	void Config_Ready();

};
