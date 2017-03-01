//Fill out your copyright notice in the Description page of Project Settings.

#pragma once


#include "Structs/PD_ServerStructs.h"

#include "MapGeneration/PD_MG_LogicPosition.h"

//forward declarations
#include "Structs/PD_ServerEnums.h"

struct FStructBasicStats;
struct FStructInitBaseStats;
struct FStructSkills;
struct FStructWeapon;
struct FStructSkin;
struct FStructTotalStats;

class PD_MG_LogicPosition;
class AMyCharacterParent;
class APD_GenericController;
class PD_GM_MapManager;

class PATD_SERVER_API PD_GM_LogicCharacter
{


	//Variables del Character Generico 
	bool isPlayer; //Define si es Jugador (0) o Enemigo (1) - Sirve para el Casteo de Controllers entre otras cosas
	FString ID_character;//Si es Player = 00, Si es Enemigo = 01, n. DEfine el tipo de enemigo que es para rellenar los Stats u el Aspecto (BP)
	ECharacterType type_character; //0 si es player, N depende del enemigo
	bool isDead; //cuando su HPcurrent =< 0 -> True, sino False ---> Para acabar el juego


				 //Controlador GenericoPadre. Se Castea a CharacterController o EnemyController dependiendo de la variable isPlayer.
	APD_GenericController* controller;

	//Character
	AMyCharacterParent* characterParent;
	ACharacter* character_Player_BP; //BP donde esta el PROP del Character


	PD_MG_LogicPosition currentLogicalPosition = PD_MG_LogicPosition(0, 0); //Posicion actual del personaje
	PD_MG_LogicPosition movingLogicalPosition = PD_MG_LogicPosition(0, 0); //Posicion del personaje en cada tick (Todo Logico, el personaje NO SE MUEVE)

	//Referencia al Mapa de la partida para que se pueda Mover-atacar-interactuar con los elementos del mismo
	PD_GM_MapManager* mapMng;
	//PlayerManager* playerMng;
	//EnemiesManager* EnemiesMng;


public:
	PD_GM_LogicCharacter();
	~PD_GM_LogicCharacter();

	//Variables - Stats del personaje , arma y habilidades
	FStructBasicStats* basicStats;
	FStructInitBaseStats* initBaseStats;
	FStructSkills* skills;
	FStructWeapon* weapon;
	FStructSkin* skin;
	FStructTotalStats* totalStats;

	/*
	//Metodo para encontrar el camino adecuado para ir a la posicion deseada
	Recibe:
	- LogicalPosition (X e Y) para ejecutar el algoritmo A*
	*/
	TArray<FStructOrderAction>* FindPathToMove(PD_MG_LogicPosition desirePosition);

	/*
	//Metodo para simular el movimiento logico
	Recibe:
	- Array de FstructOrdenAction con los movimientos que tiene que hacer hasta la posici�n final (ultimo index)
	- Int con la posicion del Array de instrucciones que tiene que simular
	Devuelve:
	- Un posicion logica, indicando la casilla destino del movimiento despues de la simulacion
	*/
	//PD_MG_LogicPosition* MoveToLogicPosition(uint8 tick, TArray<FStructOrderAction> listMove);
	PD_MG_LogicPosition* MoveToLogicPosition(FStructOrderAction* order);
	/*
	//Metodo para moverse visualmente por el mapa
	Recibe:
	- la Posicion Logica de la casilla a realizar la accion
	Devuelve:
	- Un bool para indicar si la accion se ha resuelto con exito o no
	*/
	bool MoveToPhysicalPosition(PD_MG_LogicPosition* targetPosition);


	/*
	//Metodo para realizar una acci�n
	Recibe:
	- la Posicion Logica de la casilla a realizar la accion: PosX y PosY
	- un ID de la accion - Habilidad o AtaqueBasico o Interactuar sobre esa casilla (sera un Enumerado)
	Devuelve:
	- Un bool para indicar si la accion se ha resuelto con exito o no
	*/
	bool ActionTo(PD_MG_LogicPosition targetPosition, uint32 acction);

	/*
	//Metodo para consumir un item
	Recibe:
	- Un ID para saber el tipo de item que se va a consumir
	*/
	void ConsumeItem(uint32 idItem);

	/*
	//Metodo para cuando un personaje es herido
	Recibe:
	- Un float que indica la vida que se le quita
	*/
	void UpdateHPCurrent(float receivedDamage);

	/*
	//Metodo para conseguir la tirada de un numero determinado de dados
	con un numero concreto de caras
	Recibe:
	- Un int con el numero de dados que se lanzan
	- Un int con el n�mero de caras del dado
	Devuelve:
	- La suma de lo que salga en cada dado
	*/
	int8 GetARoll(int8 numOfDice, int32 numOfFaces);

	/*
	//Metodo para saber el porcentaje de bonus de la probabilidad de IMPACTAR
	Devuelve:
	- el porcentaje que se tiene que a�adir al Impact Bonus%
	*/
	float ImpactPercentage();

	/*
	//Metodo para saber el porcentaje de bonus de la probabilidad de EVADIR
	Devuelve:
	- el porcentaje que se tiene que a�adir al Impact Bonus%
	*/
	float EvasionPercentage();

	/*
	//Metodo para saber el valor que determina si ha impactado sobre un enemigo o no
	Devuelve:
	- el porcentaje que se tiene que a�adir al Impact Bonus%
	*/
	int8 GetImpactCharacter();

	/*
	//Metodo para saber el valor que determina si ha evadido un ataque o no
	Devuelve:
	- el porcentaje que se tiene que a�adir al Impact Bonus%
	*/
	int8 GetEvasionCharacter();

	/* ===================
	METODOS AUXILIARES GET Y SET
	========================= */
	//Metodos GET para cada Struct
	FStructBasicStats* GetBasicStats();
	FStructInitBaseStats* GetInitBaseStats();
	FStructSkills* GetSkills();
	FStructWeapon* GetWeapon();
	FStructSkin* GetSkin();
	FStructTotalStats* GetTotalStats();

	//Metodos GET para variables globales de la clase
	bool GetIsPlayer();
	bool GetIsDead();
	FString GetIDCharacter();
	ECharacterType GetTypeCharacter();
	APD_GenericController* GetController();
	AMyCharacterParent* GetCharacterParent();
	ACharacter* GetCharacterBP();
	PD_MG_LogicPosition* GetCurrentLogicalPosition();
	PD_MG_LogicPosition* GetMovingLogicalPosition();

	//M�todos SET para cada Struct
	void SetBasicStats(int nPOD, int nAGI, int nDES, int nCON, int nPER, int nMAL);
	void SetInitBaseStats(int nHP, int nDMG, int nAP);
	void SetSkills(TArray<uint8> nActSkills, TArray<uint8> nPasSkills);
	void SetWapon();
	void SetSkin();
	void SetTotalStats();

	//Metodos SET para variables globales de la clase
	void SetIsPlayer(bool nIsPlayer);
	void SetIsDead(bool nIsDead);
	void SetIDCharacter(FString nID_character);
	void SetTypeCharacter(ECharacterType ntype_character);
	void SetController(APD_GenericController* ncontroller);
	void SetCharacterParent(AMyCharacterParent* ncharacterParent);
	void SetCharacterBP(ACharacter* ncharacter_Player_BP); //BP donde esta el PROP del Character
	void SetCurrentLogicalPosition(PD_MG_LogicPosition* ncurrentLogicalPosition);
	void SetMovingLogicalPosition(PD_MG_LogicPosition* nmovingLogicalPosition);
	void SetMapManager(PD_GM_MapManager* nmapManager);

};
