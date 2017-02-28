// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

class PD_GM_LogicCharacter;
class PATD_SERVER_API PD_GM_EnemyManager
{

private:
	TArray<PD_GM_LogicCharacter*> enemies;

public:
	PD_GM_EnemyManager();
	~PD_GM_EnemyManager();

	void AddEnemie(PD_GM_LogicCharacter* enemy);
	TArray<PD_GM_LogicCharacter*> GetEnemies();
};
