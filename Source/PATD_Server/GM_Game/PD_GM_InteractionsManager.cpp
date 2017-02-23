// Fill out your copyright notice in the Description page of Project Settings.

#include "PATD_Server.h"
#include "PD_GM_InteractionsManager.h"

//includes de uso
#include "MapGeneration/PD_MG_LogicPosition.h"
#include "LogicCharacter/PD_GM_PlayerLogicCharacter.h"
#include "Actors/Players/PD_PLY_Controller.h"

//Includes of forward declaration
#include "PD_PlayersManager.h"
#include "PD_GM_MapManager.h"
#include "Structs/PD_ServerStructs.h"//Para todos los structs


PD_GM_InteractionsManager::PD_GM_InteractionsManager(PD_PlayersManager* inPlayersManager, PD_GM_MapManager* inMapManager)
{
	playersManager = inPlayersManager;
	mapManager = inMapManager;
}

PD_GM_InteractionsManager::~PD_GM_InteractionsManager()
{
}

void PD_GM_InteractionsManager::IntitializeTurnStates() {
	ItemTurnInformation->CurrentState = InteractionStates::Ready;
	MoveTurnInformation->CurrentState = InteractionStates::Ready;
	AttackTurnInformation->CurrentState = InteractionStates::Ready;
}

//Control de los turnos

void PD_GM_InteractionsManager::PlayersLogicTurn() {

	LogicTurnItemPhase();
	LogicTurnMovePhase();
	LogicTurnAttackPhase();
}

void PD_GM_InteractionsManager::LogicTurnItemPhase() {
	ItemTurnInformation->CurrentState = InteractionStates::Working;
}

void PD_GM_InteractionsManager::LogicTurnMovePhase() {
	MoveTurnInformation->CurrentState = InteractionStates::Working;
	//Calcular el numero de ticks a realizar (el de la lista mas larga) -Aqui o en el playerManager?- Yo creo que mejor en el playerManager
	int numTicks = 0;
	for (int i = 0; playersManager->GetNumPlayers(); i++) {
		if (numTicks < (playersManager->getDataStructPlayer(i)->turnOrders->listMove.Num())) {
			numTicks = playersManager->getDataStructPlayer(i)->turnOrders->listMove.Num();
		};
	}
	//Ejecutar los ticks
	for (int i = 0; i < numTicks; i++) {
		TickMovePhase(i);
	}
}

void PD_GM_InteractionsManager::LogicTurnAttackPhase() {
	AttackTurnInformation->CurrentState = InteractionStates::Working;
}


//Control de los Ticks

void PD_GM_InteractionsManager::TickItemPhase(int tick) {

}

void PD_GM_InteractionsManager::TickMovePhase(int tick) {
	for (int i = 0; playersManager->GetNumPlayers(); i++) {

		//Resolucion de conflictos? choques etc.
		StructPlayer* structPlayer = playersManager->getDataStructPlayer(i);
		FStructOrderAction order = structPlayer->turnOrders->listMove[tick];

		structPlayer->logicCharacter->ProcessMoveOrder(order);
		//Llamada a moverse actor provisional
		//Aqui en realidad no tendriamos la posicion final en el logicCharacter, pues podria haber choques 
		//al moverse los siguientes personajes.
		PD_GM_PlayerLogicCharacter* logicCharacter = structPlayer->logicCharacter;
		PD_MG_LogicPosition* logicPosition = logicCharacter->getLogicPosition();
		FVector realPosition= mapManager->getUWorldPosition(logicPosition);
		playersManager->getDataStructPlayer(i)->actorController->Move(realPosition.X, realPosition.Y);
	}
}
//Ejemplo conflicto con choques
/*
Mapa:
W.1
W32

Ordenes:
1<
2<
3^

a)si el orden se hace 1, 2, 3 dara choque en el movimiento de 2 con 3
b)si el orden se hace 3, 2, 1 dara choque en el movimiento de 1 con 3

Si se hace que al chocar compruebe si es contra una unidad que ya haya realizado su movimiento:
-En el orden a) Estarian en la misma casilla momentaneamente 2 y 3. Justo mientras 2 mueve a la posicion de 3 y no hay choque, pero 3 aun no se ha movido.
-Que pasa si 3 hace < en vez de ^ (en el orden a) 2 invadiria la casilla de 3, pero se queda porque 3 aun no ha movido. Cuando 3 mueve y se choca contra la
pared, le devolveria a su casilla y seria él, el que tendria que detectar un choque al estar en ella 2.

Hacer sistema de conflictos para resolver todo a posteriori? Entre movimiento y resolucion de conflictos, el mapa se queda en una situacion que permite la
existencia de mas de un personaje por casilla.
*/
void PD_GM_InteractionsManager::TickAttackPhase(int tick) {

}