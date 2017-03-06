// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

//forward declaration
#include "Structs/PD_ServerEnums.h"
struct FStructTurnOrders;
class PD_GM_LogicCharacter;


class PATD_SERVER_API PD_GM_EnemyManager
{

private:
	//Quizas podriamos hacer un StructEnemy para guardar informacion asociada que podamos querer de un enemigo. (Las ordenes por ejemplo)
	TArray<PD_GM_LogicCharacter*> enemies;
	TArray<FStructTurnOrders*> listTurnOrders;
public:
	PD_GM_EnemyManager();
	~PD_GM_EnemyManager();

	void AddEnemy(PD_GM_LogicCharacter* enemy);
	TArray<PD_GM_LogicCharacter*> GetEnemies();
	FStructTurnOrders* GetTurnOrders(int indexEnemy);

	void newTurn();
	void AddActionTurn(FStructTurnOrders* turnOrders);
	int GetEnemyMaxLenghtActions(EActionPhase phase);
	int GetMaxLenghtActions(EActionPhase phase);
	TArray<FStructTurnOrders*> getListTurnOrders() { return listTurnOrders; }
};
