// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "PATD_Server.h"
#include "Structs/PD_ServerEnums.h"
#include "MapGeneration/PD_MG_LogicPosition.h"

/// FORWARD DECLARATIONS
class PD_MG_StaticMap;


struct RoomTemplateInfo {

	FString NAME;
	uint32 ID;
	TArray<FString> TAGS;
	uint32 WIDTH;
	uint32 HEIGHT;
	TArray<TArray<TCHAR>> RAW_DATA;
	TArray<PD_MG_LogicPosition> LOCAL_LOGIC_POSITIONS_ON_ROOM;
	TMap<PD_MG_LogicPosition, StaticMapElement> MAP_DATA;

	TArray<PD_MG_LogicPosition> OPEN_WALLS;
	TArray<PD_MG_LogicPosition> CLOSED_WALLS;
	TArray<PD_MG_LogicPosition> NORMAL_TILES;
	TArray<PD_MG_LogicPosition> SPECIAL_TILES;
	TArray<PD_MG_LogicPosition> EMPTY_TILES;

	PD_MG_LogicPosition BOUNDING_BOX_TOP_LEFT;
	PD_MG_LogicPosition BOUNDING_BOX_DOWN_RIGHT;

	RoomTemplateInfo() {}


	RoomTemplateInfo(FString Name, uint32 id, TArray<FString> tags, int h, int w) {
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


	FORCEINLINE bool operator==(const RoomTemplateInfo& other) const
	{
		return this->NAME == other.NAME;
	}
};

struct MapProceduralInfo {
	uint32 Total_Height, Total_Width;

	TMap<PD_MG_LogicPosition, StaticMapElement> mapElements;
	TMap<PD_MG_LogicPosition, RoomTemplateInfo> mapRooms;
	

	PD_MG_LogicPosition BOUNDING_BOX_TOP_LEFT;
	PD_MG_LogicPosition BOUNDING_BOX_DOWN_RIGHT;

	MapProceduralInfo(uint32 _Total_Height, uint32 _Total_Width) {
		Total_Height = _Total_Height;
		Total_Width = _Total_Width;

		BOUNDING_BOX_TOP_LEFT = PD_MG_LogicPosition((int)_Total_Height, (int)_Total_Width);
		BOUNDING_BOX_DOWN_RIGHT = PD_MG_LogicPosition(0, 0);

		mapElements = TMap<PD_MG_LogicPosition, StaticMapElement>();
		mapRooms = TMap<PD_MG_LogicPosition, RoomTemplateInfo>();
	}

	PD_MG_LogicPosition Translate_LocalPosInRoom_To_MapPosition(PD_MG_LogicPosition localPos, PD_MG_LogicPosition C, PD_MG_LogicPosition R_pivot) {
		return localPos - R_pivot + C;
	}


	void AddRoomToMapAtLocation(RoomTemplateInfo &R, PD_MG_LogicPosition C, PD_MG_LogicPosition R_pivot) {
		UE_LOG(LogTemp, Log, TEXT("MapProceduralInfo::AddRoomToMapAtLocation - Adding %d logicpositions C(%d,%d) R_p(%d,%d) of room %s"), 
			R.LOCAL_LOGIC_POSITIONS_ON_ROOM.Num(),
			C.GetX(), C.GetY(),
			R_pivot.GetX(), R_pivot.GetY(), *(R.NAME));

		for (int i = 0; i < R.LOCAL_LOGIC_POSITIONS_ON_ROOM.Num(); i++) {
			PD_MG_LogicPosition localPos = R.LOCAL_LOGIC_POSITIONS_ON_ROOM[i];
			PD_MG_LogicPosition mapPosition = Translate_LocalPosInRoom_To_MapPosition(localPos, C, R_pivot);

			/*
			UE_LOG(LogTemp, Log, TEXT("MapProceduralInfo::AddRoomToMapAtLocation - Adding L(%d,%d) to M(%d,%d)"), 
				localPos.GetX(), localPos.GetY(),
				mapPosition.GetX(), mapPosition.GetY());
			
			*/


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

			///AÑADIMOS LA ROOM AL MAPA QUE LOS ALMACENA EN FUNCION DE LA LOGPOS
			if (mapRooms.Contains(mapPosition)) {
				//UE_LOG(LogTemp, Log, TEXT("MapProceduralInfo::AddRoomToMapAtLocation - machacando"));
				mapRooms[mapPosition] = R;
			}
			else {
				//UE_LOG(LogTemp, Log, TEXT("MapProceduralInfo::AddRoomToMapAtLocation - creando"));
				mapRooms.Emplace(mapPosition, R);
			}

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

	bool ReadAndPrepareRoomTemplateInfosFromFile(FString filepath, TArray<RoomTemplateInfo> &roomTemplates);
	bool ReadAndPrepareRoomTemplateInfosFromChorizo(FString chorizo, TArray<RoomTemplateInfo> &roomTemplates);


	bool GetPreloadedData(TArray<RoomTemplateInfo> &roomTemplates);


	bool GenerateRandomStaticMap(MapProceduralInfo &M, TArray<RoomTemplateInfo> &roomTemplateArray, int _Total_Height, int _Total_Width);

	bool Hard_Check_CanBeColocated(MapProceduralInfo &M, RoomTemplateInfo &R, PD_MG_LogicPosition C, PD_MG_LogicPosition R_pivot);
	bool MapCanContainsRoom(MapProceduralInfo &M, RoomTemplateInfo &R, PD_MG_LogicPosition C, PD_MG_LogicPosition R_pivot);

	PD_MG_LogicPosition Translate_LocalPosInRoom_To_MapPosition(PD_MG_LogicPosition localPos, PD_MG_LogicPosition C, PD_MG_LogicPosition R_pivot);
	bool Put_Door_Tryng_doubleDoor_at(MapProceduralInfo &M, PD_MG_LogicPosition W1);

	void MarkARoomAsSpawingRoom(MapProceduralInfo &M);

private:
	RoomTemplateInfo FillRoomTemplateInfoWith(FString readedString, int id);
	void ParseTags(TArray<FString>& tags, FString braquets);


};
