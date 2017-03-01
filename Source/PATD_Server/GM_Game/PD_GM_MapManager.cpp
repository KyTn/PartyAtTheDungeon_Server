// Fill out your copyright notice in the Description page of Project Settings.

#include "PATD_Server.h"
#include "PD_GM_MapManager.h"
#include "PATD_Server/MapGeneration/Static/PD_MG_StaticMap.h"
#include "PATD_Server/MapGeneration/Dynamic/PD_MG_DynamicMap.h"
#include "PATD_Server/MapInfo/MapInstantiation/MapInstantiatorActor.h"
#include "PATD_Server/Structs/PD_ServerEnums.h"
#include "PATD_Server/GM_Game/PD_GM_EnemyManager.h"
#include "PATD_Server/GM_Game/PD_GM_GameManager.h"
#include "PATD_Server/GM_Game/LogicCharacter/PD_GM_LogicCharacter.h"
#include "PATD_Server/Actors/Enemies/PD_E_EnemyCharacter.h"
//include of forward declaration
#include "MapGeneration/PD_MG_LogicPosition.h"
#include "Actors/PD_GenericController.h"

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

TArray<PD_MG_LogicPosition> PD_GM_MapManager::GetSpawnPoint()
{

	TArray<PD_MG_LogicPosition> spawnPoints;
	return  spawnPoints;
}

#pragma endregion 




#pragma region INSTANTIATE MAP


void PD_GM_MapManager::InstantiateMap()
{
	UE_LOG(LogTemp, Log, TEXT("MapManager::InstantiateMap "));

	InstantiateStaticMap();
	InstantiateDynamicMap();

}


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


void PD_GM_MapManager::InstantiateDynamicMap() {
	ECharacterType enemyType;

	UE_LOG(LogTemp, Warning, TEXT("PD_GM_MapManager::InstantiateDynamicMap - Enemies Num %d"), _GAMEMANAGER->enemyManager->GetEnemies().Num());

	///necesitamos comprobar ya el ID
	for (int i = 0; i < DynamicMapRef->GetLogicPositions().Num(); i++) {

		enemyType = DynamicMapRef->getEnemies()[*DynamicMapRef->GetLogicPositions()[i]].type_Character; ///Cogemos el tipo

		switch (enemyType)
		{
			case ECharacterType::Archer: 
			{
				APD_E_EnemyCharacter* charac = instantiator->InstantiateArcher(DynamicMapRef->GetLogicPositions()[i]);
				PD_GM_LogicCharacter* logicCha = new PD_GM_LogicCharacter();
				logicCha->SetIsPlayer(false);
				logicCha->SetTypeCharacter(DynamicMapRef->getEnemies()[*DynamicMapRef->GetLogicPositions()[i]].type_Character);
				logicCha->SetIDCharacter(DynamicMapRef->getEnemies()[*DynamicMapRef->GetLogicPositions()[i]].ID_Character);
				logicCha->SetCharacterBP(charac);
				logicCha->SetController( Cast<APD_GenericController>(charac->GetController()) );

				///SETEAR AQUI TODOS LOS STATS- WEAPONS- SKILLS DE CADA TIOPO DE ENEMIGO ENE SU LOGIC CHARACTER

				_GAMEMANAGER->enemyManager->AddEnemy(logicCha);
				break;
			}
			case ECharacterType::Zombie: 
			{
				APD_E_EnemyCharacter* charac = instantiator->InstantiateZombie(DynamicMapRef->GetLogicPositions()[i]);
				PD_GM_LogicCharacter* logicCha = new PD_GM_LogicCharacter();
				logicCha->SetIsPlayer(false);
				logicCha->SetTypeCharacter(DynamicMapRef->getEnemies()[*DynamicMapRef->GetLogicPositions()[i]].type_Character);
				logicCha->SetIDCharacter(DynamicMapRef->getEnemies()[*DynamicMapRef->GetLogicPositions()[i]].ID_Character);
				logicCha->SetCharacterBP(charac);
				logicCha->SetController(Cast<APD_GenericController>(charac->GetController()));

				///SETEAR AQUI TODOS LOS STATS- WEAPONS- SKILLS DE CADA TIOPO DE ENEMIGO ENE SU LOGIC CHARACTER

				_GAMEMANAGER->enemyManager->AddEnemy(logicCha);
				break;
			}
		}
	}
}

#pragma endregion