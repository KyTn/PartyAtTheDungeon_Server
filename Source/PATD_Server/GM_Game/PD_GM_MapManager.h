// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

class PD_MG_LogicPosition;
class PD_MG_StaticMap;
class PD_MG_DynamicMap;
class PD_GM_iLogicCharacter;
class PD_GM_GameManager;
/**
 * 
 */
class PATD_SERVER_API PD_GM_MapManager
{
private:
	PD_GM_GameManager* _GAMEMANAGER;


public:
	PD_GM_MapManager();
	~PD_GM_MapManager();

	PD_MG_StaticMap* StaticMapRef;
	PD_MG_DynamicMap* DynamicMapRef;

	//PD_GM_iLogicCharacter* getLogicCharacterAt(PD_MG_LogicPosition* pos);
	
	//Esto iria aqui, o si hacemos otra clase para la visualizacion, en esa clase.
	//FVector getUWorldPosition(PD_MG_LogicPosition* pos);


#pragma region GET INFO OF THE MAP


	// Estas funciones devolver�n un GenericCharacter 
	AActor* getPlayerAt(PD_MG_LogicPosition* logpos);
	AActor* getEnemyAt(PD_MG_LogicPosition* logpos);

	// Esta funcion devolver� un Interactuable
	AActor* getInteractuableAt(PD_MG_LogicPosition* logpos);

	//Dada un LogicPosition, te da la posici�n en el mundo.
	FVector* LogicToWorldPosition(PD_MG_LogicPosition* pos);

	//Dada la posici�n en el mundo, te devuelve la posici�n l�gica que estar�a asignada a posici�n en el mundo, exista o no en el mapa. 
	PD_MG_LogicPosition* WorldToLogicPosition(FVector* pos);


#pragma endregion




};
