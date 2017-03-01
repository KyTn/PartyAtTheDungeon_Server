// Fill out your copyright notice in the Description page of Project Settings.

#include "PATD_Server.h"
#include "PD_GM_EnemyManager.h"

//Includes del forward declaration
#include "Structs/PD_ServerStructs.h"//Para todos los structs
#include "PATD_Server/GM_Game/LogicCharacter/PD_GM_LogicCharacter.h"


PD_GM_EnemyManager::PD_GM_EnemyManager()
{

}

PD_GM_EnemyManager::~PD_GM_EnemyManager()
{
}


void PD_GM_EnemyManager::AddEnemy(PD_GM_LogicCharacter* enemy) {
	enemies.Add(enemy);
	
}

TArray<PD_GM_LogicCharacter*> PD_GM_EnemyManager::GetEnemies() {
	return enemies;
}


FStructTurnOrders* PD_GM_EnemyManager::GetTurnOrders(int indexEnemy) {
	return listTurnOrders[indexEnemy];
}


//Devuelve el numero de acciones de la lista mas larga
int PD_GM_EnemyManager::GetMaxLenghtActions(EActionPhase phase) {
	int indexplayer = this->GetEnemyMaxLenghtActions(phase);
	if (indexplayer != -1) {
		if (phase == EActionPhase::Move) {
			return this->listTurnOrders[indexplayer]->listMove.Num();
		}
		else if (phase == EActionPhase::Attack) {
			return this->listTurnOrders[indexplayer]->listAttack.Num();
		}
	}
	return 0;


}
//Devuelve el index del jugador con la lista de acciones mas larga
int PD_GM_EnemyManager::GetEnemyMaxLenghtActions(EActionPhase phase) {
	int numTicks = -1;
	int indexPlayer = -1;

	for (int i = 0; this->GetEnemies().Num(); i++) {
		TArray<FStructOrderAction> listActions;
		if (phase == EActionPhase::Move) {
			listActions = this->listTurnOrders[i]->listMove;
		}
		else if (phase == EActionPhase::Attack) {
			listActions = this->listTurnOrders[i]->listAttack;
		}


		if (numTicks < (listActions.Num())) {
			numTicks = listActions.Num();
			indexPlayer = i;
		};
	}
	return numTicks;
}