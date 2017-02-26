// Fill out your copyright notice in the Description page of Project Settings.

#include "PATD_Server.h"
#include "PD_GM_MapManager.h"
#include "PATD_Server/MapGeneration/Static/PD_MG_StaticMap.h"
#include "PATD_Server/MapGeneration/Dynamic/PD_MG_DynamicMap.h"
#include "PATD_Server/MapInfo/MapInstantiation/MapInstantiatorActor.h"

//include of forward declaration
#include "MapGeneration/PD_MG_LogicPosition.h"
#include "PATD_Server/Actors/Players/PD_PLY_GenericCharacter.h"

PD_GM_MapManager::PD_GM_MapManager()
{
}

PD_GM_MapManager::~PD_GM_MapManager()
{
}

#pragma region GET INFO OF THE MAP

bool PD_GM_MapManager::getGenericCharacterAt(PD_MG_LogicPosition* logpos, APD_PLY_GenericCharacter* genCharacter) { return false; }
bool PD_GM_MapManager::getPlayerAt(PD_MG_LogicPosition* logpos, APD_PLY_GenericCharacter* genCharacter) { return false; }
bool PD_GM_MapManager::getEnemyAt(PD_MG_LogicPosition* logpos, APD_PLY_GenericCharacter* genCharacter) { return false; }

AActor* PD_GM_MapManager::getInteractuableAt(PD_MG_LogicPosition* logpos) { return 0; }


FVector* PD_GM_MapManager::LogicToWorldPosition(PD_MG_LogicPosition* pos) {
	return new FVector(-1.0f * pos->GetX()*100.0f, pos->GetY() * 100.0f, 0.f);
}

PD_MG_LogicPosition* PD_GM_MapManager::WorldToLogicPosition(FVector* pos) {
	float x, y;

	x = -1.0f * pos->X / 100.f;
	y = pos->X / 100.f;

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


void PD_GM_MapManager::InstantiateDynamicMap() {
	EEnemiesType enemyId;
	for (int i = 0; i < DynamicMapRef->GetLogicPositions().Num(); i++) {

		enemyId = DynamicMapRef->GetXYMap()[*DynamicMapRef->GetLogicPositions()[i]];

		switch (enemyId) {
		case EEnemiesType::Archer:
			DynamicMapRef->UpdateActor(instantiator->InstantiateArcher(DynamicMapRef->GetLogicPositions()[i]), DynamicMapRef->GetLogicPositions()[i]);///instancia el objeto fisico en el lógico
			break;

		case EEnemiesType::Zombie:
			DynamicMapRef->UpdateActor(instantiator->InstantiateZombie(DynamicMapRef->GetLogicPositions()[i]), DynamicMapRef->GetLogicPositions()[i]);///instancia el objeto fisico en el lógico
			break;
		}
		//if (dynamicMap->GetEnemyId(dynamicMap->GetXYMap()[*dynamicMap->GetLogicPositions()[i]])=="id_zombie") {
		//parserActor->InstantiateArcher(dynamicMap->GetLogicPositions()[i]);
		//UE_LOG(LogTemp, Error, TEXT("Entra :%s !!!!"), *enemyId);
		//}
		//dynamicMap->GetLogicPositions

	}
}

#pragma endregion