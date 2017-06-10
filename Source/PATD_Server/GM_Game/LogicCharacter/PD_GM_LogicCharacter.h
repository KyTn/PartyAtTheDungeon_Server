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
	ACharacter* character_Player_BP; //BP donde esta el PROP del Character


	PD_MG_LogicPosition currentLogicalPosition = PD_MG_LogicPosition(0, 0); //Posicion actual del personaje
	TArray<PD_MG_LogicPosition> movingLogicalPosition = TArray<PD_MG_LogicPosition>(); //Array de posiciiones logicas en las que se movera en la fase de movimiento
	bool isStoppingByCollision = false; //determina si se ha parado por collision (y ha perdido esta) //False no se para, true si se para

	//Referencia al Mapa de la partida para que se pueda Mover-atacar-interactuar con los elementos del mismo
	PD_GM_MapManager* mapMng;
	//PlayerManager* playerMng;
	//EnemiesManager* EnemiesMng;

	//Solo para IA
	EIAPersonality IAPersonality;


public:
	PD_GM_LogicCharacter();
	~PD_GM_LogicCharacter();

	//Variables - Stats del personaje , arma y habilidades
	FStructBasicStats* basicStats;
	FStructInitBaseStats* initBaseStats;
	FStructCharacterSkills* skills;
	FStructWeapon* weapon;
	FStructSkin* skin;
	FStructTotalStats* totalStats;
	FStructCharacterState* characterState;

	/*
	//Metodo para encontrar el camino adecuado para ir a la posicion deseada
	Recibe:
	- LogicalPosition (X e Y) para ejecutar el algoritmo A*
	*/
	TArray<FStructOrderAction>* FindPathToMove(PD_MG_LogicPosition desirePosition);

	/*
	//Metodo para simular el movimiento logico
	Recibe:
	- Array de FstructOrdenAction con los movimientos que tiene que hacer hasta la posición final (ultimo index)
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
	bool MoveToPhysicalPosition(TArray<FVector> listPositionsToMove);
	void SetCurrentLogicalPositionFromFVector(FVector PostInWorld);

	/*
	//Metodo para realizar una acción
	Recibe:
	- la Posicion Logica de la casilla a realizar la accion: PosX y PosY
	- un ID de la accion - Habilidad o AtaqueBasico o Interactuar sobre esa casilla (sera un Enumerado)
	Devuelve:
	- Un bool para indicar si la accion se ha resuelto con exito o no
	*/
	bool ActionTo(FStructTargetToAction action);

	bool UseInteractable();

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
	void UpdateHPCurrent(float updateLifeIn);

	/*
	//Metodo para conseguir la tirada de un numero determinado de dados
	con un numero concreto de caras
	Recibe:
	- Un int con el numero de dados que se lanzan
	- Un int con el número de caras del dado
	Devuelve:
	- La suma de lo que salga en cada dado
	*/
	int8 GetARoll(int8 numOfDice, int32 numOfFaces);

	/*
	//Metodo para saber el porcentaje de bonus de la probabilidad de IMPACTAR
	Devuelve:
	- el porcentaje que se tiene que añadir al Impact Bonus%
	*/
	float ImpactPercentage();

	/*
	//Metodo para saber el porcentaje de bonus de la probabilidad de EVADIR
	Devuelve:
	- el porcentaje que se tiene que añadir al Impact Bonus%
	*/
	float EvasionPercentage();

	/*
	//Metodo para saber el valor que determina si ha impactado sobre un enemigo o no
	Devuelve:
	- el porcentaje que se tiene que añadir al Impact Bonus%
	*/
	int8 GetImpactCharacter();

	/*
	//Metodo para saber el valor que determina si ha evadido un ataque o no
	Devuelve:
	- el porcentaje que se tiene que añadir al Impact Bonus%
	*/
	int8 GetEvasionCharacter();

	//Moverse cuando se choca con alquien y pierde el choque
	void MoveWhenCollisionLost();
	void SetCurrentLogicalPositionFromOut(FVector positionWorld);

	//Da valor para comparar el choque en caso de que tenga el mismo CH
	int GetAValueToDecideCollision();
	// Calcular si ha sido CRITICO y su %, devuelve el valor final
	bool CheckIfWasACriticalAttack(int* initialDamage, PD_GM_LogicCharacter* character);


	/* ========================================
	FUNCIONES DE LAS HABILIDADES / DEFINEN EL COMPORTAMIENTO DE ESTAS
	============================*/
	///Comprobar los estados del jugador o los powerUps para ver si se incrementa el ataque o no
	void CheckCharacterActiveEffects(PD_GM_LogicCharacter* CharWhoAttacks);

	///Calcular el % de defensa que tiene un jugador
	int CalculateReductionOfDamage(PD_GM_LogicCharacter* CharWhoDeffense);
	int CalculateIncreaseOfDamage(PD_GM_LogicCharacter* CharWhoAttacks);

	///Calcular los AP restantes de las acciones del jugador - Para habilidad defensa o las que gasten todo el AP
	int CalculateAPleftInPlayerActions(PD_GM_LogicCharacter* CharWhoAttacks);

	//Ataque basico
	void Skill_BasicAttack(PD_GM_LogicCharacter* CharWhoAttacks, PD_GM_LogicCharacter* CharWhoReceiveTheAttacks);
	//Protegerse hasta el siguiente turno (HAB SIN ESCUDO)
	void Skill_Defense(PD_GM_LogicCharacter* CharWhoAttacks);

	/*MELE*/
	///Dagas
	void Skill_Melee_Daggers_WhenFua(PD_GM_LogicCharacter* CharWhoAttacks);
	///Mandoble
	void Skill_Melee_LargeSword_JumpFatTigger(PD_GM_LogicCharacter* CharWhoAttacks, PD_MG_LogicPosition PositionToJump);
	///Melee
	void Skill_Melee_Hostion(PD_GM_LogicCharacter* CharWhoAttacks, PD_GM_LogicCharacter* CharWhoReceiveTheAttacks);

	/*RANGO*/
	///Guns
	void Skill_Range_Guns_SomeHit(PD_GM_LogicCharacter* CharWhoAttacks, PD_GM_LogicCharacter* CharWhoReceiveTheAttacks);
	///Range
	void Skill_Range_RightInTheAsshole(PD_GM_LogicCharacter* CharWhoAttacks, PD_GM_LogicCharacter* CharWhoReceiveTheAttacks);

	/*MAGIA*/
	///Magic
	void Skill_Magic_GiveMeTheFireBlas(PD_GM_LogicCharacter* CharWhoAttacks, PD_GM_LogicCharacter* CharWhoReceiveTheAttacks);
	void Skill_Magic_ExclaimChas(PD_GM_LogicCharacter* CharWhoAttacks, PD_MG_LogicPosition PositionToTeleport);
	void Skill_Magic_BeInCrossroads(PD_GM_LogicCharacter* CharWhoAttacks);
	void Skill_Magic_WhoHeal(PD_GM_LogicCharacter* CharWhoAttacks, PD_GM_LogicCharacter* CharWhoReceiveTheAttacks);
	void Skill_Magic_ShotMana(PD_GM_LogicCharacter* CharWhoAttacks);

	/* ===================
	METODOS AUXILIARES GET Y SET
	========================= */
	//Metodos GET para cada Struct
	FStructBasicStats* GetBasicStats();
	FStructInitBaseStats* GetInitBaseStats();
	FStructCharacterSkills* GetSkills();
	FStructWeapon* GetWeapon();
	FStructSkin* GetSkin();
	FStructTotalStats* GetTotalStats();
	FStructCharacterState* GetCharacterState();

	//Metodos GET para variables globales de la clase
	bool GetIsPlayer();
	bool GetIsDead();
	FString GetIDCharacter();
	ECharacterType GetTypeCharacter();
	APD_GenericController* GetController();
	ACharacter* GetCharacterBP();
	PD_MG_LogicPosition GetCurrentLogicalPosition();
	TArray<PD_MG_LogicPosition> GetMovingLogicalPosition();
	bool GetIsStoppingByCollision();
	EIAPersonality GetIAPersonality();

	//Métodos SET para cada Struct
	void SetBasicStats(int nPOD, int nAGI, int nDES, int nCON, int nPER, int nMAL);
	void SetInitBaseStats(int nHP, int nDMG, int nAP);
	void SetSkills(TArray<FStructSkill> nActSkills, TArray<FStructSkill> nPasSkills);
	void SetWeapon();
	void SetSkin();
	void SetTotalStats();

	//Metodos SET para variables globales de la clase
	void SetIsPlayer(bool nIsPlayer);
	void SetIsDead(bool nIsDead);
	void SetIDCharacter(FString nID_character);
	void SetTypeCharacter(ECharacterType ntype_character);
	void SetController(APD_GenericController* ncontroller);
	void SetCharacterBP(ACharacter* ncharacter_Player_BP); //BP donde esta el PROP del Character
	void SetCurrentLogicalPosition(PD_MG_LogicPosition ncurrentLogicalPosition);
	void AddMovementLogicalPosition(PD_MG_LogicPosition nnewLogicalPosition);
	void SetIsStoppingByCollision(bool nIsStoppingByCollision);
	void SetMapManager(PD_GM_MapManager* nmapManager);
	void SetIAPersonality(EIAPersonality inPersonality);

	void ClearMovingLogicalPosition();

	void UpdatePointsCurrent(int inPoints);

};
