// Fill out your copyright notice in the Description page of Project Settings.

#include "PATD_Server.h"
#include "PD_GM_MapManager.h"
#include "PATD_Server/MapGeneration/Static/PD_MG_StaticMap.h"
#include "PATD_Server/MapGeneration/Dynamic/PD_MG_DynamicMap.h"
#include "PATD_Server/MapInfo/MapInstantiation/MapInstantiatorActor.h"
#include "PATD_Server/Structs/PD_ServerEnums.h"
#include "PATD_Server/GM_Game/PD_GM_EnemyManager.h"
#include "PATD_Server/GM_Game/LogicCharacter/PD_GM_LogicCharacter.h"
#include "PATD_Server/Actors/Enemies/PD_E_EnemyCharacter.h"
//include of forward declaration
#include "MapGeneration/PD_MG_LogicPosition.h"

PD_GM_MapManager::PD_GM_MapManager()
{
}

PD_GM_MapManager::~PD_GM_MapManager()
{
}

#pragma region GET INFO OF THE MAP

/*
bool PD_GM_MapManager::getGenericCharacterAt(PD_MG_LogicPosition* logpos, APD_PLY_GenericCharacter* genCharacter) { return false; }
bool PD_GM_MapManager::getPlayerAt(PD_MG_LogicPosition* logpos, APD_PLY_GenericCharacter* genCharacter) { return false; }
bool PD_GM_MapManager::getEnemyAt(PD_MG_LogicPosition* logpos, APD_PLY_GenericCharacter* genCharacter) { return false; }
*/
AActor* PD_GM_MapManager::getInteractuableAt(PD_MG_LogicPosition* logpos) { return 0; }


FVector* PD_GM_MapManager::LogicToWorldPosition(PD_MG_LogicPosition* pos) {
	return new FVector(-1.0f * pos->GetX()*100.0f, pos->GetY() * 100.0f, 0.f);
}

PD_MG_LogicPosition* PD_GM_MapManager::WorldToLogicPosition(FVector* pos) {
	float x, y;

	x = -1.0f * pos->X / 100.f;
	y = pos->Y / 100.f;

	return new PD_MG_LogicPosition((int)roundf(x), (int)roundf(y));
}

#pragma endregion 




#pragma region INSTANTIATE MAP

void PD_GM_MapManager::InstantiateStaticMap() {

	for (int i = 0; i < StaticMapRef->GetLogicPositions().Num(); i++) {

		/**/
		switch (StaticMapRef->GetXYMap()[*StaticMapRef->GetLogicPositions()[i]]) {
		case 'w':
			instantiator->InstantiateWall(StaticMapRef->GetLogicPositions()[i]);
			break;

		case '.':
			instantiator->InstantiateTile(StaticMapRef->GetLogicPositions()[i]);
			break;
		case 'd':

			instantiator->InstantiateTile(StaticMapRef->GetLogicPositions()[i]);
			break;
			/*default:

			parserActor->InstantiateTile(staticMap->GetLogicPositions()[i]);
			break;
			*/
		}
	}
}


void PD_GM_MapManager::InstantiateDynamicMap(PD_GM_EnemyManager* enemyMan) {
	ECharacterType enemyType;
	///necesitamos comprobar ya el ID
	for (int i = 0; i < DynamicMapRef->GetLogicPositions().Num(); i++) {

		
		enemyType = DynamicMapRef->getEnemies()[*DynamicMapRef->GetLogicPositions()[i]][0]->GetTypeCharacter();///Cogemos el tipo
		
		for (int j = 0; j < enemyMan->GetEnemies().Num(); j++) {
			if (enemyMan->GetEnemies()[j]->GetIDCharacter() == DynamicMapRef->getEnemies()[*DynamicMapRef->GetLogicPositions()[i]][0]->GetIDCharacter()) {///Comprobamos el ID, para asignar el Character correctamente

				switch (enemyType) {
					case ECharacterType::Archer: {
						APD_E_EnemyCharacter* charac = instantiator->InstantiateArcher(DynamicMapRef->GetLogicPositions()[i]);
						//DynamicMapRef->UpdateActor(charac, DynamicMapRef->GetLogicPositions()[i]);///instancia el objeto fisico en el l�gico
						enemyMan->GetEnemies()[j]->SetCharacterBP(charac);
						break;
					}
					case ECharacterType::Zombie: {
						APD_E_EnemyCharacter* charac = instantiator->InstantiateZombie(DynamicMapRef->GetLogicPositions()[i]);
						//DynamicMapRef->UpdateActor(charac, DynamicMapRef->GetLogicPositions()[i]);///instancia el objeto fisico en el l�gico
						enemyMan->GetEnemies()[j]->SetCharacterBP(charac);
						break;
					}
				}
			}
		}
	}
}

#pragma endregion