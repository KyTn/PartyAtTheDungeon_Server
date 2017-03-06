// Fill out your copyright notice in the Description page of Project Settings.

#include "PATD_Server.h"
#include "PD_GM_AIManager.h"
#include "PD_GM_MapManager.h"
#include "Structs/PD_NetStructs.h"
#include "PATD_Server/Structs/PD_ServerEnums.h"
#include "PATD_Server/GM_Game/LogicCharacter/PD_GM_LogicCharacter.h"
#include "PATD_Server/Actors/Enemies/PD_AIController.h"



PD_GM_AIManager::PD_GM_AIManager()
{
}

PD_GM_AIManager::~PD_GM_AIManager()
{
}

FStructTurnOrders* PD_GM_AIManager::AIExecEnemy(PD_GM_LogicCharacter* charac, PD_GM_MapManager* mngrMap) {
	FStructTurnOrders* orders;
	orders = NULL;
	switch (charac->GetTypeCharacter()) {
		case ECharacterType::Archer: {
			APD_AIController* contr = new APD_AIController();
			contr->StartTurnZombie(mngrMap, charac->GetCurrentLogicalPosition());
			while (!contr->EndBehaviour()); ///esperamos a que gaste los AP
			orders = contr->GetActions();
			//contr->Destroy(); Creo que se deberia destruir, no se si lo hace al salir del case
			break;
		}
		case ECharacterType::Zombie: {
			APD_AIController* contr = new APD_AIController();
			contr->StartTurnZombie(mngrMap, charac->GetCurrentLogicalPosition());
			while (!contr->EndBehaviour()); ///esperamos a que gaste los AP
			orders = contr->GetActions();
			//contr->Destroy(); Creo que se deberia destruir, no se si lo hace al salir del case
			break; 
		}
	}
	return orders;
}
