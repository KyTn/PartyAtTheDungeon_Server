// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "PATD_Server.h"
#include "Structs/PD_ServerEnums.h"
#include "MapGeneration/PD_MG_LogicPosition.h"

/// FORWARD DECLARATIONS
class PD_MG_StaticMap;
class PD_MatchConfigManager;
class PD_MG_MapGenerator;

struct LogicPositionAmplified {

	PD_MG_LogicPosition logicPosition;

	TArray<uint32> info;

	LogicPositionAmplified() {}
	LogicPositionAmplified(PD_MG_LogicPosition logpos) {
		logicPosition = logpos;
		info = TArray<uint32>();
	}

	void AddInfo(uint32 newInfo) { info.Add(newInfo); }
};


struct RoomTemplateInfo {

	FString NAME;
	uint32 ID;
	TArray<MapSkinType> TAGS;
	uint32 WIDTH;
	uint32 HEIGHT;

	TArray<TArray<TCHAR>> RAW_DATA;
	TArray<PD_MG_LogicPosition> LOCAL_LOGIC_POSITIONS_ON_ROOM; // posiciones en local 
	TMap<PD_MG_LogicPosition, StaticMapElementInRoomTemplate> MAP_DATA; // posiciones en local 

	MapSkinType ChoosedTag;

	TArray<PD_MG_LogicPosition> OPEN_WALLS;// posiciones en local 
	TArray<PD_MG_LogicPosition> CLOSED_WALLS;// posiciones en local 
	TArray<PD_MG_LogicPosition> NORMAL_TILES;// posiciones en local 
	TArray<PD_MG_LogicPosition> SPECIAL_TILES;// posiciones en local 
	TArray<PD_MG_LogicPosition> EMPTY_TILES;// posiciones en local 

	PD_MG_LogicPosition BOUNDING_BOX_TOP_LEFT; // son globales (al mapa) no locales de la sala (si el mapa ha sido trimeado, esta info ESTA ACTUALIZADA)
	PD_MG_LogicPosition BOUNDING_BOX_DOWN_RIGHT; // son globales (al mapa) no locales de la sala (si el mapa ha sido trimeado, esta info ESTA ACTUALIZADA)



#pragma region FUNCTIONS

#pragma region Constructors

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
		MAP_DATA = TMap<PD_MG_LogicPosition, StaticMapElementInRoomTemplate>();

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

#pragma endregion 


	// Añade al MAP_DATA la posicion y el tipo de StaticMapElementInRoomTemplate asociado al char que está leyendo
	// - x es la posicion local en el eje X
	// - y es la posicion local en el eje Y
	// - c es el caracter leido del fichero de .rooms
	void AddNewLogicPosition(int x, int y, TCHAR c) {

		RAW_DATA[x][y] = c;
		PD_MG_LogicPosition p = PD_MG_LogicPosition(x, y);
		
		switch (c)
		{
		case 'O':
			MAP_DATA.Add(p, StaticMapElementInRoomTemplate::EMPTY);
			LOCAL_LOGIC_POSITIONS_ON_ROOM.Add(p);
			EMPTY_TILES.Add(p);
			break;
		case '.':
			MAP_DATA.Add(p, StaticMapElementInRoomTemplate::NORMAL_TILE);
			LOCAL_LOGIC_POSITIONS_ON_ROOM.Add(p);
			NORMAL_TILES.Add(p);
			break;
		case ',':
			MAP_DATA.Add(p, StaticMapElementInRoomTemplate::SPECIAL_TILE);
			LOCAL_LOGIC_POSITIONS_ON_ROOM.Add(p);
			SPECIAL_TILES.Add(p);
			NORMAL_TILES.Add(p);
			break;
		case 'W':
			MAP_DATA.Add(p, StaticMapElementInRoomTemplate::WALL_ONLY);
			LOCAL_LOGIC_POSITIONS_ON_ROOM.Add(p);
			CLOSED_WALLS.Add(p);
			break;
		case 'w':
			MAP_DATA.Add(p, StaticMapElementInRoomTemplate::WALL_OR_DOOR);
			LOCAL_LOGIC_POSITIONS_ON_ROOM.Add(p);
			OPEN_WALLS.Add(p);
			break;
		case 'd':
			MAP_DATA.Add(p, StaticMapElementInRoomTemplate::DOOR);
			LOCAL_LOGIC_POSITIONS_ON_ROOM.Add(p);
			break;
		case 't':
			MAP_DATA.Add(p, StaticMapElementInRoomTemplate::PROP_CHEST);
			break;
		case 'c':
			MAP_DATA.Add(p, StaticMapElementInRoomTemplate::COLUMN);
			break;
		case 'q':
			MAP_DATA.Add(p, StaticMapElementInRoomTemplate::WALL_PROP_1);
			break;
		case 'l':
			MAP_DATA.Add(p, StaticMapElementInRoomTemplate::WALL_PROP_TORCH);
			break;
		case 'e':
			MAP_DATA.Add(p, StaticMapElementInRoomTemplate::WALL_PROP_2);
			break;
		case 'h':
			MAP_DATA.Add(p, StaticMapElementInRoomTemplate::PROP_1);
			break;
		case 'j':
			MAP_DATA.Add(p, StaticMapElementInRoomTemplate::PROP_2);
			break;
		case 'b':
			MAP_DATA.Add(p, StaticMapElementInRoomTemplate::OBSTRUCTION);
			break;
		default:
			break;
		}


		//UE_LOG(LogTemp, Log, TEXT("MapProceduralInfo::AddNewLogicPosition - Added!"));

	}


	// Actualiza el bounding box de la habitacion para que esté en POSICION DE MUNDO
	// - BB_TOP_LEFT_IN_MAP es la posicion del (0,0) local en el mundo
	void UpdateBoundingBoxes(PD_MG_LogicPosition BB_TOP_LEFT_IN_MAP) {
		BOUNDING_BOX_TOP_LEFT.SetX(BB_TOP_LEFT_IN_MAP.GetX());
		BOUNDING_BOX_TOP_LEFT.SetY(BB_TOP_LEFT_IN_MAP.GetY());
		BOUNDING_BOX_DOWN_RIGHT.SetX(BB_TOP_LEFT_IN_MAP.GetX() + HEIGHT);
		BOUNDING_BOX_DOWN_RIGHT.SetY(BB_TOP_LEFT_IN_MAP.GetY() + WIDTH);
		//UE_LOG(LogTemp, Log, TEXT("MapProceduralInfo::UpdateBoundingBoxes - New BB_TL(%d,%d) of room %s"),	BOUNDING_BOX_TOP_LEFT.GetX(),	BOUNDING_BOX_TOP_LEFT.GetY(), *NAME);
	}

	// dado una posicion local, te la devuelve en posicion de mundo (en funcion del bounding box que tuviera)
	PD_MG_LogicPosition Translate_LocalPosInRoom_To_MapPosition(PD_MG_LogicPosition localPos) {
		return BOUNDING_BOX_TOP_LEFT + localPos;
	}



	FORCEINLINE bool operator==(const RoomTemplateInfo& other) const
	{
		return this->NAME == other.NAME;
	}

	// devuelve un MapSkin aleatorio de entre los que puede ser, sin tener en cuenta las habitaciones adyacentes 
	MapSkinType ChooseOneMapSkin() {
		MapSkinType m = TAGS[FMath::RandRange(0, TAGS.Num() - 1)];
		UE_LOG(LogTemp, Error, TEXT("RoomTemplateInfo::ChooseOneMapSkin -> choosed: %d"), (int)m);
		return m;
	}

	// Funcion que contiene las reglas de generacion de las skins 
	bool MatchSkins(MapSkinType RoomMapSkinA, MapSkinType RoomMapSkinB)
	{
		switch (RoomMapSkinA) {
		case MapSkinType::DUNGEON_NORMAL:
			switch (RoomMapSkinB)
			{
			case MapSkinType::DUNGEON_NORMAL:
				return true;
			case MapSkinType::GARDEN:
				return true;
			case MapSkinType::LIBRARY:
				return true;
			case MapSkinType::SACRIFICE:
				return true;
			case MapSkinType::BOSS:
				return true;
			default:
				break;
			}

		case MapSkinType::GARDEN:
			switch (RoomMapSkinB)
			{
			case MapSkinType::DUNGEON_NORMAL:
				return true;
			case MapSkinType::GARDEN:
				return true;
			case MapSkinType::LIBRARY:
				return true;
			case MapSkinType::SACRIFICE:
				return true;
			case MapSkinType::BOSS:
				return true;
			default:
				break;
			}

		case MapSkinType::LIBRARY:
			switch (RoomMapSkinB)
			{
			case MapSkinType::DUNGEON_NORMAL:
				return true;
			case MapSkinType::GARDEN:
				return true;
			case MapSkinType::LIBRARY:
				return true;
			case MapSkinType::SACRIFICE:
				return true;
			case MapSkinType::BOSS:
				return true;
			default:
				break;
			}

		case MapSkinType::SACRIFICE:
			switch (RoomMapSkinB)
			{
			case MapSkinType::DUNGEON_NORMAL:
				return true;
			case MapSkinType::GARDEN:
				return true;
			case MapSkinType::LIBRARY:
				return true;
			case MapSkinType::SACRIFICE:
				return true;
			case MapSkinType::BOSS:
				return true;
			default:
				break;
			}

		case MapSkinType::BOSS:
			switch (RoomMapSkinB)
			{
			case MapSkinType::DUNGEON_NORMAL:
				return true;
			case MapSkinType::GARDEN:
				return true;
			case MapSkinType::LIBRARY:
				return true;
			case MapSkinType::SACRIFICE:
				return true;
			case MapSkinType::BOSS:
				return true;
			default:
				break;
			}

		}

		return true;
	}




#pragma endregion 

};

struct MapProceduralInfo {
	uint32 Total_Height, Total_Width;

	TMap<PD_MG_LogicPosition, StaticMapElementInRoomTemplate> mapElements;
	TMap<PD_MG_LogicPosition, MapSkinType> mapSkinByLogicalPosition;
	//TMap<PD_MG_LogicPosition, RoomTemplateInfo> mapRooms;
	TArray<RoomTemplateInfo> mapRooms; 

	TMap<uint8, TArray<uint8>> mapAdj;

	TMap<MapSkinType, TArray<RoomTemplateInfo*>> mapRoomsBySkin;

	TMap <PD_MG_LogicPosition, uint8> enemies; ///TMap que registra los enemigos instanciados por posicion y tipo

	uint8 SPAWN_ID; // indica el idroom de la sala de spawn de jugadores
	uint8 BOSS_ID; // indica el idroom de la sala del boss (solo para cuando el tipo de mision sea con boss)

	PD_MG_LogicPosition BOUNDING_BOX_TOP_LEFT;
	PD_MG_LogicPosition BOUNDING_BOX_DOWN_RIGHT;


	FStructMapData * NETMAPDATA;

#pragma region FUNCTIONS

#pragma region Constructores

	MapProceduralInfo( uint32 _Total_Height, uint32 _Total_Width) {
		Total_Height = _Total_Height;
		Total_Width = _Total_Width;
		NETMAPDATA = nullptr;

		BOUNDING_BOX_TOP_LEFT = PD_MG_LogicPosition((int)_Total_Height, (int)_Total_Width);
		BOUNDING_BOX_DOWN_RIGHT = PD_MG_LogicPosition(0, 0);

		mapElements = TMap<PD_MG_LogicPosition, StaticMapElementInRoomTemplate>();
		mapSkinByLogicalPosition = TMap<PD_MG_LogicPosition, MapSkinType>();
		mapRooms = TArray<RoomTemplateInfo>();

		mapAdj = TMap<uint8, TArray<uint8>>();

		mapRoomsBySkin = TMap<MapSkinType, TArray<RoomTemplateInfo*>>();

		 enemies = TMap <PD_MG_LogicPosition, uint8>();
	}
	
	MapProceduralInfo(FStructMapData * mapData, uint32 _Total_Height, uint32 _Total_Width) {
		Total_Height = _Total_Height;
		Total_Width = _Total_Width;
		NETMAPDATA = mapData;

		BOUNDING_BOX_TOP_LEFT = PD_MG_LogicPosition((int)_Total_Height, (int)_Total_Width);
		BOUNDING_BOX_DOWN_RIGHT = PD_MG_LogicPosition(0, 0);

		mapElements = TMap<PD_MG_LogicPosition, StaticMapElementInRoomTemplate>();
		mapSkinByLogicalPosition = TMap<PD_MG_LogicPosition, MapSkinType>();
		mapRooms = TArray<RoomTemplateInfo>();

		mapAdj = TMap<uint8, TArray<uint8>>();

		mapRoomsBySkin = TMap<MapSkinType, TArray<RoomTemplateInfo*>>();

	   enemies = TMap <PD_MG_LogicPosition, uint8>();
	}

#pragma endregion 


	// pasa de coordenadas locales a coordenadas de mundo
	// - localPos es la posicion local de la que quieres obtener la posicion global
	// - worldPositionOfPivot es la posicion del pivote en posicion de mundo 
	// - localPositionOfPivot es la posicion del pivote en posicion local 
	PD_MG_LogicPosition Translate_LocalPosInRoom_To_MapPosition(PD_MG_LogicPosition localPos, PD_MG_LogicPosition worldPositionOfPivot, PD_MG_LogicPosition localPositionOfPivot) {
		return localPos - localPositionOfPivot + worldPositionOfPivot;
	}
	
	// Añade la informacion de un RoomTemplate al mapa 
	// Actualiza las variables del MapProceduralInfo
	// y actualiza el FStructMapData que se enviará
	// - R la info de la habitacion
	// - C es la posicion del pivote en posicion de mundo
	// - R_pivot es la posicion del pivote en posicion local 
	// - ID es el IDRoom que tendrá esa habitacion 
	void AddRoomToMapAtLocation(RoomTemplateInfo &R, PD_MG_LogicPosition C, PD_MG_LogicPosition R_pivot, uint32 ID) {
		
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
			StaticMapElementInRoomTemplate v = R.MAP_DATA[localPos];

			if (mapElements.Contains(mapPosition)) {
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

			

			UpdateBoundingBoxes(mapPosition);
		}

		//ShowMap();
	}

	


	/*
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
	*/


	// Reduce el tamaño del mapa al minimo posible 
	void TrimBoundingBoxOfRoomsInMap() {
		TArray <PD_MG_LogicPosition> keys;
		//mapRooms.GenerateKeyArray(keys);
		for (int i = 0; i < mapRooms.Num(); i++) {
			mapRooms[i].UpdateBoundingBoxes(mapRooms[i].BOUNDING_BOX_TOP_LEFT - BOUNDING_BOX_TOP_LEFT);
		}
	}

	// Actualiza los bounding boxes del mapa 
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
					case StaticMapElementInRoomTemplate::EMPTY:
						s.AppendChar('O');
						break;

					case StaticMapElementInRoomTemplate::DOOR:
						s.AppendChar('d');
						break;

					case StaticMapElementInRoomTemplate::NORMAL_TILE:
						s.AppendChar('.');
						break;

					case StaticMapElementInRoomTemplate::SPECIAL_TILE:
						s.AppendChar(',');
						break;

					case StaticMapElementInRoomTemplate::WALL_ONLY:
						s.AppendChar('w');
						break;

					case StaticMapElementInRoomTemplate::WALL_OR_DOOR:
						s.AppendChar('w');
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

#pragma endregion

};


class PATD_SERVER_API PD_MG_MapGenerationUtils
{

public:
	PD_MG_MapGenerator* mapGenerator;

	PD_MG_MapGenerationUtils();
	PD_MG_MapGenerationUtils(PD_MG_MapGenerator* mapGenerator);
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

	bool InteractuablesGeneration_v02(MapProceduralInfo &M, PD_MatchConfigManager* MatchConfigMan, int numPlayers, TArray<LogicPositionAmplified> &LInteractuables);

#pragma endregion




private:

#pragma region PROCEDURAL GENERATION UTILS 

	bool Hard_Check_CanBeColocated(MapProceduralInfo &M, RoomTemplateInfo &R, PD_MG_LogicPosition C, PD_MG_LogicPosition R_pivot);
	bool MapCanContainsRoom(MapProceduralInfo &M, RoomTemplateInfo &R, PD_MG_LogicPosition C, PD_MG_LogicPosition R_pivot);

	PD_MG_LogicPosition Translate_LocalPosInRoom_To_MapPosition(PD_MG_LogicPosition localPos, PD_MG_LogicPosition C, PD_MG_LogicPosition R_pivot);
	bool Put_Door_Tryng_doubleDoor_at(MapProceduralInfo &M, PD_MG_LogicPosition W1, uint8 IDR, TArray<LogicPositionAmplified> &LD, TArray<LogicPositionAmplified> &LInteractuables);

	//devuelve la dificultad del dungeon
	int DifficultyDungeon(MATCHCONFIG_DIFFICULTY matchConfig_MATCHCONFIG_MAPDIFFICULTY);

	// devuelve el numero de habitaciones en funcion de la configuracion del mapa 
	int NumberOfRoomsOnMatchConfig(MATCHCONFIG_MAPSIZE matchConfig_MATCHCONFIG_MAPSIZE, int numberOfPlayers);

	// Dado una skin y una coleccion de posibles skins, te devuelve, en funcion de las reglas, el indice de esa coleccion que es la skin elegida
	MapSkinType ChoosesSkinRules(RoomTemplateInfo &R, TArray<MapSkinType> & ChoosableMapSkins);

	// Te devuelve el StaticMapElement que puede tener en funcion del tipo de mapskin y del caracter que haya en el StaticMapElementInRoomTemplate
	StaticMapElement GetStaticMapElementFrom(MapSkinType mst, StaticMapElementInRoomTemplate melem);


#pragma region COMPOSITIONS
	uint16 MapSkinByRoomOf(int IdRoom, MapSkinType mapskintype);
	uint32 RoomCompositionOf(PD_MG_LogicPosition logpos, StaticMapElement, int IdRoom);
	uint32 WallCompositionOf(PD_MG_LogicPosition logpos, int IdRoomA, int IdRoomB);
	uint32 DoorCompositionOf(PD_MG_LogicPosition logpos, int IdRoomA, int IdRoomB);
	uint32 InteractuableIDOf(PD_MG_LogicPosition logpos, int IdInteractuable, int TypeInteractuable);
	uint16 InteractuableComposition(int IdInteractuable, int IdReactuable);
	uint32 EnemyCompositionOf(PD_MG_LogicPosition logpos, int IdEnemy, int TypeEnemy);

#pragma endregion

	void Fill_NETMAPDATA_from(MapProceduralInfo &M, TArray<LogicPositionAmplified> walls, TArray<LogicPositionAmplified> doors, TArray<LogicPositionAmplified> interactuables);

#pragma endregion
};
