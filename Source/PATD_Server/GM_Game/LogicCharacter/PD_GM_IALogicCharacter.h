// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "PATD_Server/Actors/Enemies/PD_E_EnemiesEnum.h"
/**
 * 
 */
class PATD_SERVER_API PD_GM_IALogicCharacter 
{
private:
	//EEnemiesType type;
	AActor* character;
public:


	PD_GM_IALogicCharacter();
	void UpdateCharacter(AActor* character);
	~PD_GM_IALogicCharacter();
	//Hereda de iLogicCharacter
};
