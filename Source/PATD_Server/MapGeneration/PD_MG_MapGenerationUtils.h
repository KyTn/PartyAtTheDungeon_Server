// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "PATD_Server.h"
#include "Structs/PD_ServerEnums.h"
#include "MapGeneration/PD_MG_LogicPosition.h"

/// FORWARD DECLARATIONS
class PD_MG_StaticMap;
class PD_MatchConfigManager;

struct RoomTemplateInfo {

	FString NAME;
	uint32 ID;
	TArray<MapSkinType> TAGS;
	uint32 WIDTH;
	uint32 HEIGHT;

	TArray<TArray<TCHAR>> RAW_DATA;
	TArray<PD_MG_LogicPosition> LOCAL_LOGIC_POSITIONS_ON_ROOM;
	TMap<PD_MG_LogicPosition, StaticMapElement> MAP_DATA;

	MapSkinType ChoosedTag;

	TArray<PD_MG_LogicPosition> OPEN_WALLS;
	TArray<PD_MG_LogicPosition> CLOSED_WALLS;
	TArray<PD_MG_LogicPosition> NORMAL_TILES;
	TArray<PD_MG_LogicPosition> SPECIAL_TILES;
	TArray<PD_MG_LogicPosition> EMPTY_TILES;

	PD_MG_LogicPosition BOUNDING_BOX_TOP_LEFT; // son globales (al mapa) no locales de la sala
	PD_MG_LogicPosition BOUNDING_BOX_DOWN_RIGHT; // son globales (al mapa) no locales de la sala

	RoomTemplateInfo() {}


	RoomTemplateInfo(FString Name, uint32 id, TArray<MapSkinType> tags, int h, int w) {
		NAME = Name;
		ID = id;
		TAGS = tags;
		WIDTH = w;
		HEIGHT = h;

		BOUNDING_BOX_TOP_LEFT = PD_MG_LogicPosition(0, 0);
		BOUNDING_BOX_DOWN_RIGHT = PD_MG_LogicPosition(HEIGHT, WIDTH);


		LOCAL_LOGIC_POSITIONS_ON_ROOM = TArray <PD_MG_LogicPosition>();
		RAW_DATA = TArray<TArray<TCHAR>>();
		MAP_DATA = TMap<PD_MG_LogicPosition, StaticMapElement>();

		OPEN_WALLS = 	TArray<PD_MG_LogicPosition>();
		CLOSED_WALLS = 	TArray<PD_MG_LogicPosition>();
		NORMAL_TILES = 	TArray<PD_MG_LogicPosition>();
		SPECIAL_TILES = TArray<PD_MG_LogicPosition>();
		EMPTY_TILES = 	TArray<PD_MG_LogicPosition>();

		if(tags.Num() > 0) ChoosedTag = tags[0];
		else ChoosedTag = MapSkinType::DUNGEON_NORMAL;

		for (uint32 i = 0; i < HEIGHT; i++) {
			RAW_DATA.Add(TArray<TCHAR>());
			for (uint32 j= 0; j < WIDTH; j++) {
				RAW_DATA[i].Add(' ');
			}
		}

		//UE_LOG(LogTemp, Log, TEXT("MapProceduralInfo::RoomTemplateInfo - RAW_DATA  (%d,%d)"), RAW_DATA.Num(), RAW_DATA[0].Num());
	}


	void AddNewLogicPosition(int x, int y, TCHAR c) {

		//UE_LOG(LogTemp, Log, TEXT("MapProceduralInfo::AddNewLogicPosition - Adding (%d,%d) - %c - RAW_DATA  (%d,%d)"),x,y,c, RAW_DATA.Num(), RAW_DATA[x].Num());


		RAW_DATA[x][y] = c;
		PD_MG_LogicPosition p = PD_MG_LogicPosition(x, y);
		
		switch (c)
		{
		case 'O':
			MAP_DATA.Add(p, StaticMapElement::EMPTY);
			LOCAL_LOGIC_POSITIONS_ON_ROOM.Add(p);
			EMPTY_TILES.Add(p);
			break;
		case '.':
			MAP_DATA.Add(p, StaticMapElement::NORMAL_TILE);
			LOCAL_LOGIC_POSITIONS_ON_ROOM.Add(p);
			NORMAL_TILES.Add(p);
			break;
		case ',':
			MAP_DATA.Add(p, StaticMapElement::SPECIAL_TILE);
			LOCAL_LOGIC_POSITIONS_ON_ROOM.Add(p);
			SPECIAL_TILES.Add(p);
			NORMAL_TILES.Add(p);
			break;
		case 'W':
			MAP_DATA.Add(p, StaticMapElement::WALL_ONLY);
			LOCAL_LOGIC_POSITIONS_ON_ROOM.Add(p);
			CLOSED_WALLS.Add(p);
			break;
		case 'w':
			MAP_DATA.Add(p, StaticMapElement::WALL_OR_DOOR);
			LOCAL_LOGIC_POSITIONS_ON_ROOM.Add(p);
			OPEN_WALLS.Add(p);
			break;
		default:
			break;
		}


		//UE_LOG(LogTemp, Log, TEXT("MapProceduralInfo::AddNewLogicPosition - Added!"));

	}

	void UpdateBoundingBoxes(PD_MG_LogicPosition BB_TOP_LEFT_IN_MAP) {
		BOUNDING_BOX_TOP_LEFT.SetX(BB_TOP_LEFT_IN_MAP.GetX());
		BOUNDING_BOX_TOP_LEFT.SetY(BB_TOP_LEFT_IN_MAP.GetY());
		BOUNDING_BOX_DOWN_RIGHT.SetX(BB_TOP_LEFT_IN_MAP.GetX() + HEIGHT);
		BOUNDING_BOX_DOWN_RIGHT.SetY(BB_TOP_LEFT_IN_MAP.GetY() + WIDTH);
		//UE_LOG(LogTemp, Log, TEXT("MapProceduralInfo::UpdateBoundingBoxes - New BB_TL(%d,%d) of room %s"),	BOUNDING_BOX_TOP_LEFT.GetX(),	BOUNDING_BOX_TOP_LEFT.GetY(), *NAME);
	}

	PD_MG_LogicPosition Translate_LocalPosInRoom_To_MapPosition(PD_MG_LogicPosition localPos) {
		return BOUNDING_BOX_TOP_LEFT + localPos;
	}



	FORCEINLINE bool operator==(const RoomTemplateInfo& other) const
	{
		return this->NAME == other.NAME;
	}
};

struct MapProceduralInfo {
	uint32 Total_Height, Total_Width;

	TMap<PD_MG_LogicPosition, StaticMapElement> mapElements;
	TMap<PD_MG_LogicPosition, MapSkinType> mapSkinByLogicalPosition;
	//TMap<PD_MG_LogicPosition, RoomTemplateInfo> mapRooms;
	TArray<RoomTemplateInfo> mapRooms; 

	TMap<MapSkinType, TArray<RoomTemplateInfo*>> mapRoomsBySkin;

	TArray<TArray<int>> Ady;
	uint32 SPAWN_ID;
	PD_MG_LogicPosition BOUNDING_BOX_TOP_LEFT;
	PD_MG_LogicPosition BOUNDING_BOX_DOWN_RIGHT;


	FStructMapData * NETMAPDATA;


	MapProceduralInfo( uint32 _Total_Height, uint32 _Total_Width) {
		Total_Height = _Total_Height;
		Total_Width = _Total_Width;
		NETMAPDATA = nullptr;

		BOUNDING_BOX_TOP_LEFT = PD_MG_LogicPosition((int)_Total_Height, (int)_Total_Width);
		BOUNDING_BOX_DOWN_RIGHT = PD_MG_LogicPosition(0, 0);

		mapElements = TMap<PD_MG_LogicPosition, StaticMapElement>();
		mapSkinByLogicalPosition = TMap<PD_MG_LogicPosition, MapSkinType>();
		mapRooms = TArray<RoomTemplateInfo>();

		mapRoomsBySkin = TMap<MapSkinType, TArray<RoomTemplateInfo*>>();

	}
	
	MapProceduralInfo(FStructMapData * mapData, uint32 _Total_Height, uint32 _Total_Width) {
		Total_Height = _Total_Height;
		Total_Width = _Total_Width;
		NETMAPDATA = mapData;

		BOUNDING_BOX_TOP_LEFT = PD_MG_LogicPosition((int)_Total_Height, (int)_Total_Width);
		BOUNDING_BOX_DOWN_RIGHT = PD_MG_LogicPosition(0, 0);

		mapElements = TMap<PD_MG_LogicPosition, StaticMapElement>();
		mapSkinByLogicalPosition = TMap<PD_MG_LogicPosition, MapSkinType>();
		mapRooms = TArray<RoomTemplateInfo>();

		mapRoomsBySkin = TMap<MapSkinType, TArray<RoomTemplateInfo*>>();

	}

	PD_MG_LogicPosition Translate_LocalPosInRoom_To_MapPosition(PD_MG_LogicPosition localPos, PD_MG_LogicPosition C, PD_MG_LogicPosition R_pivot) {
		return localPos - R_pivot + C;
	}


	void AddRoomToMapAtLocation(RoomTemplateInfo &R, PD_MG_LogicPosition C, PD_MG_LogicPosition R_pivot, uint32 ID) {
		UE_LOG(LogTemp, Log, TEXT("MapProceduralInfo::AddRoomToMapAtLocation - Adding %d logicpositions C(%d,%d) R_p(%d,%d) of room %s"), 
			R.LOCAL_LOGIC_POSITIONS_ON_ROOM.Num(),
			C.GetX(), C.GetY(),
			R_pivot.GetX(), R_pivot.GetY(), *(R.NAME));
		PD_MG_LogicPosition start;
		start.SetX(0); start.SetY(0);///Se deberia coger la (0,0) porque sino da problemas de desplazamiento
		PD_MG_LogicPosition bb_t_l = Translate_LocalPosInRoom_To_MapPosition(start, C, R_pivot);
		R.UpdateBoundingBoxes(bb_t_l);
		R.ID = ID;
		mapRooms.Add(R);
		for (int i = 0; i < R.LOCAL_LOGIC_POSITIONS_ON_ROOM.Num(); i++) {
			PD_MG_LogicPosition localPos = R.LOCAL_LOGIC_POSITIONS_ON_ROOM[i];
			PD_MG_LogicPosition mapPosition = Translate_LocalPosInRoom_To_MapPosition(localPos, C, R_pivot);


			/// AÑADIMOS EL ELEMENTO AL MAPA DE ELEMENTOS
			StaticMapElement v = R.MAP_DATA[localPos];
			if (mapElements.Contains(mapPosition)) {
				//UE_LOG(LogTemp, Log, TEXT("MapProceduralInfo::AddRoomToMapAtLocation - machacando"));
				mapElements[mapPosition] = v;
			}
			else {
				//UE_LOG(LogTemp, Log, TEXT("MapProceduralInfo::AddRoomToMapAtLocation - creando"));
				mapElements.Emplace(mapPosition, v);
			}

			/// Y AÑADIMOS AL MAPA DE MAPSKINS
			if (mapSkinByLogicalPosition.Contains(mapPosition)) {
				//UE_LOG(LogTemp, Log, TEXT("MapProceduralInfo::AddRoomToMapAtLocation - machacando"));
				mapSkinByLogicalPosition[mapPosition] = R.ChoosedTag;
			}
			else {
				//UE_LOG(LogTemp, Log, TEXT("MapProceduralInfo::AddRoomToMapAtLocation - creando"));
				mapSkinByLogicalPosition.Emplace(mapPosition, R.ChoosedTag);
			}

			///AÑADIMOS LA ROOM AL MAPA QUE LOS ALMACENA EN FUNCION DE LA LOGPOS
			/*if (mapRooms.Contains(mapPosition)) {
				//UE_LOG(LogTemp, Log, TEXT("MapProceduralInfo::AddRoomToMapAtLocation - machacando"));
				mapRooms[mapPosition] = R;
			}
			else {
				//UE_LOG(LogTemp, Log, TEXT("MapProceduralInfo::AddRoomToMapAtLocation - creando"));
				mapRooms.Emplace(mapPosition, R);
			}*/

			UpdateBoundingBoxes(mapPosition);
		}

		//ShowMap();
	}

	void UpdateBoundingBoxes(PD_MG_LogicPosition pos) {
		if (pos.GetX() < BOUNDING_BOX_TOP_LEFT.GetX()) {
			BOUNDING_BOX_TOP_LEFT.SetX(pos.GetX());
			//UE_LOG(LogTemp, Log, TEXT("MapProceduralInfo::UpdateBoundingBoxes -X New BOUNDING_BOX_TOP_LEFT at /\\ (%d,%d)"),BOUNDING_BOX_TOP_LEFT.GetX(), BOUNDING_BOX_TOP_LEFT.GetY());
		}
		if (pos.GetY() < BOUNDING_BOX_TOP_LEFT.GetY()) {
			BOUNDING_BOX_TOP_LEFT.SetY(pos.GetY());
			//UE_LOG(LogTemp, Log, TEXT("MapProceduralInfo::UpdateBoundingBoxes -X New BOUNDING_BOX_TOP_LEFT at /\\ (%d,%d)"),BOUNDING_BOX_TOP_LEFT.GetX(), BOUNDING_BOX_TOP_LEFT.GetY());
		}
		if (pos.GetX() > BOUNDING_BOX_DOWN_RIGHT.GetX()) {
			BOUNDING_BOX_DOWN_RIGHT.SetX(pos.GetX());
			//UE_LOG(LogTemp, Log, TEXT("MapProceduralInfo::UpdateBoundingBoxes -Y New BOUNDING_BOX_DOWN_RIGHT at \\/ (%d,%d)"),BOUNDING_BOX_DOWN_RIGHT.GetX(), BOUNDING_BOX_DOWN_RIGHT.GetY());
		}
		if (pos.GetY() > BOUNDING_BOX_DOWN_RIGHT.GetY()) {
			BOUNDING_BOX_DOWN_RIGHT.SetY(pos.GetY());
			//UE_LOG(LogTemp, Log, TEXT("MapProceduralInfo::UpdateBoundingBoxes -Y New BOUNDING_BOX_DOWN_RIGHT at \\/ (%d,%d)"),BOUNDING_BOX_DOWN_RIGHT.GetX(), BOUNDING_BOX_DOWN_RIGHT.GetY());
		}
	}

	void ShowMap() {
		UE_LOG(LogTemp, Log, TEXT("MapProceduralInfo::AddRoomToMapAtLocation MAP: "));
		for (uint32 i = 0; i < Total_Height; i++) {
			FString s = "";
			PD_MG_LogicPosition p;
			for (uint32 j = 0; j < Total_Width; j++) {
				p = PD_MG_LogicPosition(i, j);
				if (mapElements.Contains(p)) {
					switch (mapElements[p])
					{
					case StaticMapElement::EMPTY:
						s.AppendChar('O');
						break;

					case StaticMapElement::DOOR:
						s.AppendChar('d');
						break;

					case StaticMapElement::NORMAL_TILE:
						s.AppendChar('.');
						break;

					case StaticMapElement::SPECIAL_TILE:
						s.AppendChar(',');
						break;

					case StaticMapElement::WALL_ONLY:
						s.AppendChar('W');
						break;

					case StaticMapElement::WALL_OR_DOOR:
						s.AppendChar('w');
						break;

					default:
						break;
					}
				}
				else {
					s.AppendChar('#');
				}
			}

			UE_LOG(LogTemp, Log, TEXT("%s"), *s);
		}

	}

	void ShowMapOnBoundingBox() {
		UE_LOG(LogTemp, Log, TEXT("MapProceduralInfo::ShowMapOnBoundingBox FROM(%d,%d) TO(%d,%d) MAP: \n%s"), BOUNDING_BOX_TOP_LEFT.GetX(), BOUNDING_BOX_TOP_LEFT.GetY(), BOUNDING_BOX_DOWN_RIGHT.GetX(), BOUNDING_BOX_DOWN_RIGHT.GetY(), *ToString());

	}

	void TrimBoundingBoxOfRoomsInMap() {
		TArray <PD_MG_LogicPosition> keys;
		//mapRooms.GenerateKeyArray(keys);
		for (int i = 0; i < mapRooms.Num(); i++) {
			mapRooms[i].UpdateBoundingBoxes(mapRooms[i].BOUNDING_BOX_TOP_LEFT - BOUNDING_BOX_TOP_LEFT);
		}
	}

	FString ToString() {
		FString s = "v0.1\ndefeatboss\n";
		s.AppendInt(BOUNDING_BOX_DOWN_RIGHT.GetX() - BOUNDING_BOX_TOP_LEFT.GetX() +1);
		s.AppendChar('\n'); 
		s.AppendInt(BOUNDING_BOX_DOWN_RIGHT.GetY() - BOUNDING_BOX_TOP_LEFT.GetY()+2);
		s.AppendChar('\n');
		for (int i = BOUNDING_BOX_TOP_LEFT.GetX(); i <= BOUNDING_BOX_DOWN_RIGHT.GetX(); i++) {
			PD_MG_LogicPosition p;
			for (int j = BOUNDING_BOX_TOP_LEFT.GetY(); j <= BOUNDING_BOX_DOWN_RIGHT.GetY(); j++) {
				p = PD_MG_LogicPosition(i, j);
				if (mapElements.Contains(p)) {
					switch (mapElements[p])
					{
					case StaticMapElement::EMPTY:
						s.AppendChar('O');
						break;

					case StaticMapElement::DOOR:
						s.AppendChar('d');
						break;

					case StaticMapElement::NORMAL_TILE:
						s.AppendChar('.');
						break;

					case StaticMapElement::SPECIAL_TILE:
						s.AppendChar(',');
						break;

					case StaticMapElement::WALL_ONLY:
						s.AppendChar('w');
						break;

					case StaticMapElement::WALL_OR_DOOR:
						s.AppendChar('w');
						break;

					case StaticMapElement::SPAWN_POINT:
						s.AppendChar('s');
						break;
					default:
						break;
					}
				}
				else {
					s.AppendChar(' ');
				}
			}

			s.AppendChar('\n');
		}

		return s;
	}

};


class PATD_SERVER_API PD_MG_MapGenerationUtils
{

public:
	PD_MG_MapGenerationUtils();
	~PD_MG_MapGenerationUtils();

#pragma region READ ROOMS FROM FILE AND FROM CHORIZO

	bool ReadAndPrepareRoomTemplateInfosFromFile(FString filepath, TArray<RoomTemplateInfo> &roomTemplates);
	bool ReadAndPrepareRoomTemplateInfosFromChorizo(FString chorizo, TArray<RoomTemplateInfo> &roomTemplates);
	RoomTemplateInfo FillRoomTemplateInfoWith(FString readedString, int id);
	void ParseTags(TArray<MapSkinType>& tags, FString braquets);

#pragma endregion


#pragma region PRELOADED ROOMS

	bool GetPreloadedData(TArray<RoomTemplateInfo> &roomTemplates);

#pragma endregion


#pragma region PROCEDURAL GENERATION v0.1

	bool GenerateRandomStaticMap_v01(MapProceduralInfo &M, TArray<RoomTemplateInfo> &roomTemplateArray, int _Total_Height, int _Total_Width, PD_MatchConfigManager* MatchConfigMan, int numPlayers);

	void MarkARoomAsSpawingRoom_v01(MapProceduralInfo &M, MATCHCONFIG_MISSIONTYPE missionType);

	FString EnemiesGeneration_v01(MapProceduralInfo &M);

#pragma endregion


#pragma region PROCEDURAL GENERATION v0.2

	bool GenerateRandomStaticMap_v02(MapProceduralInfo &M, TArray<RoomTemplateInfo> &roomTemplateArray, int _Total_Height, int _Total_Width, PD_MatchConfigManager* MatchConfigMan, int numPlayers);

	void MarkARoomAsSpawingRoom_v02(MapProceduralInfo &M, MATCHCONFIG_MISSIONTYPE missionType);

	bool EnemiesGeneration_v02(MapProceduralInfo &M, PD_MatchConfigManager* MatchConfigMan, int numPlayers);

#pragma endregion




private:

#pragma region PROCEDURAL GENERATION UTILS 

	bool Hard_Check_CanBeColocated(MapProceduralInfo &M, RoomTemplateInfo &R, PD_MG_LogicPosition C, PD_MG_LogicPosition R_pivot);
	bool MapCanContainsRoom(MapProceduralInfo &M, RoomTemplateInfo &R, PD_MG_LogicPosition C, PD_MG_LogicPosition R_pivot);

	PD_MG_LogicPosition Translate_LocalPosInRoom_To_MapPosition(PD_MG_LogicPosition localPos, PD_MG_LogicPosition C, PD_MG_LogicPosition R_pivot);
	bool Put_Door_Tryng_doubleDoor_at(MapProceduralInfo &M, PD_MG_LogicPosition W1);

	// devuelve el numero de habitaciones en funcion de la configuracion del mapa 
	int NumberOfRoomsOnMatchConfig(MATCHCONFIG_MAPSIZE matchConfig_MATCHCONFIG_MAPSIZE, int numberOfPlayers);

	// Dado una skin y una coleccion de posibles skins, te devuelve, en funcion de las reglas, el indice de esa coleccion que es la skin elegida
	int ChoosesSkinRules(MapSkinType RoomMapSkin, TArray<MapSkinType> & ChoosableMapSkins);

	// Funcion que contiene las reglas de generacion de las skins 
	bool MatchSkins(MapSkinType RoomMapSkinA, MapSkinType RoomMapSkinB);
#pragma endregion
};
