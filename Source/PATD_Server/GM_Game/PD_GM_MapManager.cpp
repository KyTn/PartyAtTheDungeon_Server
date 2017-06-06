// Fill out your copyright notice in the Description page of Project Settings.

#include "PATD_Server.h"
#include "PD_GM_MapManager.h"
#include "MapGeneration/Static/PD_MG_StaticMap.h"
#include "MapGeneration/Dynamic/PD_MG_DynamicMap.h"
#include "MapInfo/MapInstantiation/MapInstantiatorActor.h"
#include "Structs/PD_ServerEnums.h"
#include "GM_Game/PD_GM_EnemyManager.h"
#include "GM_Game/PD_GM_GameManager.h"
#include "PD_PlayersManager.h"
#include "GM_Game/LogicCharacter/PD_GM_LogicCharacter.h"
#include "Actors/PD_E_Character.h"
#include "Actors/PD_SplineActors.h"
#include "GM_Game/PD_GM_SplineManager.h"
#include"PD_ServerGameInstance.h"
#include "Actors/Enemies/PD_AIController.h"
//include of forward declaration
#include "MapGeneration/PD_MG_LogicPosition.h"
#include "Actors/PD_GenericController.h"
#include "MapInfo/PD_MM_MapInfo.h"
#include "Actors/PD_E_ElementActor.h"
#include "Actors/Interactuables/PD_E_Door.h"
#include "Actors/MapElements/PD_E_WallActor.h"


PD_GM_MapManager::PD_GM_MapManager()
{
}

PD_GM_MapManager::~PD_GM_MapManager()
{
}



void PD_GM_MapManager::Init() {


	DynamicMapRef = new PD_MG_DynamicMap();

	// Ya tengo la info del mapa cargada por el Struct o por las refs del estatico y dinamico
	MapInfo = new PD_MM_MapInfo(this);
}


#pragma region GET RAW INFO FROM STATIC

bool PD_GM_MapManager::IsLogicPositionAWall(PD_MG_LogicPosition logpos)
{

	if (MapInfo->allLogicPos.Contains(logpos)) {
		for (int i = 0; i < MapInfo->rooms.Num(); i++) {
			if (MapInfo->rooms[i]->LogicWallPosInRoom.Contains(logpos)) {
				return true;
			}
		}

	}
	return false;
}

bool PD_GM_MapManager::IsLogicPositionATile(PD_MG_LogicPosition logpos)
{
	if (MapInfo->roomByLogPos.Contains(logpos)) {
		PD_MM_Room *r = MapInfo->roomByLogPos[logpos];
		if (r->PropsAndTilesInRoomByLogicPosition.Contains(logpos) && r->tiles.Contains(logpos))
			return true;
	}
	return false;
}

bool PD_GM_MapManager::IsLogicPositionAProp(PD_MG_LogicPosition logpos)
{
	if (MapInfo->roomByLogPos.Contains(logpos)) {
		PD_MM_Room *r = MapInfo->roomByLogPos[logpos];
		if (r->PropsAndTilesInRoomByLogicPosition.Contains(logpos) && !r->tiles.Contains(logpos))
			return true;
	}
	return false;
}

bool PD_GM_MapManager::IsLogicPositionADoor(PD_MG_LogicPosition logpos)
{
	if (MapInfo->allLogicPos.Contains(logpos)) {
		for (int i = 0; i < MapInfo->rooms.Num(); i++) {
			if (MapInfo->rooms[i]->LogicDoorPosInRoom.Contains(logpos)) {
				return true;
			}
		}

	}
	return false;
}

bool PD_GM_MapManager::IsLogicPositionAEnemy(PD_MG_LogicPosition logpos) {
	for (PD_GM_LogicCharacter* logicCharacter : _GAMEMANAGER->enemyManager->GetEnemies()) {
		if (logicCharacter->GetCurrentLogicalPosition() == logpos) return true;
	}
	return false;
}


#pragma endregion



#pragma region GET INFO OF THE MAP


bool PD_GM_MapManager::IsTherePlayer(uint32 x, uint32 y) {
	PD_MG_LogicPosition logpos;
	logpos.SetX(x);
	logpos.SetY(y);
	if (DynamicMapRef->getEnemies().Contains(logpos))
		if (DynamicMapRef->getEnemies()[logpos]->type_Character == ECharacterType::Player)
			return true;
	return false;
}

TArray<PD_MG_LogicPosition> PD_GM_MapManager::GetSpawnPoints() {
	UE_LOG(LogTemp, Warning, TEXT("PD_GM_MapManager::GetSpawnPoints() - Num Rooms: %d"), MapInfo->rooms.Num());
	/*
	for (int i = 0; i < MapInfo->rooms.Num(); i++) {
		if (MapInfo->rooms[i].IsSpawnRoom) {
			UE_LOG(LogTemp, Warning, TEXT("PD_GM_MapManager::GetSpawnPoints() - Hay una habitacion marcada como spawn"));
			return MapInfo->rooms[i].LogicPosInRoom;
		}
	}
	*/
	//UE_LOG(LogTemp, Warning, TEXT("PD_GM_MapManager::GetSpawnPoints() -  Room %d num spawnpoints: %d"), MapInfo->SpawnRoom->GetIDRoom(), MapInfo->rooms[MapInfo->SpawnRoom->GetIDRoom()].LogicPosInRoom.Num());
	return MapInfo->rooms[MapInfo->SpawnRoom->GetIDRoom()]->LogicPosInRoom;
	
	//UE_LOG(LogTemp, Warning, TEXT("PD_GM_MapManager::GetSpawnPoints() -  IDSPAN   %d"), MapInfo->SpawnRoomIndex);

	/*TArray<PD_MG_LogicPosition> ret = TArray<PD_MG_LogicPosition>();

	for (int i = 0; i < MapInfo->SpawnRoom->LogicPosInRoom.Num(); i++) {

		if (MapInfo->mapManager->IsLogicPositionATile(MapInfo->SpawnRoom->LogicPosInRoom[i]))
		{
			ret.Add(MapInfo->SpawnRoom->LogicPosInRoom[i]);
		}
	}

	return ret;*/

}

AActor* PD_GM_MapManager::getInteractuableAt(PD_MG_LogicPosition logpos) { return 0; }


const FVector PD_GM_MapManager::LogicToWorldPosition(PD_MG_LogicPosition pos) {
	return pos.ToWorldPosition();
}

PD_MG_LogicPosition PD_GM_MapManager::WorldToLogicPosition(FVector pos) {
	float x, y;

	x = -1.0f * pos.X / 100.f;
	y = pos.Y / 100.f;

	return PD_MG_LogicPosition((int)roundf(x), (int)roundf(y));
}



TArray<PD_MG_LogicPosition> PD_GM_MapManager::Get_LogicPosition_Adyacents_To(PD_MG_LogicPosition logPos) {

	return logPos.GetAdjacentsFromList(MapInfo->allLogicPos);
}


TArray<PD_MG_LogicPosition> PD_GM_MapManager::Get_LogicPosition_Diagonals_And_Adyacents_To(PD_MG_LogicPosition logPos) {
	if (MapInfo)
	{
		UE_LOG(LogTemp, Warning, TEXT("PD_GM_MapManager: Get_LogicPosition_Diagonals_And_Adyacents_To hay MapInfo"));
		return logPos.GetDiagonalsAndAdjacentsFromList(MapInfo->allLogicPos);
	}
	else 
	{
		UE_LOG(LogTemp, Warning, TEXT("PD_GM_MapManager: Get_LogicPosition_Diagonals_And_Adyacents_To NO HAY MapInfo"));
		return TArray<PD_MG_LogicPosition>();
	}
}

#pragma endregion




#pragma region INSTANTIATE MAP


void PD_GM_MapManager::InstantiateMap()
{
	UE_LOG(LogTemp, Log, TEXT("MapManager::InstantiateMap "));

	InstantiateRoomAndAdj(MapInfo->SpawnRoom->IDRoom);
	InstantiateDynamicMap();

}

void PD_GM_MapManager::InstantiateRoomAndAdj(uint8 id) {

	if (!MapInfo->mapAdj.Contains(id)) {

		UE_LOG(LogTemp, Log, TEXT("MapManager::InstantiateRoomAndAdj - El mapAdj no contiene el id %d "), id);
	}
	PD_MM_Room* room = MapInfo->roomByIDRoom[id];
	room->IsOpen = true;
	if (!room->IsInstantiated) {
		TArray<PD_MG_LogicPosition> lp;
		room->PropsAndTilesInRoomByLogicPosition.GenerateKeyArray(lp);
		for (int j = 0; j < lp.Num(); j++)///Instanciamos los tiles de una habitacion.
		{
			InstantiateMapElementBySkin(room->mapSkin, room->PropsAndTilesInRoomByLogicPosition[lp[j]], lp[j]);
		}
		for (int j = 0; j < room->LogicWallPosInRoom.Num(); j++)///Instanciamos los tiles de una habitacion.
		{
			InstantiateWallBySkin(room->mapSkin, room->LogicWallPosInRoom[j]);
		}
		room->IsInstantiated = true;
	}

	TArray<uint8> adj = MapInfo->mapAdj[id];
	for (int i = 0; i < adj.Num(); i++)//Recorreriamos el grafo de las adyacentes
	{
		if (!MapInfo->rooms[adj[i]]->IsInstantiated) {
			TArray<PD_MG_LogicPosition> lp;
			MapInfo->rooms[adj[i]]->PropsAndTilesInRoomByLogicPosition.GenerateKeyArray(lp);
			for (int j = 0; j < lp.Num(); j++)///Instanciamos los tiles de una habitacion.
			{
				InstantiateMapElementBySkin(MapInfo->rooms[adj[i]]->mapSkin, MapInfo->rooms[adj[i]]->PropsAndTilesInRoomByLogicPosition[lp[j]], lp[j]);
			}
			for (int j = 0; j < MapInfo->rooms[adj[i]]->LogicWallPosInRoom.Num(); j++)///Instanciamos los muros de una habitacion.
			{
				InstantiateWallBySkin(MapInfo->rooms[adj[i]]->mapSkin, MapInfo->rooms[adj[i]]->LogicWallPosInRoom[j]);
			}
			UE_LOG(LogTemp, Error, TEXT("PD_GM_MapManager::InstantiateRoomAndAdj - door in room %d -> %d"), MapInfo->rooms[adj[i]]->IDRoom, MapInfo->rooms[adj[i]]->LogicDoorPosInRoom.Num());

			for (int j = 0; j < MapInfo->rooms[adj[i]]->LogicDoorPosInRoom.Num(); j++)///Instanciamos las puertas de una habitacion.
			{
				if (!MapInfo->IsDoorInstantiatedAt(MapInfo->rooms[adj[i]]->LogicDoorPosInRoom[j])) {
					UE_LOG(LogTemp, Error, TEXT("PD_GM_MapManager::InstantiateRoomAndAdj - InstantiateDoor at (%d,%d)"), MapInfo->rooms[adj[i]]->LogicDoorPosInRoom[j].GetX(), MapInfo->rooms[adj[i]]->LogicDoorPosInRoom[j].GetY());
					InstantiateDoor(MapInfo->rooms[adj[i]]->LogicDoorPosInRoom[j], MapInfo->rooms[adj[i]]->DoorInfoInRoomByLogicPosition[MapInfo->rooms[adj[i]]->LogicDoorPosInRoom[j]]);
				}
			}
			MapInfo->rooms[adj[i]]->IsInstantiated = true;
		}
	}

}


void PD_GM_MapManager::InstantiateMapElementBySkin(MapSkinType mapSkin, StaticMapElement element, PD_MG_LogicPosition lp) {

	UE_LOG(LogTemp, Error, TEXT("PD_GM_MapManager::InstantiateMapElementBySkin at (%d,%d)"), lp.GetX(), lp.GetY());
	APD_E_ElementActor* actorElement;
	switch (mapSkin) {
		case MapSkinType::DUNGEON_NORMAL: {
			switch (element) {
				case StaticMapElement::NORMAL_TILE: {
					actorElement = instantiator->InstantiateTile(lp);
					actorElement->SetMaterialSkin(MapSkinType::DUNGEON_NORMAL);
					MapInfo->AddTile(lp, actorElement);
					break;
				}
				case StaticMapElement::SPECIAL_TILE: {
					actorElement = instantiator->Instantiate_Dungeon_SpecialTile(lp);
					actorElement->SetMaterialSkin(MapSkinType::DUNGEON_NORMAL);
					break;
				}
				case StaticMapElement::EMPTY: {
					break;
				}
				case StaticMapElement::PROP_CHEST: {
					instantiator->Instantiate_Dungeon_Prop_Treasure_01(lp);
					break;
				}
				case StaticMapElement::TREE_OR_COLUMN_00: {
					instantiator->Instantiate_Dungeon_Prop_Column_01(lp);
					break;
				}
				case StaticMapElement::TREE_OR_COLUMN_01: {
					instantiator->Instantiate_Dungeon_Prop_Column_02_1(lp);
					break;
				}
				case StaticMapElement::TREE_OR_COLUMN_02: {
					instantiator->Instantiate_Dungeon_Prop_Column_03(lp);
					break;
				}
				case StaticMapElement::OBSTRUCTION_00: {
					

					instantiator->Instantiate_Dungeon_Prop_Obstruction_02(lp);
					break;
				}
			}
			break;
		}
		case MapSkinType::GARDEN: {
			switch (element) {
				case StaticMapElement::NORMAL_TILE: {
					actorElement = instantiator->InstantiateTile(lp);
					actorElement->SetMaterialSkin(MapSkinType::GARDEN);
					MapInfo->AddTile(lp, actorElement);
					break;
				}
				case StaticMapElement::SPECIAL_TILE: {
					actorElement = instantiator->Instantiate_Dungeon_SpecialTile(lp);
					actorElement->SetMaterialSkin(MapSkinType::DUNGEON_NORMAL);
					break;
				}
				case StaticMapElement::EMPTY: {
					break;
				}
				case StaticMapElement::PROP_CHEST: {
					instantiator->Instantiate_Garden_Prop_Treasure_01(lp);
					break;
				}
				case StaticMapElement::SMALL_CHEST: {
					break;
				}
				case StaticMapElement::TREE_OR_COLUMN_00: {
					instantiator->Instantiate_Garden_Prop_Tree_01(lp);
					break;
				}
				case StaticMapElement::TREE_OR_COLUMN_01: {
					break;
				}
				case StaticMapElement::TREE_OR_COLUMN_02: {
					break;
				}
				case StaticMapElement::OBSTRUCTION_00: {
					instantiator->Instantiate_Garden_Prop_Bush_01(lp);
					break;
				}
			}
			break;
		}
		case MapSkinType::SACRIFICE: {
			switch (element) {
				case StaticMapElement::NORMAL_TILE: {
					actorElement = instantiator->InstantiateTile(lp);
					actorElement->SetMaterialSkin(MapSkinType::SACRIFICE);
					MapInfo->AddTile(lp, actorElement);
					break;
				}
				case StaticMapElement::SPECIAL_TILE: {
					actorElement = instantiator->Instantiate_Dungeon_SpecialTile(lp);
					actorElement->SetMaterialSkin(MapSkinType::DUNGEON_NORMAL);
					break;
				}
				case StaticMapElement::EMPTY: {
					break;
				}
				case StaticMapElement::PROP_CHEST: {
					instantiator->Instantiate_Dungeon_Prop_Treasure_01(lp);
					break;
				}
				case StaticMapElement::SMALL_CHEST: {
					break;
				}
				case StaticMapElement::TREE_OR_COLUMN_00: {
					instantiator->Instantiate_Sacrifice_Prop_Jail_01(lp);
					break;
				}
				case StaticMapElement::TREE_OR_COLUMN_01: {
					break;
				}
				case StaticMapElement::TREE_OR_COLUMN_02: {
					break;
				}
				case StaticMapElement::OBSTRUCTION_00: {
					instantiator->Instantiate_Sacrifice_Prop_Obstruction_12_2(lp);
					break;
				}
			}
			break;
		}
		case MapSkinType::BOSS: {
			switch (element) {
				case StaticMapElement::NORMAL_TILE: {
					actorElement = instantiator->InstantiateTile(lp);
					actorElement->SetMaterialSkin(MapSkinType::BOSS);
					MapInfo->AddTile(lp, actorElement);
					break;
				}
				case StaticMapElement::SPECIAL_TILE: {
					actorElement = instantiator->Instantiate_Dungeon_SpecialTile(lp);
					actorElement->SetMaterialSkin(MapSkinType::DUNGEON_NORMAL);
					break;
				}
				case StaticMapElement::EMPTY: {
					break;
				}
				case StaticMapElement::PROP_CHEST: {
					instantiator->Instantiate_Dungeon_Prop_Treasure_01(lp);
					break;
				}
				case StaticMapElement::SMALL_CHEST: {
					break;
				}
				case StaticMapElement::TREE_OR_COLUMN_00: {
					instantiator->Instantiate_Dungeon_Prop_Column_04_1(lp);
					break;
				}
				case StaticMapElement::TREE_OR_COLUMN_01: {
					break;
				}
				case StaticMapElement::TREE_OR_COLUMN_02: {
					break;
				}
				case StaticMapElement::OBSTRUCTION_00: {
					instantiator->Instantiate_Dungeon_Prop_Obstruction_02(lp);
					break;
				}
			}
			break;
		}
		case MapSkinType::LIBRARY: {
			switch (element) {
				case StaticMapElement::NORMAL_TILE: {
					actorElement = instantiator->InstantiateTile(lp);
					actorElement->SetMaterialSkin(MapSkinType::LIBRARY);
					MapInfo->AddTile(lp, actorElement);
					break;
				}
				case StaticMapElement::SPECIAL_TILE: {
					actorElement = instantiator->Instantiate_Dungeon_SpecialTile(lp);
					actorElement->SetMaterialSkin(MapSkinType::DUNGEON_NORMAL);
					break;
				}
				case StaticMapElement::EMPTY: {
					break;
				}
				case StaticMapElement::PROP_CHEST: {
					instantiator->Instantiate_Library_Prop_Treasure_01(lp);
					break;
				}
				case StaticMapElement::SMALL_CHEST: {
					break;
				}
				case StaticMapElement::TREE_OR_COLUMN_00: {
					instantiator->Instantiate_Library_Prop_Column_04_2(lp);
					break;
				}
				case StaticMapElement::TREE_OR_COLUMN_01: {
					break;
				}
				case StaticMapElement::TREE_OR_COLUMN_02: {
					break;
				}
				case StaticMapElement::OBSTRUCTION_00: {
					instantiator->Instantiate_Library_Prop_Obstruction_12_1(lp);
					break;
				}
			}
			break;
		}
	}
}


void PD_GM_MapManager::InstantiateWallBySkin(MapSkinType mapSkin, PD_MG_LogicPosition lp) {
	UE_LOG(LogTemp, Error, TEXT("PD_GM_MapManager::InstantiateWallBySkin at (%d,%d)"), lp.GetX(), lp.GetY());
	APD_E_WallActor* actorElement;
	switch (mapSkin) {
	case MapSkinType::DUNGEON_NORMAL: {
		actorElement = instantiator->InstantiateWall(lp);
		actorElement->SetMaterialSkin(lp);
		MapInfo->AddWall(lp, actorElement);
		break;
	}
	case MapSkinType::GARDEN: {
		actorElement = instantiator->InstantiateWall(lp);
		actorElement->SetMaterialSkin(lp);
		MapInfo->AddWall(lp, actorElement);
		break;
	}
	case MapSkinType::LIBRARY: {
		actorElement = instantiator->InstantiateWall(lp);
		actorElement->SetMaterialSkin(lp);
		MapInfo->AddWall(lp, actorElement);
		break;
	}
	case MapSkinType::SACRIFICE: {
		actorElement = instantiator->InstantiateWall(lp);
		actorElement->SetMaterialSkin(lp);
		MapInfo->AddWall(lp, actorElement);
		break;
	}
	case MapSkinType::BOSS: {
		actorElement = instantiator->InstantiateWall(lp);
		actorElement->SetMaterialSkin(lp);
		MapInfo->AddWall(lp, actorElement);
		break;
	}
	}


}


void PD_GM_MapManager::InstantiateDoor(PD_MG_LogicPosition lp, PD_MM_DoorInfo* doorInfo) {
	UE_LOG(LogTemp, Error, TEXT("PD_GM_MapManager::InstantiateDoor at (%d,%d)"), lp.GetX(), lp.GetY());
	APD_E_Door* doorElement = instantiator->InstantiateDoor(lp);

	TArray<APD_E_Interactuable*> interact;
	MapInfo->interactuableActorByID.GenerateValueArray(interact);
	doorElement->Set_DoorInfo(interact, doorInfo);

	MapInfo->AddDoor_WithoutLink(lp, doorElement);
	doorElement->ChangeRotationToReal(lp);
}


void PD_GM_MapManager::InstantiateDynamicMap() {
	
	UE_LOG(LogTemp, Warning, TEXT("PD_GM_MapManager::InstantiateDynamicMap - Players Num %d"), _GAMEMANAGER->playersManager->GetNumPlayers());

	for (int i = 0; i < _GAMEMANAGER->playersManager->GetNumPlayers(); i++)
	{
		FOutputDeviceNull ar;

		_GAMEMANAGER->playersManager->GetDataPlayers()[i]->logic_Character->SetMapManager(this);

		_GAMEMANAGER->playersManager->GetDataPlayers()[i]->logic_Character->SetCharacterBP(instantiator->InstantiatePlayer(
			_GAMEMANAGER->playersManager->GetDataPlayers()[i]->logic_Character->GetCurrentLogicalPosition()));

		Cast<APD_E_Character>(_GAMEMANAGER->playersManager->GetDataPlayers()[i]->logic_Character->GetCharacterBP())->SetLogicCharacter(
			_GAMEMANAGER->playersManager->GetDataPlayers()[i]->logic_Character);

		_GAMEMANAGER->playersManager->GetDataPlayers()[i]->logic_Character->SetController(Cast<APD_GenericController>(
			_GAMEMANAGER->playersManager->GetDataPlayers()[i]->logic_Character->GetCharacterBP()->GetController()));

		switch (_GAMEMANAGER->playersManager->GetDataPlayers()[i]->logic_Character->GetWeapon()->TypeWeapon)
		{
		case 11:
			_GAMEMANAGER->playersManager->GetDataPlayers()[i]->logic_Character->GetController()->SetTypeCharanimation(0);
			break;
		case 12:
			_GAMEMANAGER->playersManager->GetDataPlayers()[i]->logic_Character->GetController()->SetTypeCharanimation(0);
			break;
		case 13:
			_GAMEMANAGER->playersManager->GetDataPlayers()[i]->logic_Character->GetController()->SetTypeCharanimation(0);
			break;
		case 21:
			_GAMEMANAGER->playersManager->GetDataPlayers()[i]->logic_Character->GetController()->SetTypeCharanimation(0);
			break;
		case 22:
			_GAMEMANAGER->playersManager->GetDataPlayers()[i]->logic_Character->GetController()->SetTypeCharanimation(0);
			break;
		case 23:
			_GAMEMANAGER->playersManager->GetDataPlayers()[i]->logic_Character->GetController()->SetTypeCharanimation(1);
			break;
		case 31:
			_GAMEMANAGER->playersManager->GetDataPlayers()[i]->logic_Character->GetController()->SetTypeCharanimation(0);
			break;
		case 32:
			_GAMEMANAGER->playersManager->GetDataPlayers()[i]->logic_Character->GetController()->SetTypeCharanimation(0);
			break;
		case 33:
			_GAMEMANAGER->playersManager->GetDataPlayers()[i]->logic_Character->GetController()->SetTypeCharanimation(0);
			break;
		default:
			break;
		}
		UE_LOG(LogTemp, Warning, TEXT("PD_GM_MapManager::weapon  - %d "), _GAMEMANAGER->playersManager->GetDataPlayers()[i]->logic_Character->GetSkin()->ID_SkinHead);

		const FString command = FString::Printf(TEXT("ChangeSkin %d"), _GAMEMANAGER->playersManager->GetDataPlayers()[i]->logic_Character->GetSkin()->ID_SkinHead);
		if (_GAMEMANAGER->playersManager->GetDataPlayers()[i]->logic_Character->GetCharacterBP()->CallFunctionByNameWithArguments(*command, ar, NULL, true))
		{
			UE_LOG(LogTemp, Warning, TEXT("PD_GM_MapManager::InstantiateDynamicMap -- EXITO EN LLAMAR A LA FUNCION"));
		}
		else {
			UE_LOG(LogTemp, Error, TEXT("PD_GM_MapManager::InstantiateDynamicMap - EEROR EN LLAMATR A LA FUNCION"));
		}


		const FString command2 = FString::Printf(TEXT("ChangeWeapon %d"), _GAMEMANAGER->playersManager->GetDataPlayers()[i]->logic_Character->GetWeapon()->TypeWeapon);
		if (_GAMEMANAGER->playersManager->GetDataPlayers()[i]->logic_Character->GetCharacterBP()->CallFunctionByNameWithArguments(*command2, ar, NULL, true))
		{
			UE_LOG(LogTemp, Warning, TEXT("PD_GM_MapManager::InstantiateDynamicMap -- EXITO EN LLAMAR A LA FUNCION 2"));
		}
		else {
			UE_LOG(LogTemp, Error, TEXT("PD_GM_MapManager::InstantiateDynamicMap - EEROR EN LLAMATR A LA FUNCION 2"));
		}
		
		///actualizamos la referencia del BP

	}
	InstantiateEnemies();

	//Instancacion del Jefe FINAL
	/*
	APD_E_Character* charac = nullptr;
	PD_GM_LogicCharacter* logicCha = nullptr;
	UPD_ServerGameInstance* SGI = Cast<UPD_ServerGameInstance>(instantiator->GetGameInstance());

	PD_MG_LogicPosition logicPosition = _GAMEMANAGER->enemyManager->GetEnemies()[0]->GetCurrentLogicalPosition();
	PD_MG_LogicPosition logicPositionInst = PD_MG_LogicPosition(logicPosition.GetX(), logicPosition.GetY() + 1);

	charac = instantiator->InstantiateOrcBoss(logicPositionInst);

	logicCha = new PD_GM_LogicCharacter();
	logicCha->SetIsPlayer(false);
	logicCha->SetTypeCharacter(ECharacterType::OrcBoss);
	logicCha->SetIDCharacter("1");
	UE_LOG(LogTemp, Log, TEXT("PD_GM_MapManager::Instanciando de risas id: %s"), *logicCha->GetIDCharacter());
	logicCha->SetCharacterBP(charac);
	logicCha->SetController(Cast<APD_AIController>(charac->GetController()));
	logicCha->SetCurrentLogicalPosition(logicPositionInst);

	///SETEAR AQUI TODOS LOS STATS- WEAPONS- SKILLS DE CADA TIOPO DE ENEMIGO ENE SU LOGIC CHARACTER
	if (charac)
	{
		UE_LOG(LogTemp, Log, TEXT("PD_GM_MapManager::UPD_ServerGameInstance"));

		//		UPD_ServerGameInstance* SGI = Cast<UPD_ServerGameInstance>(charac->GetGameInstance());
		if (SGI)
		{
			//Weapon
			int id_weapon, classWeapon, typeWeapon, damage, range;
			SGI->LoadWeaponSpecificDataByType(charac->weapon.GetValue(), id_weapon, classWeapon, typeWeapon, damage, range);
			logicCha->GetWeapon()->ID_Weapon = id_weapon;
			logicCha->GetWeapon()->ClassWeapon = classWeapon;
			logicCha->GetWeapon()->TypeWeapon = typeWeapon;
			logicCha->GetWeapon()->DMWeapon = damage;
			logicCha->GetWeapon()->RangeWeapon = range;

			//Skills
			//PASIVAS
			for (int z = 0; z < charac->pasiveSkillList.Num(); z++)
			{
				FStructSkill skillPasAdded = FStructSkill();
				int weaponR, APSkill, CDSkill, targetSkill, Range;
				SGI->LoadSkillSpecificDataByType(1, (int)charac->pasiveSkillList[z].GetValue(), skillPasAdded.name_Skill, skillPasAdded.description, weaponR, APSkill, CDSkill, targetSkill, Range);
				skillPasAdded.ID_Skill = (int)charac->pasiveSkillList[z].GetValue();
				skillPasAdded.weaponRequired = weaponR;
				skillPasAdded.AP = APSkill;
				skillPasAdded.CD = CDSkill;
				skillPasAdded.currentCD = CDSkill;
				skillPasAdded.range = Range;
				skillPasAdded.target = targetSkill;
				logicCha->GetSkills()->listPasiveSkills.Add(skillPasAdded);
			}
			//ACTIVAS
			for (int j = 0; j < charac->activeSkillList.Num(); j++)
			{
				FStructSkill skillActAdded = FStructSkill();
				int weaponR, APSkill, CDSkill, targetSkill, Range;
				SGI->LoadSkillSpecificDataByType(0, (int)charac->activeSkillList[j].GetValue(), skillActAdded.name_Skill, skillActAdded.description, weaponR, APSkill, CDSkill, targetSkill, Range);
				skillActAdded.ID_Skill = (int)charac->activeSkillList[j].GetValue();
				skillActAdded.weaponRequired = weaponR;
				skillActAdded.AP = APSkill;
				skillActAdded.CD = CDSkill;
				skillActAdded.currentCD = CDSkill;
				skillActAdded.range = Range;
				skillActAdded.target = targetSkill;
				logicCha->GetSkills()->listActiveSkills.Add(skillActAdded);
			}
		}
	}


	//STATS
	logicCha->SetBasicStats(charac->GetPOD(), charac->GetAGI(), charac->GetDES(), charac->GetCON(), charac->GetPER(), charac->GetMAL());
	logicCha->SetInitBaseStats(charac->GetBaseHP(), charac->GetBaseDamage(), charac->GetBaseAP());

	//IA
	if (charac->generateRandomPersonality) {
		//hardcodeado porque no admite c++ saber el tama�o de un enum
		int randPersonality = FMath::RandRange(0, 3);
		logicCha->SetIAPersonality(EIAPersonality(randPersonality));

	}
	else {
		logicCha->SetIAPersonality(charac->IAPersonality.GetValue());

	}

	logicCha->SetTotalStats();

	charac->SetLogicCharacter(logicCha);

	_GAMEMANAGER->enemyManager->AddEnemy(logicCha);


	*/

}

void PD_GM_MapManager::InstantiateEnemies() {
	ECharacterType enemyType;

	bool unaVez = false;

	UE_LOG(LogTemp, Warning, TEXT("PD_GM_MapManager::InstantiateDynamicMap - Numero de enemigos enemigos %d"), DynamicMapRef->GetLogicPositions().Num());
	for (int i = 0; i < DynamicMapRef->GetLogicPositions().Num(); i++) {

		UE_LOG(LogTemp, Warning, TEXT("PD_GM_MapManager::InstantiateDynamicMap is %s"), (DynamicMapRef->getEnemies()[DynamicMapRef->GetLogicPositions()[i]]->isInstantiated ? TEXT("True") : TEXT("False")));

		if (MapInfo->roomByLogPos[DynamicMapRef->GetLogicPositions()[i]]->IsInstantiated && !DynamicMapRef->getEnemies()[DynamicMapRef->GetLogicPositions()[i]]->isInstantiated) {
			UE_LOG(LogTemp, Warning, TEXT("PD_GM_MapManager::InstantiateDynamicMap - Instanciando enemigos %d"), i);
			
			DynamicMapRef->UpdateIsInstantiatedEnemy(DynamicMapRef->GetLogicPositions()[i], true);
			//DynamicMapRef->getEnemies()[DynamicMapRef->GetLogicPositions()[i]].isInstantiated = true;

			UE_LOG(LogTemp, Warning, TEXT("PD_GM_MapManager::InstantiateDynamicMap DESPUES is %s"), (DynamicMapRef->getEnemies()[DynamicMapRef->GetLogicPositions()[i]]->isInstantiated ? TEXT("True") : TEXT("False")));

			enemyType = DynamicMapRef->getEnemies()[DynamicMapRef->GetLogicPositions()[i]]->type_Character; ///Cogemos el tipo
			APD_E_Character* charac = nullptr;
			PD_GM_LogicCharacter* logicCha = nullptr;
			bool enemyInstantiated = false;

			switch (enemyType)
			{
			case ECharacterType::OrcBow:
			{
				charac = instantiator->InstantiateOrcBow(DynamicMapRef->GetLogicPositions()[i]);
				if (charac)
					enemyInstantiated = true;

				break;
			}
			case ECharacterType::OrcGuns:
			{
				charac = instantiator->InstantiateOrcGuns(DynamicMapRef->GetLogicPositions()[i]);
				if (charac)
					enemyInstantiated = true;

				break;
			}
			case ECharacterType::OrcMelee:
			{
				charac = instantiator->InstantiateOrcMelee(DynamicMapRef->GetLogicPositions()[i]);
				if (charac)
					enemyInstantiated = true;

				break;
			}
			case ECharacterType::OrcBoss: {
				charac = instantiator->InstantiateOrcBoss(DynamicMapRef->GetLogicPositions()[i]);
				if (charac)
					enemyInstantiated = true;

				break;
			}
			default:
			{
				charac = instantiator->InstantiateOrcMelee(DynamicMapRef->GetLogicPositions()[i]);
				if (charac)
					enemyInstantiated = true;
				break;
			}
			}

			//Inicializacion de enemigo independientemente de que enemigo sea.
			if (enemyInstantiated) {
				logicCha = new PD_GM_LogicCharacter();
				logicCha->SetIsPlayer(false);
				logicCha->SetTypeCharacter(DynamicMapRef->getEnemies()[DynamicMapRef->GetLogicPositions()[i]]->type_Character);
				FString id_char = "Enemy_";
				id_char.Append(FString::FromInt(index_enemies_forID));
				//index_enemies_forID++;
				//logicCha->SetIDCharacter(id_char);
				logicCha->SetIDCharacter(DynamicMapRef->getEnemies()[DynamicMapRef->GetLogicPositions()[i]]->ID_Character);
				UE_LOG(LogTemp, Log, TEXT("PD_GM_MapManager::InstantiateDynamicMap: Id Dinamicmap: %s"), *logicCha->GetIDCharacter());
				logicCha->SetCharacterBP(charac);
				logicCha->SetController(Cast<APD_AIController>(charac->GetController()));
				logicCha->SetCurrentLogicalPosition(DynamicMapRef->GetLogicPositions()[i]);
				logicCha->SetMapManager(this);
				switch (enemyType)
				{
					case ECharacterType::OrcBow:
					{
						UE_LOG(LogTemp, Log, TEXT("PD_GM_MapManager::InstantiateDynamicMap: OrcBow"));
						logicCha->GetController()->SetTypeCharanimation(0);
						break;
					}
					case ECharacterType::OrcGuns:
					{
						UE_LOG(LogTemp, Log, TEXT("PD_GM_MapManager::InstantiateDynamicMap: OrcGuns"));
						logicCha->GetController()->SetTypeCharanimation(0);
						break;
					}
					case ECharacterType::OrcMelee:
					{
						UE_LOG(LogTemp, Log, TEXT("PD_GM_MapManager::InstantiateDynamicMap: OrcMelee"));
						logicCha->GetController()->SetTypeCharanimation(0);
						break;
					}
				}

				///SETEAR AQUI TODOS LOS STATS- WEAPONS- SKILLS DE CADA TIOPO DE ENEMIGO ENE SU LOGIC CHARACTER
				if (charac)
				{
					UE_LOG(LogTemp, Log, TEXT("PD_GM_MapManager::UPD_ServerGameInstance"));

					UPD_ServerGameInstance* SGI = Cast<UPD_ServerGameInstance>(charac->GetGameInstance());
					if (SGI)
					{
						UE_LOG(LogTemp, Log, TEXT("PD_GM_MapManager:: hay sgi"));

						//Weapon
						int id_weapon, classWeapon, typeWeapon, damage, range;
						SGI->LoadWeaponSpecificDataByType(charac->weapon.GetValue(), id_weapon, classWeapon, typeWeapon, damage, range);
						logicCha->GetWeapon()->ID_Weapon = id_weapon;
						logicCha->GetWeapon()->ClassWeapon = classWeapon;
						logicCha->GetWeapon()->TypeWeapon = typeWeapon;
						logicCha->GetWeapon()->DMWeapon = damage;
						logicCha->GetWeapon()->RangeWeapon = range;

						//Skills
						//PASIVAS
						for (int z = 0; z < charac->pasiveSkillList.Num(); z++)
						{
							FStructSkill skillPasAdded = FStructSkill();
							int weaponR, APSkill, CDSkill, targetSkill, Range;
							SGI->LoadSkillSpecificDataByType(1, (int)charac->pasiveSkillList[z].GetValue(), skillPasAdded.name_Skill, skillPasAdded.description, weaponR, APSkill, CDSkill, targetSkill, Range);
							skillPasAdded.ID_Skill = (int)charac->pasiveSkillList[z].GetValue();
							skillPasAdded.weaponRequired = weaponR;
							skillPasAdded.AP = APSkill;
							skillPasAdded.CD = CDSkill;
							skillPasAdded.currentCD = CDSkill;
							skillPasAdded.range = Range;
							skillPasAdded.target = targetSkill;
							logicCha->GetSkills()->listPasiveSkills.Add(skillPasAdded);
						}
						//ACTIVAS
						for (int j = 0; j < charac->activeSkillList.Num(); j++)
						{
							FStructSkill skillActAdded = FStructSkill();
							int weaponR, APSkill, CDSkill, targetSkill, Range;
							SGI->LoadSkillSpecificDataByType(0, (int)charac->activeSkillList[j].GetValue(), skillActAdded.name_Skill, skillActAdded.description, weaponR, APSkill, CDSkill, targetSkill, Range);
							skillActAdded.ID_Skill = (int)charac->activeSkillList[j].GetValue();
							skillActAdded.weaponRequired = weaponR;
							skillActAdded.AP = APSkill;
							skillActAdded.CD = CDSkill;
							skillActAdded.currentCD = CDSkill;
							skillActAdded.range = Range;
							skillActAdded.target = targetSkill;
							logicCha->GetSkills()->listActiveSkills.Add(skillActAdded);
						}
					}
					else {
						UE_LOG(LogTemp, Log, TEXT("PD_GM_MapManager::NO HAY SGI"));
					}
				}


				//STATS
				logicCha->SetBasicStats(charac->GetPOD(), charac->GetAGI(), charac->GetDES(), charac->GetCON(), charac->GetPER(), charac->GetMAL());
				logicCha->SetInitBaseStats(charac->GetBaseHP(), charac->GetBaseDamage(), charac->GetBaseAP());

				//IA
				if (charac->generateRandomPersonality) {
					//hardcodeado porque no admite c++ saber el tama�o de un enum
					int randPersonality=FMath::RandRange(0, 3);
					logicCha->SetIAPersonality(EIAPersonality(randPersonality));

				}
				else {
					logicCha->SetIAPersonality(charac->IAPersonality.GetValue());

				}

				logicCha->SetTotalStats();

				charac->SetLogicCharacter(logicCha);

				_GAMEMANAGER->enemyManager->AddEnemy(logicCha);
			}

		}
	}

}

TArray<PD_MG_LogicPosition> PD_GM_MapManager::GetAllTilesInRange(float range, PD_MG_LogicPosition logPos)
{
	return logPos.GetAllTilesInRange(range, MapInfo->allLogicPos);

}
#pragma endregion