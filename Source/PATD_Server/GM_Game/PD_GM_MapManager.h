// Fill out your copyright notice in the Description page of Project Settings.

#pragma once


// FORWARD DECLARATIONS
class PD_MG_LogicPosition;
class PD_MG_StaticMap;
class PD_MG_DynamicMap;
class PD_GM_GameManager;
class APD_PLY_GenericCharacter;





class PATD_SERVER_API PD_GM_MapManager
{
private:
	PD_GM_GameManager* _GAMEMANAGER;
	// Dada una posici�n l�gica, devuelve el GenericCharacter que est� en esa posici�n, 0 si no hay ninguno. 
	bool getGenericCharacterAt(PD_MG_LogicPosition* logpos, APD_PLY_GenericCharacter* genCharacter);

public:
	PD_GM_MapManager();
	~PD_GM_MapManager();

	PD_MG_StaticMap* StaticMapRef;
	PD_MG_DynamicMap* DynamicMapRef;


#pragma region GET INFO OF THE MAP


	// Dada una posici�n l�gica, devuelve el GenericCharacter que est� en esa posici�n, que adem�s es un Player. 0 si no hay ninguno. 
	bool getPlayerAt(PD_MG_LogicPosition* logpos, APD_PLY_GenericCharacter* genCharacter);
	// Dada una posici�n l�gica, devuelve el GenericCharacter que est� en esa posici�n, que adem�s es un Enemy. 0 si no hay ninguno. 
	bool getEnemyAt(PD_MG_LogicPosition* logpos, APD_PLY_GenericCharacter* genCharacter);

	// Esta funcion devolver� un Interactuable
	AActor* getInteractuableAt(PD_MG_LogicPosition* logpos);

	//Dada un LogicPosition, te da la posici�n en el mundo.
	FVector* LogicToWorldPosition(PD_MG_LogicPosition* pos);

	//Dada la posici�n en el mundo, te devuelve la posici�n l�gica que estar�a asignada a posici�n en el mundo, exista o no en el mapa. 
	PD_MG_LogicPosition* WorldToLogicPosition(FVector* pos);


#pragma endregion




};
