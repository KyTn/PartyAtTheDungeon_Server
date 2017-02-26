// Fill out your copyright notice in the Description page of Project Settings.

#include "PATD_Server.h"
#include "PD_GM_IALogicCharacter.h"
#include "PATD_Server/Actors/Enemies/PD_E_EnemiesEnum.h"


PD_GM_IALogicCharacter::PD_GM_IALogicCharacter()
{
}

void PD_GM_IALogicCharacter::UpdateCharacter(AActor* charact) {
	this->character = charact;
}

PD_GM_IALogicCharacter::~PD_GM_IALogicCharacter()
{
}
