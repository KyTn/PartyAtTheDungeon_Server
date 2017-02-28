// Fill out your copyright notice in the Description page of Project Settings.

#include "PATD_Server.h"
#include "PD_GM_EnemyManager.h"
#include "PATD_Server/GM_Game/LogicCharacter/PD_GM_LogicCharacter.h"

PD_GM_EnemyManager::PD_GM_EnemyManager()
{

}

PD_GM_EnemyManager::~PD_GM_EnemyManager()
{
}


void PD_GM_EnemyManager::AddEnemie(PD_GM_LogicCharacter* enemy) {
	enemies.Add(enemy);
}

TArray<PD_GM_LogicCharacter*> PD_GM_EnemyManager::GetEnemies() {
	return enemies;
}
