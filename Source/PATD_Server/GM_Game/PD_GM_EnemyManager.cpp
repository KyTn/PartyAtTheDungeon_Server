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

void PD_GM_EnemyManager::newTurn() {
	listTurnOrders.Empty(); 
	for (int i = 0; i < enemies.Num(); i++) {
		listTurnOrders.Add(nullptr);
	}
	//listTurnOrders.AddDefaulted(enemies.Num());//Iniciamos una posicion en el array con null para cada enemigo. Cuando esta posicion no sea null consideramos que ha recibido ordenes
	UE_LOG(LogTemp, Log, TEXT("PD_GM_EnemyManager::newTurn: Index of ListTurnOrders %d"), listTurnOrders.Num());

}

void PD_GM_EnemyManager::AddActionTurn(FStructTurnOrders* turnOrders, int indexEnemy) {
	listTurnOrders[indexEnemy]= turnOrders;
}

FStructTurnOrders* PD_GM_EnemyManager::GetTurnOrders(int indexEnemy) {
	return listTurnOrders[indexEnemy];
}


//Devuelve el numero de acciones de la lista mas larga
int PD_GM_EnemyManager::GetMaxLenghtActions(EActionPhase phase) {
	int indexplayer = this->GetEnemyMaxLenghtActions(phase);
	if (indexplayer != -1) {
		if (phase == EActionPhase::Move) {
			return this->listTurnOrders[indexplayer]->positionsToMove.Num();
		}
		else if (phase == EActionPhase::Attack) {
			return this->listTurnOrders[indexplayer]->actions.Num();
		}
	}
	return 0;
}

//Devuelve el index del jugador con la lista de acciones mas larga
int PD_GM_EnemyManager::GetEnemyMaxLenghtActions(EActionPhase phase) {
	int numTicks = -1;
	int indexPlayer = -1;

	for (int i = 0; i<this->listTurnOrders.Num(); i++) {
	//	(LogTemp, Error, TEXT("Enemigos totales: %i"), this->listTurnOrders.Num());
	//	UE_LOG(LogTemp, Error, TEXT("Añade acciones enemy:%i"), i);

		int listActions = 0;
		if (phase == EActionPhase::Move) {
			if (this->listTurnOrders[i]->positionsToMove.Num() > 0) {}
			//UE_LOG(LogTemp, Error, TEXT("Enemigo %i se mueve"), i);
			listActions = this->listTurnOrders[i]->positionsToMove.Num();
		}
		else if (phase == EActionPhase::Attack) {
			//UE_LOG(LogTemp, Error, TEXT("Enemigo %i ataca"), i);
			if(this->listTurnOrders[i]->actions.Num()>0) {}
			listActions = this->listTurnOrders[i]->actions.Num();
		}


		if (numTicks < (listActions)) {
			numTicks = listActions;
			indexPlayer = i;
		};
	}
	return indexPlayer;
}


bool  PD_GM_EnemyManager::AllAnimationEnd() {
	UE_LOG(LogTemp, Log, TEXT("PD_GM_EnemyManager::AllAnimationEnd"));
	for (PD_GM_LogicCharacter* enemyLogicCharacter : enemies) {
		if (enemyLogicCharacter->GetController()->IsAtAnimation()) {
			return false;
		}
	}
	return true;
}

PD_GM_LogicCharacter* PD_GM_EnemyManager::GetCharacterByID(FString id) {
	for (int i = 0; i < enemies.Num(); i++) {
		if (enemies[i]->GetIDCharacter() == id) {
			return enemies[i];
		}
	}
	UE_LOG(LogTemp, Warning, TEXT("PD_GM_EnemyManager:: GetCharacterByID ERROR: No se ha encontrado character con id %s"), *id);
	return nullptr;
}

PD_GM_LogicCharacter* PD_GM_EnemyManager::GetCharacterByIndex(int iEnemy) {
	return enemies[iEnemy];
	//UE_LOG(LogTemp, Warning, TEXT("PD_GM_EnemyManager:: GetCharacterByIndex ERROR: No se ha encontrado character con indice %d"), *iEnemy);
	//return nullptr;
}

int PD_GM_EnemyManager::GetIndexByID(FString id) {
	for (int i = 0; i < enemies.Num(); i++) {
		if (enemies[i]->GetIDCharacter() == id) {
			return i;
		}
	}
	UE_LOG(LogTemp, Warning, TEXT("PD_GM_EnemyManager:: GetCharacterByID ERROR: No se ha encontrado character con id %s"), *id);
	return -1;
}



bool PD_GM_EnemyManager::AllEnemiesHaveOrders() {
	UE_LOG(LogTemp, Log, TEXT("PD_GM_EnemyManager::AllEnemiesHaveOrders"));

	int numWithOrders = 0;
	for (int i = 0; i < enemies.Num();i++) {

		//(listTurnOrders[i]->actions.Num() == 0) && (listTurnOrders[i]->positionsToMove.Num() == 0)
		if (!listTurnOrders[i] ) {
			//listTurnOrders[i] = new FStructTurnOrders();
			UE_LOG(LogTemp, Log, TEXT("PD_GM_EnemyManager::AllEnemiesHaveOrders: Enemigo numero %d no tiene ordenes"),i);
			return false;
		}
		else {
			numWithOrders++;
		}
	}
	//if (numWithOrders>0) return true;
	//else return false;
	UE_LOG(LogTemp, Log, TEXT("PD_GM_EnemyManager::AllEnemiesHaveOrders: Todos los enemigos (%d) tienen ordenes "), numWithOrders);
	return true;
}