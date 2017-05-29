// Fill out your copyright notice in the Description page of Project Settings.

#include "PATD_Server.h"
#include "PD_MG_MapGenerationUtils.h"
#include "PATD_Server/MapGeneration/PD_MG_MapGenerator.h"
#include "PATD_Server/PD_MatchConfigManager.h"
#include "PATD_Server/Structs/PD_ServerEnums.h"
#include "PATD_Server/MapGeneration/Static/PD_MG_StaticMap.h"

PD_MG_MapGenerationUtils::PD_MG_MapGenerationUtils()
{
}

PD_MG_MapGenerationUtils::PD_MG_MapGenerationUtils(PD_MG_MapGenerator* mGenerator)
{
	mapGenerator = mGenerator;
}

PD_MG_MapGenerationUtils::~PD_MG_MapGenerationUtils()
{
}


#pragma region READ ROOMS FROM FILE AND FROM CHORIZO


//TESTING : Content/DungeonTestingMaps/test1.rooms



/// Lee desde fichero el contenido de un fichero y lo parsea como un contenedor de habitaciones
bool PD_MG_MapGenerationUtils::ReadAndPrepareRoomTemplateInfosFromFile(FString filepath, TArray<RoomTemplateInfo> &roomTemplates) {

	// Leamos el fichero
	UE_LOG(LogTemp, Log, TEXT("PD_MG_MapGenerationUtils::ReadAndPrepareRoomTemplateInfosFromFile - reading from %s"), *filepath);

	FString FileData = "";

	if (FFileHelper::LoadFileToString(FileData, *filepath))
	{
		//UE_LOG(LogTemp, Log, TEXT("PD_MG_MapGenerationUtils::ReadAndPrepareRoomTemplateInfosFromFile el chorizo es:\n %s"), *FileData);
		return ReadAndPrepareRoomTemplateInfosFromChorizo(FileData, roomTemplates);
	}

	return false;
}


/// Parsea un FString como un contenedor de habitaciones
bool PD_MG_MapGenerationUtils::ReadAndPrepareRoomTemplateInfosFromChorizo(FString chorizo, TArray<RoomTemplateInfo> &roomTemplates) {

	roomTemplates.Empty();

	TArray<FString> fileSplitted;
	chorizo.ParseIntoArray(fileSplitted, TEXT("START_ROOM"), true);

	//UE_LOG(LogTemp, Log, TEXT("PD_MG_MapGenerationUtils::ReadAndPrepareRoomTemplateInfosFromChorizo num of room splitted %d"), fileSplitted.Num());

	for (int i = 0; i < fileSplitted.Num(); i++) {
		RoomTemplateInfo rti = FillRoomTemplateInfoWith(fileSplitted[i], i);
		roomTemplates.Add(rti);

	}

	return true;
}



/// Dado un RoomTemplateInfo y la informacion en un FString, lo rellena con dicha informacion
RoomTemplateInfo PD_MG_MapGenerationUtils::FillRoomTemplateInfoWith(FString readedString, int id) {

	TArray<FString> roomInfoSplitted;
	FString aux2 = "";

	readedString.ParseIntoArray(roomInfoSplitted, TEXT("\n"), true);

	/// GET NAME
	roomInfoSplitted[1].Split(": ", &aux2, &roomInfoSplitted[1]);
	FString name = roomInfoSplitted[1];

	/// GET TAGS
	TArray<MapSkinType> TAGS;
	ParseTags(TAGS, roomInfoSplitted[2]);
	

	/// GET WIDTH
	roomInfoSplitted[3].Split(": ", &aux2, &roomInfoSplitted[3]);
	int w = FCString::Atoi(*(roomInfoSplitted[3]));

	/// GET HEIGHT
	roomInfoSplitted[4].Split(": ", &aux2, &roomInfoSplitted[4]);
	int h = FCString::Atoi(*(roomInfoSplitted[4]));


	RoomTemplateInfo r = RoomTemplateInfo(name, id, TAGS, h, w);



	/// GET DATA
	for (uint32 i = 5; i < r.HEIGHT + 5; i++) {
		TArray<TCHAR> ta = roomInfoSplitted[i].GetCharArray();

		UE_LOG(LogTemp, Log, TEXT("PD_MG_MapGenerationUtils::FillRoomTemplateInfoWith - Adding this:-%s-"), *roomInfoSplitted[i]);

		for (uint32 j = 0; j < r.WIDTH; j++) {
			r.AddNewLogicPosition(i-5, j, ta[j]);
		}
	}

	//UE_LOG(LogTemp, Log, TEXT("PD_MG_MapGenerationUtils::FillRoomTemplateInfoWith Size RAWDATA : %d x 0"), r->RAW_DATA.Num());// , r->RAW_DATA[0].Num());

	return r;
}

///Dado un array de tags y un string con los tags, rellena el array con el contenido del string
void PD_MG_MapGenerationUtils::ParseTags(TArray<MapSkinType> &tags, FString braquets) {
	tags.Empty();

	TArray<FString> s1 = TArray<FString>(), s = TArray<FString>();
	braquets.ParseIntoArray(s1, TEXT(":"), true);
	for (int i = 0; i < s1.Num(); i++) {
		UE_LOG(LogTemp, Error, TEXT("PD_MG_MapGenerationUtils::ParseTags s[%d] -> MapSkinType %s"), i, *(s1[i]));
	}
	s1[1].ParseIntoArray(s, TEXT(","), true);

	UE_LOG(LogTemp, Log, TEXT("PD_MG_MapGenerationUtils::ParseTags - brackets-%s-"), *braquets);

	if (s.Num() == 0) {
		UE_LOG(LogTemp, Error, TEXT("PD_MG_MapGenerationUtils::ParseTags s == 0 -> aplicando MapSkinType::DUNGEON_NORMAL"));
		tags.Add(MapSkinType::DUNGEON_NORMAL);
		return;
	}

	for (int i = 0; i < s.Num(); i++) {
		MapSkinType m = MapSkinType(FCString::Atoi(*(s[i])));
		UE_LOG(LogTemp, Error, TEXT("PD_MG_MapGenerationUtils::ParseTags s == 0 -> aplicando MapSkinType::%d"), (int)m);
		tags.Add(m);
	}

}


#pragma endregion


#pragma region PRELOADED ROOMS

/// Si la lectura del fichero falla, llamamos a esta funcion que nos da algunas habitaciones para que no pete el tema 
bool PD_MG_MapGenerationUtils::GetPreloadedData(TArray<RoomTemplateInfo> &roomTemplates) {

	TArray<MapSkinType> tags = TArray<MapSkinType>();
	tags.Add(MapSkinType::DUNGEON_NORMAL);
	RoomTemplateInfo r = RoomTemplateInfo("simple_Cuadrada", 0, tags, 8, 8);

	//FString s = "WWwwwwWW";
	r.AddNewLogicPosition(0, 0, 'W');
	r.AddNewLogicPosition(0, 1, 'w');
	r.AddNewLogicPosition(0, 2, 'w');
	r.AddNewLogicPosition(0, 3, 'w');
	r.AddNewLogicPosition(0, 4, 'w');
	r.AddNewLogicPosition(0, 5, 'w');
	r.AddNewLogicPosition(0, 6, 'w');
	r.AddNewLogicPosition(0, 7, 'W');

	//s = "W......W";
	r.AddNewLogicPosition(1, 0, 'W');
	r.AddNewLogicPosition(1, 1, '.');
	r.AddNewLogicPosition(1, 2, '.');
	r.AddNewLogicPosition(1, 3, '.');
	r.AddNewLogicPosition(1, 4, '.');
	r.AddNewLogicPosition(1, 5, '.');
	r.AddNewLogicPosition(1, 6, '.');
	r.AddNewLogicPosition(1, 7, 'W');

	//s = "w......w";
	r.AddNewLogicPosition(2, 0, 'w');
	r.AddNewLogicPosition(2, 1, '.');
	r.AddNewLogicPosition(2, 2, '.');
	r.AddNewLogicPosition(2, 3, '.');
	r.AddNewLogicPosition(2, 4, '.');
	r.AddNewLogicPosition(2, 5, '.');
	r.AddNewLogicPosition(2, 6, '.');
	r.AddNewLogicPosition(2, 7, 'w');

	//s = "w......w";
	r.AddNewLogicPosition(3, 0, 'w');
	r.AddNewLogicPosition(3, 1, '.');
	r.AddNewLogicPosition(3, 2, '.');
	r.AddNewLogicPosition(3, 3, '.');
	r.AddNewLogicPosition(3, 4, '.');
	r.AddNewLogicPosition(3, 5, '.');
	r.AddNewLogicPosition(3, 6, '.');
	r.AddNewLogicPosition(3, 7, 'w');
	
	//s = "w......w";
	r.AddNewLogicPosition(4, 0, 'w');
	r.AddNewLogicPosition(4, 1, '.');
	r.AddNewLogicPosition(4, 2, '.');
	r.AddNewLogicPosition(4, 3, '.');
	r.AddNewLogicPosition(4, 4, '.');
	r.AddNewLogicPosition(4, 5, '.');
	r.AddNewLogicPosition(4, 6, '.');
	r.AddNewLogicPosition(4, 7, 'w');
	
	//s = "w......w";
	r.AddNewLogicPosition(5, 0, 'w');
	r.AddNewLogicPosition(5, 1, '.');
	r.AddNewLogicPosition(5, 2, '.');
	r.AddNewLogicPosition(5, 3, '.');
	r.AddNewLogicPosition(5, 4, '.');
	r.AddNewLogicPosition(5, 5, '.');
	r.AddNewLogicPosition(5, 6, '.');
	r.AddNewLogicPosition(5, 7, 'w');
	
	//s = "w......w";
	r.AddNewLogicPosition(6, 0, 'W');
	r.AddNewLogicPosition(6, 1, '.');
	r.AddNewLogicPosition(6, 2, '.');
	r.AddNewLogicPosition(6, 3, '.');
	r.AddNewLogicPosition(6, 4, '.');
	r.AddNewLogicPosition(6, 5, '.');
	r.AddNewLogicPosition(6, 6, '.');
	r.AddNewLogicPosition(6, 7, 'W');

	//s = "WWwwwwWW";
	r.AddNewLogicPosition(7, 0, 'W');
	r.AddNewLogicPosition(7, 1, 'w');
	r.AddNewLogicPosition(7, 2, 'w');
	r.AddNewLogicPosition(7, 3, 'w');
	r.AddNewLogicPosition(7, 4, 'w');
	r.AddNewLogicPosition(7, 5, 'w');
	r.AddNewLogicPosition(7, 6, 'w');
	r.AddNewLogicPosition(7, 7, 'W');

	roomTemplates.Add(r);



	return true;

}



#pragma endregion



#pragma region PROCEDURAL GENERATION v0.1


/*
================ VARS ================
IN
W - width
H - height
LR - List Room

INTERNALS
M - mapa
LW - List Walls
LWC - List Walls Choosable
LRT - List Rooms Tested
LWT - List Walls Tested
LCT - List 'Casillas' Tested
C - 'Casilla'
R - current Room
W1, W2 - current Walls



============ Algorithm ==============

Dado un mapa M de W de ancho por H de alto y una lista de habitaciones LR.

// Init
1- Crear un tablero que ocupe WxH.
	1.1- Crear una lista de posiciones que indican pared LW, crear una lista de posiciones que indican paredes elegibles LWC.
	1.2- Crear una lista de habitación probadas LRT. Crear una lista de paredes probadas LWT.
	1.3- Crear una lista de casillas probadas LCT.



// Crear la habitacion inicial.
2- Elegir una casilla C no elegida de M contenida en el cuadrado (H/3, W/3), (2H/3, 2W/3) y añadirla a la lista LCT.

3- Elegir una habitación R de la lista LR no elegida, añadirla a LRT y comprobar que se puede colocar en C.
	3.1- Si no se puede colocar, ir al paso 2.

4- Si no hay combinación casilla C y habitación R posible, devolver FALLO.



// Una vez hay una habitación lista
5- Limpiar las listas LWT, LRT, LCT.

6- Elegir cualquier pared W1 de entre las de las lista de paredes eligibles LWC que no esté en LWT. Añadirla a la lista LWT. La habitación R es la que está asignada a la pared W1.

7- Elegir una habitación R2 de la lista LR no elegida. Añadira a la lista LWT.

8- Elegir una pared W2 de las paredes elegibles de R2.

9- Comprobar que se puede colocar la habitación R colindante respecto de las paredes W1, W2.
	9.1- Si no se puede colocar, ir al paso 6.
	9.2- Si todas las paredes de LWC están en LWT, devolver FIN.

10- Si se pudo colocar:
	10.1- añadir a LW todas las paredes de R2.
	10.2- añadir a LWC todas las paredes elegibles de R2.
	10.3- quitar de LWC las paredes de R2 que comparten habitación con otra habitación que no sea R2.
	10.4- quitar de LWC las paredes de R2 que en R estén en LW. Serán no elegibles.
	10.5- quitar de LWC las paredes que en su diagonal tengan alguna pared que esté en LW pero no en LWC.
	10.6- Cambiar la pared W1 de la habitación R y la pared W2 de la habitación R2 por una puerta D que conecta la habitación R2 con R2.
		10.6.1- Si puedo poner una puerta D2 en algun W contenido en LWC adjacente a la puerta D, quito el muro W y añado D2. Marco D y D2 como mismo Conector.

11- Si las condiciones del mapa permiten generar más habitaciones, hacer las modificaciones pertinentes en LR y LWC e ir al paso 5.
	11.1- Si las condiciones del mapa no permiten generar más habitaciones, devolver FIN.


*/


bool PD_MG_MapGenerationUtils::GenerateRandomStaticMap_v01(MapProceduralInfo &M, TArray<RoomTemplateInfo> & LR, int _Total_Height, int _Total_Width, PD_MatchConfigManager* MatchConfigMan, int numPlayers) {

	//M = MapProceduralInfo(_Total_Height, _Total_Width); //M - mapa
	//M.Total_Height = _Total_Height;
	//M.Total_Width = _Total_Width;
	//M.BOUNDING_BOX_TOP_LEFT = PD_MG_LogicPosition(_Total_Height, _Total_Width);
	//M.BOUNDING_BOX_DOWN_RIGHT = PD_MG_LogicPosition(0, 0);

	TArray<PD_MG_LogicPosition> LW; //LW - List Walls
	TArray<PD_MG_LogicPosition> LWC; //LWC - List Walls Choosable
	TArray<PD_MG_LogicPosition> LWT; //LWT - List Walls Tested

	TArray<RoomTemplateInfo> LRT; // LRT - List Rooms Tested
	TArray<PD_MG_LogicPosition> LCT; // LCT - List 'Casillas' Tested

	PD_MG_LogicPosition C; // C - 'Casilla'

	RoomTemplateInfo R; // R - current Room
	PD_MG_LogicPosition R_pivot; // pivote aleatorio de la habitacion R

	PD_MG_LogicPosition W1, W2; //W1, W2 - current Walls
	int totalRooms = NumberOfRoomsOnMatchConfig(MatchConfigMan->Get_MapSize(), numPlayers); //Habitaciones totales

	

	//MatchConfigMan->Get_MapSize();


	LW.Empty();
	LWC.Empty();
	LWT.Empty();
	LRT.Empty();
	LCT.Empty();


	do {
		// 2 Elegir una casilla C no elegida de M ...

		/*
		UE_LOG(LogTemp, Warning, TEXT("PD_MG_MapGenerationUtils::GenerateRandomStaticMap - Randon searching on X entre (%d, %d) e Y entre (%d, %d) "),
			(M.Total_Height / 3), (2 * M.Total_Height / 3), (M.Total_Width / 3), (2 * M.Total_Width / 3));
		*/

		int tile_x = FMath::RandRange((int)(M.Total_Height / 3), (int)(2 * M.Total_Height / 3));
		int tile_y = FMath::RandRange((int)(M.Total_Width / 3), (int)(2 * M.Total_Width / 3));
		C = PD_MG_LogicPosition(tile_x, tile_y);

		LCT.Add(C);

		// 3 Elegir una habitación R de la lista LR no elegida ...
		R = LR[FMath::RandRange(0, LR.Num() - 1)];
		LRT.Add(R);

		// elegimos un pivote de la habitacion R
		TArray<PD_MG_LogicPosition> posiblesPivotesR;
		R.MAP_DATA.GenerateKeyArray(posiblesPivotesR);
		R_pivot = posiblesPivotesR[FMath::RandRange(0, posiblesPivotesR.Num() - 1)];

		// comprobar que se puede colocar, si no volvemos a generar todo lo anterior
	} while (!Hard_Check_CanBeColocated(M, R, C, R_pivot));

	// Si se puede colocar, lo incrustamos en el mapa y actualizamos la lista de walls 
	//UE_LOG(LogTemp, Log, TEXT("PD_MG_MapGenerationUtils::GenerateRandomStaticMap Name of first room: %s - Adding to map ..."), *(R.NAME));
	M.AddRoomToMapAtLocation(R, C, R_pivot, 0);

	//UE_LOG(LogTemp, Log, TEXT("PD_MG_MapGenerationUtils::GenerateRandomStaticMap Adding %d openwalls "), R.OPEN_WALLS.Num());
	for (int i = 0; i < R.OPEN_WALLS.Num(); i++) {
		LWC.Add(Translate_LocalPosInRoom_To_MapPosition(R.OPEN_WALLS[i], C, R_pivot));
		LW.Add(Translate_LocalPosInRoom_To_MapPosition(R.OPEN_WALLS[i], C, R_pivot));

		//UE_LOG(LogTemp, Log, TEXT("PD_MG_MapGenerationUtils::GenerateRandomStaticMap Adding (%d,%d) openwalls "), Translate_LocalPosInRoom_To_MapPosition(R.OPEN_WALLS[i], C, R_pivot).GetX(), Translate_LocalPosInRoom_To_MapPosition(R.OPEN_WALLS[i], C, R_pivot).GetY());
	}

	//UE_LOG(LogTemp, Log, TEXT("PD_MG_MapGenerationUtils::GenerateRandomStaticMap Adding %d closedwalls "), R.CLOSED_WALLS.Num());
	for (int i = 0; i < R.CLOSED_WALLS.Num(); i++) {
		LW.Add(Translate_LocalPosInRoom_To_MapPosition(R.CLOSED_WALLS[i], C, R_pivot));

		//UE_LOG(LogTemp, Log, TEXT("PD_MG_MapGenerationUtils::GenerateRandomStaticMap Adding (%d,%d) CLOSED_WALLS "), Translate_LocalPosInRoom_To_MapPosition(R.CLOSED_WALLS[i], C, R_pivot).GetX(), Translate_LocalPosInRoom_To_MapPosition(R.CLOSED_WALLS[i], C, R_pivot).GetY());
	}



		// una vez actualizada la lista de walls, vamos a por otra habitación, la intentamos meter ... 
		// limpiamos las listas temporales
	LWT.Empty();
	LRT.Empty();
	LCT.Empty();

	int roomPlaced = 1;
	while (roomPlaced < totalRooms) {
		// Elegimos un wall de la lista de posibles
		switch (MATCHCONFIG_MISSIONTYPE(MatchConfigMan->Get_MissionType())) {
			case MATCHCONFIG_MISSIONTYPE::DefeatAll: {
				W1 = LWC[FMath::RandRange(0, LWC.Num() - 1)];
				break;
			}
			case MATCHCONFIG_MISSIONTYPE::DefeatBoss: {
				W1 = M.mapRooms[roomPlaced - 1].OPEN_WALLS[FMath::RandRange(0, M.mapRooms[roomPlaced - 1].OPEN_WALLS.Num() - 1)] + M.mapRooms[roomPlaced - 1].BOUNDING_BOX_TOP_LEFT;
				break;
			}
			case MATCHCONFIG_MISSIONTYPE::RecoverTreasure: {
				W1 = LWC[FMath::RandRange(0, LWC.Num() - 1)];
				break;
			}
		}

		// Elegimos una habitacion
		R = LR[FMath::RandRange(0, LR.Num() - 1)];
		LRT.Add(R);
		// de la habitacion elegimos un wall elegible
		W2 = R.OPEN_WALLS[FMath::RandRange(0, R.OPEN_WALLS.Num() - 1)];


		// hacemos matching
		if (Hard_Check_CanBeColocated(M, R, W1, W2)) {

			//UE_LOG(LogTemp, Log, TEXT("PD_MG_MapGenerationUtils::GenerateRandomStaticMap W1(%d,%d) W2(%d,%d)"), W1.GetX(), W1.GetY(), W2.GetX(), W2.GetY());
			//UE_LOG(LogTemp, Log, TEXT("PD_MG_MapGenerationUtils::GenerateRandomStaticMap Name of next room: %s"), *(R.NAME));
			
			M.AddRoomToMapAtLocation(R, W1, W2, roomPlaced);


			//meter puerta y si se puede, puerta doble
			//Put_Door_Tryng_doubleDoor_at(M, W1);
			//if (Put_Door_Tryng_doubleDoor_at(M, W1)) {
				//UE_LOG(LogTemp, Log, TEXT("PD_MG_MapGenerationUtils::GenerateRandomStaticMap Double door!"));
			//}


			for (int i = 0; i < R.OPEN_WALLS.Num(); i++) {
				LWC.Add(Translate_LocalPosInRoom_To_MapPosition(R.OPEN_WALLS[i], W1, W2));
				LW.Add(Translate_LocalPosInRoom_To_MapPosition(R.OPEN_WALLS[i], W1, W2));

				//UE_LOG(LogTemp, Log, TEXT("PD_MG_MapGenerationUtils::GenerateRandomStaticMap Adding (%d,%d) openwalls "), Translate_LocalPosInRoom_To_MapPosition(R.OPEN_WALLS[i], W1, W2).GetX(), Translate_LocalPosInRoom_To_MapPosition(R.OPEN_WALLS[i], W1, W2).GetY());
			}

			//UE_LOG(LogTemp, Log, TEXT("PD_MG_MapGenerationUtils::GenerateRandomStaticMap Adding %d closedwalls "), R.CLOSED_WALLS.Num());
			for (int i = 0; i < R.CLOSED_WALLS.Num(); i++) {
				LW.Add(Translate_LocalPosInRoom_To_MapPosition(R.CLOSED_WALLS[i], W1, W2));

				//UE_LOG(LogTemp, Log, TEXT("PD_MG_MapGenerationUtils::GenerateRandomStaticMap Adding (%d,%d) CLOSED_WALLS "), Translate_LocalPosInRoom_To_MapPosition(R.CLOSED_WALLS[i], W1, W2).GetX(), Translate_LocalPosInRoom_To_MapPosition(R.CLOSED_WALLS[i], W1, W2).GetY());
			}
			
			roomPlaced++;


			//M.ShowMap();
		}
	}

	MarkARoomAsSpawingRoom_v01(M, MatchConfigMan->Get_MissionType());

	//M.ShowMapOnBoundingBox();
	M.TrimBoundingBoxOfRoomsInMap();
	return true;
}


void PD_MG_MapGenerationUtils::MarkARoomAsSpawingRoom_v01(MapProceduralInfo &M, MATCHCONFIG_MISSIONTYPE missionType) {

	//PD_MG_LogicPosition p = PD_MG_LogicPosition(FMath::RandRange(0, M.Total_Height), FMath::RandRange(0, M.Total_Width));
	
	/*UE_LOG(LogTemp, Log, TEXT("PD_MG_MapGenerationUtils::MarkARoomAsSpawingRoom testing Spawn point on (%d,%d)"), p.GetX(), p.GetY());


	while (!(M.mapElements.Contains(p) && M.mapElements[p] == StaticMapElement::NORMAL_TILE)) {
		//p = PD_MG_LogicPosition(FMath::RandRange(0, M.BOUNDING_BOX_DOWN_RIGHT.GetX()- M.BOUNDING_BOX_TOP_LEFT.GetX()), FMath::RandRange(0, M.BOUNDING_BOX_DOWN_RIGHT.GetY() - M.BOUNDING_BOX_TOP_LEFT.GetY()));
		p = PD_MG_LogicPosition(FMath::RandRange(0, M.Total_Height), FMath::RandRange(0, M.Total_Width));
		UE_LOG(LogTemp, Log, TEXT("PD_MG_MapGenerationUtils::MarkARoomAsSpawingRoom testing Spawn point on (%d,%d)"), p.GetX(), p.GetY());
	}
	UE_LOG(LogTemp, Log, TEXT("posicion de spawn %i,%i"), p.GetX(), p.GetY());
	/*for (int i = 0; i < M.mapRooms.Num(); i++)
	{
		if(M.mapRooms[i].NORMAL_TILES.Contains(p))  // No funciona porque p es global y las de la sala son locales
			M.SPAWN_ID = M.mapRooms[i].ID;
	}*/
	PD_MG_LogicPosition p, worldP;
	
	switch (MATCHCONFIG_MISSIONTYPE(missionType)) {
		case MATCHCONFIG_MISSIONTYPE::DefeatAll: {
			int i = FMath::RandRange(0, M.mapRooms.Num() - 1);
			M.SPAWN_ID = M.mapRooms[i].ID;
			int j = (FMath::RandRange(0, M.mapRooms[i].NORMAL_TILES.Num() - 1));
			 p = M.mapRooms[i].NORMAL_TILES[j];

			//Transformamos la posición en posición global
			 worldP = M.mapRooms[i].BOUNDING_BOX_TOP_LEFT + p;
			break;
		}
		case MATCHCONFIG_MISSIONTYPE::DefeatBoss: {
			M.SPAWN_ID = M.mapRooms[0].ID;
			int j = (FMath::RandRange(0, M.mapRooms[0].NORMAL_TILES.Num() - 1));
			 p = M.mapRooms[0].NORMAL_TILES[j];

			//Transformamos la posición en posición global
			 worldP = M.mapRooms[0].BOUNDING_BOX_TOP_LEFT + p;
			break;
		}
		case MATCHCONFIG_MISSIONTYPE::RecoverTreasure: {
			int i = FMath::RandRange(0, M.mapRooms.Num() - 1);
			M.SPAWN_ID = M.mapRooms[i].ID;
			int j = (FMath::RandRange(0, M.mapRooms[i].NORMAL_TILES.Num() - 1));
			 p = M.mapRooms[i].NORMAL_TILES[j];

			//Transformamos la posición en posición global
			 worldP = M.mapRooms[i].BOUNDING_BOX_TOP_LEFT + p;
			break;
		}
	}
	
	//M.mapElements[worldP] = StaticMapElement::SPAWN_POINT;
}

FString PD_MG_MapGenerationUtils::EnemiesGeneration_v01(MapProceduralInfo &M) {
	/* 1) Primero definimos el número de tiles por enemigo, despues cogemos las keys del tMap de las salas.
	   2) Las recorremos, descartando la de spawn, contamos los tiles normales que tiene.
	   3) Dentro de cada sala elegimos una posición al azar y la seleccionamos, si es válida( si no hay otro) para poner un enemigo en ella.
	   4) Una vez recorridas todas las salas, recorremos el array de las posiciones de enemigos y guardamos uno al azar en cada una de ellas.
	*/

	int TilesPerEnemy = 60, enemy;
	int totalEnemies;
	TArray <PD_MG_LogicPosition> enemies;
	UE_LOG(LogTemp, Log, TEXT("Generacion de enemigos"));
	UE_LOG(LogTemp, Log, TEXT("Sala de spawn %i"), M.SPAWN_ID);
	for (int i = 0; i < M.mapRooms.Num(); i++)
	{
		if (M.mapRooms[i].ID!= M.SPAWN_ID) {

			totalEnemies = M.mapRooms[i].NORMAL_TILES.Num();
			totalEnemies /= TilesPerEnemy;
			int j = 0;
			UE_LOG(LogTemp, Log, TEXT("Sala %d visitada, tiene %i enemigos, con un total de tiles disponibles %i"), M.mapRooms[i].ID, totalEnemies, M.mapRooms[i].NORMAL_TILES.Num());
			while (j < totalEnemies)
			{
				int pos = FMath::RandRange(0, M.mapRooms[i].NORMAL_TILES.Num()-1);//Cogemos una posicion aleatoria
				if (!enemies.Contains(M.mapRooms[i].BOUNDING_BOX_TOP_LEFT + M.mapRooms[i].NORMAL_TILES[pos])) {
					UE_LOG(LogTemp, Log, TEXT("Poniendo enemigo %i en sala %d"), j, M.mapRooms[i].ID);
					//UE_LOG(LogTemp, Log, TEXT("PD_MG_MapGenerationUtils::MarkARoomAsSpawingRoom testing Spawn point on (%d,%d), inicio de la sala (%d,%d)"), M.mapRooms[keys[i]].BOUNDING_BOX_TOP_LEFT.GetX(), M.mapRooms[keys[i]].BOUNDING_BOX_TOP_LEFT.GetY());
					enemies.Add(M.mapRooms[i].BOUNDING_BOX_TOP_LEFT + M.mapRooms[i].NORMAL_TILES[pos]);///falta pasar de posición local a posicion global en el mapa
					j++;
				}
			}
		}
	}
	FString enemiesString = FString::FromInt(enemies.Num()) + '\n';
	for (int i = 0; i < enemies.Num(); i++)
	{
		///TODO: Cuando se cambie el enumerado de los characters, hay que cambiar aqui para que se spawneen diferentes enemigos. 
		enemy = rand() % 2 + 2;///Esto es para conseguir un enemigo aleatorio rand da un número entre 0 y 1, y le sumamos 2, para que sea 2 o 3.
		enemiesString += FString::FromInt(enemy) + ':' + FString::FromInt(enemies[i].GetX()) + ',' + FString::FromInt(enemies[i].GetY()) + '\n';
	}
	UE_LOG(LogTemp, Log, TEXT("Enemigos %s"), *enemiesString);
	return enemiesString;
}

#pragma endregion

#pragma region PROCEDURAL GENERATION v0.2

bool PD_MG_MapGenerationUtils::GenerateRandomStaticMap_v02(MapProceduralInfo &M, TArray<RoomTemplateInfo> &LR, int _Total_Height, int _Total_Width, PD_MatchConfigManager* MatchConfigMan, int numPlayers)
{

	/////////////////////
	// INIT LOCAL VARS //
	/////////////////////

	UE_LOG(LogTemp, Log, TEXT("PD_MG_MapGenerationUtils::GenerateRandomStaticMap_v02 init vars"));


#pragma region Init local vars 
	// Esta lista va a almacenar TODAS las paredes. Hay que añadir cada IDRoom a la que pertenece a mano. 
	TMap<PD_MG_LogicPosition, LogicPositionAmplified> LW; //LW - List Walls

	// Esta lista guarda las paredes que pueden ser elegidas en la iteracion actual para ser candidatas como world position para el matching
	TArray<PD_MG_LogicPosition> LWC; //LWC - List Walls Choosable
	
	// Esta lista guarda las paredes que han sido elegidas al menos una vez en la iteracion actual para ser candidatas como world position para el matching
	TArray<PD_MG_LogicPosition> LWT; //LWT - List Walls Tested


	// lista de habitaciones que has probado en al iteracion actual
	TArray<RoomTemplateInfo> LRT; // LRT - List Rooms Tested

	// lista de casillas que has probado en el paso de ELEGIR HABITACION INICIAL para no repetir casilla. Solo se usa en el paso ELEGIR HABITACION INICIAL.
	TArray<PD_MG_LogicPosition> LCT; // LCT - List 'Casillas' Tested

	// Casilla actual en el posicion de mundo para el matching. Solo se usa en el paso ELEGIR HABITACION INICIAL.
	PD_MG_LogicPosition C; // C - 'Casilla'

	// la habitacion candidata actual
	RoomTemplateInfo R; // R - current Room
	// posicion local de la habitacion candidata actual (tambien llamando pivote de R). Solo se usa en el paso ELEGIR HABITACION INICIAL.
	PD_MG_LogicPosition R_pivot; // pivote aleatorio de la habitacion R

	// posiciones de muros para hacer el matching en iteraciones posteriores. 
	// - W1 es un muro con posicion en el mundo de una habitacion ya colocada
	// - W2 es un muro con posicion local de la habitacion candidata R. 
	PD_MG_LogicPosition W1, W2; //W1, W2 - current Walls
	

	TArray<LogicPositionAmplified> LDoors;
	TArray<LogicPositionAmplified> LInteractuables;

	
	// Indica el numero de habitaciones que se generaran. También indica el numero de vueltas que dará el algoritmo para generar las habitaciones como minimo. 
	// Nota: puede dar mas vueltas si la habitacion candidata actual no hace matching respecto de las posiciones de mundo y locales elegidas. (es lo común)
	int totalRooms = NumberOfRoomsOnMatchConfig(MatchConfigMan->Get_MapSize(), numPlayers); //Habitaciones totales


	LW.Empty();
	LWC.Empty();
	LWT.Empty();
	LRT.Empty();
	LCT.Empty();
	LDoors.Empty();
	LInteractuables.Empty();
#pragma endregion 

	/////////////////////
	// INIT NETMAPDATA //
	/////////////////////

#pragma region INIT MAP DATA
	UE_LOG(LogTemp, Log, TEXT("PD_MG_MapGenerationUtils::GenerateRandomStaticMap_v02 init net map data"));

	M.NETMAPDATA->MISSION_TYPE = (int)MatchConfigMan->Get_MissionType();
	M.NETMAPDATA->PARSER_VERSION = "v0.2";
	M.NETMAPDATA->MAP_SIZE_IN_LOGIC_POSITIONS = 0;

	M.NETMAPDATA->skinByRoom = TArray<uint16>();
	M.NETMAPDATA->IDRoomSpawn = 0;
	M.NETMAPDATA->roomsAdj = TArray<uint16>();
	M.NETMAPDATA->roomComposition = TArray<uint32>();
	M.NETMAPDATA->wallComposition = TArray<uint32>();
	M.NETMAPDATA->doorComposition = TArray<uint32>();
	M.NETMAPDATA->interactuableId = TArray<uint32>();
	M.NETMAPDATA->interactuableComposition = TArray<uint16>();
	M.NETMAPDATA->enemyComposition = TArray<uint32>();
	
#pragma endregion

	////////////////////////////////
	//  ELEGIR HABITACION INICIAL //
	////////////////////////////////

	UE_LOG(LogTemp, Log, TEXT("PD_MG_MapGenerationUtils::GenerateRandomStaticMap_v02 choose initial room"));


#pragma region elegir habitacion inicial 

	do {
		// 2 Elegir una casilla C no elegida de M ...

		int tile_x = FMath::RandRange((int)(M.Total_Height / 3), (int)(2 * M.Total_Height / 3));
		int tile_y = FMath::RandRange((int)(M.Total_Width / 3), (int)(2 * M.Total_Width / 3));
		C = PD_MG_LogicPosition(tile_x, tile_y);

		LCT.Add(C);

		// 3 Elegir una habitación R de la lista LR no elegida ...
		R = LR[FMath::RandRange(0, LR.Num() - 1)];
		LRT.Add(R);

		// elegimos un pivote de la habitacion R
		TArray<PD_MG_LogicPosition> posiblesPivotesR;
		R.MAP_DATA.GenerateKeyArray(posiblesPivotesR);
		R_pivot = posiblesPivotesR[FMath::RandRange(0, posiblesPivotesR.Num() - 1)];

		// comprobar que se puede colocar, si no volvemos a generar todo lo anterior
	} while (!Hard_Check_CanBeColocated(M, R, C, R_pivot));


	// Si se puede colocar, lo incrustamos en el mapa y actualizamos la lista de walls 
	R.ChoosedTag = R.ChooseOneMapSkin();
	M.AddRoomToMapAtLocation(R, C, R_pivot, 0);









	for (int i = 0; i < R.OPEN_WALLS.Num(); i++) {
		LWC.AddUnique(R.OPEN_WALLS[i] + R.BOUNDING_BOX_TOP_LEFT);

		PD_MG_LogicPosition lp = R.OPEN_WALLS[i] + R.BOUNDING_BOX_TOP_LEFT;
		LogicPositionAmplified logposAmp = LogicPositionAmplified(lp);
		logposAmp.AddInfo(0);
		LW.Add(lp,logposAmp);
	}

	for (int i = 0; i < R.CLOSED_WALLS.Num(); i++) {
		PD_MG_LogicPosition lp = R.CLOSED_WALLS[i] + R.BOUNDING_BOX_TOP_LEFT;
		LogicPositionAmplified logposAmp = LogicPositionAmplified(lp);
		logposAmp.AddInfo(0);
		LW.Add(lp, logposAmp);
	}

#pragma endregion 



	//////////////////////////////////////
	//  ELEGIR EL RESTO DE HABITACIONES //
	//////////////////////////////////////


	UE_LOG(LogTemp, Log, TEXT("PD_MG_MapGenerationUtils::GenerateRandomStaticMap_v02 choose rest of the rooms"));


#pragma region elegir el resto de habitaciones 

	// una vez actualizada la lista de walls, vamos a por otra habitación, la intentamos meter ... 
	// limpiamos las listas temporales
	LWT.Empty();
	LRT.Empty();
	LCT.Empty();

	int roomPlaced = 1;
	while (roomPlaced < totalRooms) {
		// Elegimos un wall de la lista de posibles
		switch (MATCHCONFIG_MISSIONTYPE(MatchConfigMan->Get_MissionType())) {
		case MATCHCONFIG_MISSIONTYPE::DefeatAll: {
			W1 = LWC[FMath::RandRange(0, LWC.Num() - 1)];
			break;
		}
		case MATCHCONFIG_MISSIONTYPE::DefeatBoss: {
			W1 = M.mapRooms[roomPlaced - 1].OPEN_WALLS[FMath::RandRange(0, M.mapRooms[roomPlaced - 1].OPEN_WALLS.Num() - 1)] + M.mapRooms[roomPlaced - 1].BOUNDING_BOX_TOP_LEFT;
			break;
		}
		case MATCHCONFIG_MISSIONTYPE::RecoverTreasure: {
			W1 = LWC[FMath::RandRange(0, LWC.Num() - 1)];
			break;
		}
		}


		RoomTemplateInfo Room_of_W1;

		// de cual habitacion es W1?
		for (int i = 0; i < M.mapRooms.Num(); i++)
		{
			bool found = false;
			for (int j = 0; j < M.mapRooms[i].OPEN_WALLS.Num(); j++)
			{
				if ((M.mapRooms[i].OPEN_WALLS[j] + M.mapRooms[i].BOUNDING_BOX_TOP_LEFT == W1)) {
					Room_of_W1 = M.mapRooms[i];
					found = true;
					break;
				}
			}
			if (found) break;
		}

		// cuales son las posibles habitaciones que casan con el MapSkinType del Room_of_W1?
		MapSkinType mST = ChoosesSkinRules(Room_of_W1, Room_of_W1.TAGS);

		TArray<RoomTemplateInfo*> posiblesR = mapGenerator->Map_RoomTemplate_By_MapSkinsType[mST];

		if (posiblesR.Num() == 0) continue;

		// Elegimos una habitacion que case bien 
		R = *(posiblesR[FMath::RandRange(0, posiblesR.Num() - 1)]);
		LRT.Add(R);
		// de la habitacion elegimos un wall elegible
		W2 = R.OPEN_WALLS[FMath::RandRange(0, R.OPEN_WALLS.Num() - 1)];

		UE_LOG(LogTemp, Log, TEXT("PD_MG_MapGenerationUtils::GenerateRandomStaticMap_v02  .... checking %s "), *(R.NAME));

		// hacemos matching
		if (Hard_Check_CanBeColocated(M, R, W1, W2)) {

			//UE_LOG(LogTemp, Log, TEXT("PD_MG_MapGenerationUtils::GenerateRandomStaticMap W1(%d,%d) W2(%d,%d)"), W1.GetX(), W1.GetY(), W2.GetX(), W2.GetY());
			//UE_LOG(LogTemp, Log, TEXT("PD_MG_MapGenerationUtils::GenerateRandomStaticMap Name of next room: %s"), *(R.NAME));

			M.AddRoomToMapAtLocation(R, W1, W2, roomPlaced);


			//meter puerta y si se puede, puerta doble
			Put_Door_Tryng_doubleDoor_at(M, W1, R.ID, LDoors, LInteractuables);
			
			UE_LOG(LogTemp, Warning, TEXT("PD_MG_MapGenerationUtils::GenerateRandomStaticMap_v02  .... LDoors (%d,%d) - %d - %d"),
				LDoors[LDoors.Num() - 1].logicPosition.GetX(), LDoors[LDoors.Num() - 1].logicPosition.GetY(), LDoors[LDoors.Num() - 1].info[0], LDoors[LDoors.Num() - 1].info[1]);
			UE_LOG(LogTemp, Warning, TEXT("PD_MG_MapGenerationUtils::GenerateRandomStaticMap_v02  .... LInteractuables (%d,%d) - %d - %d"),
				LInteractuables[LInteractuables.Num() - 1].logicPosition.GetX(), LInteractuables[LInteractuables.Num() - 1].logicPosition.GetY(), LInteractuables[LInteractuables.Num() - 1].info[0], LInteractuables[LInteractuables.Num() - 1].info[1]);

			//if (Put_Door_Tryng_doubleDoor_at(M, W1)) {
			//UE_LOG(LogTemp, Log, TEXT("PD_MG_MapGenerationUtils::GenerateRandomStaticMap Double door!"));
			//}


			for (int i = 0; i < R.OPEN_WALLS.Num(); i++) {
				LWC.AddUnique(R.OPEN_WALLS[i] + R.BOUNDING_BOX_TOP_LEFT);
	

				PD_MG_LogicPosition lp = R.OPEN_WALLS[i] + R.BOUNDING_BOX_TOP_LEFT;
				if (W1 == lp) {
					if (LW.Contains(lp))
						LW.Remove(lp);
					continue;
				}
				if (LW.Contains(lp)) {
					LW[lp].AddInfo(R.ID);
				}
				else {
					LogicPositionAmplified logposAmp = LogicPositionAmplified(lp);
					logposAmp.AddInfo(R.ID);
					LW.Add(lp, logposAmp);
				}
			}

			//UE_LOG(LogTemp, Log, TEXT("PD_MG_MapGenerationUtils::GenerateRandomStaticMap Adding %d closedwalls "), R.CLOSED_WALLS.Num());
			for (int i = 0; i < R.CLOSED_WALLS.Num(); i++) {
				PD_MG_LogicPosition lp = R.CLOSED_WALLS[i] + R.BOUNDING_BOX_TOP_LEFT;
				if (LW.Contains(lp)) {
					LW[lp].AddInfo(R.ID);
				}
				else {
					LogicPositionAmplified logposAmp = LogicPositionAmplified(lp);
					logposAmp.AddInfo(R.ID);
					LW.Add(lp, logposAmp);
				}
			}
	
			roomPlaced++;

			
			//M.ShowMap();
		}
	}

	MarkARoomAsSpawingRoom_v02(M, MatchConfigMan->Get_MissionType());

	EnemiesGeneration_v02(M, MatchConfigMan, numPlayers);
	InteractuablesGeneration_v02(M, MatchConfigMan, numPlayers, LInteractuables);



#pragma endregion 

	////////////////////////////
	// RELLENAR EL NETMAPDATA //
	////////////////////////////


	UE_LOG(LogTemp, Log, TEXT("PD_MG_MapGenerationUtils::GenerateRandomStaticMap_v02 fill net map data "));
	TArray <LogicPositionAmplified> walls = TArray <LogicPositionAmplified>();
	LW.GenerateValueArray(walls);
	Fill_NETMAPDATA_from(M, walls, LDoors, LInteractuables);


	return false;
}

void PD_MG_MapGenerationUtils::MarkARoomAsSpawingRoom_v02(MapProceduralInfo &M, MATCHCONFIG_MISSIONTYPE missionType) {
	PD_MG_LogicPosition p, worldP;

	switch (MATCHCONFIG_MISSIONTYPE(missionType)) {
		case MATCHCONFIG_MISSIONTYPE::DefeatAll: {
			int i = FMath::RandRange(0, M.mapRooms.Num() - 1);
			M.SPAWN_ID = M.mapRooms[i].ID;
			break;
		}
		case MATCHCONFIG_MISSIONTYPE::DefeatBoss: {
			M.SPAWN_ID = M.mapRooms[0].ID;
			break;
		}
		case MATCHCONFIG_MISSIONTYPE::RecoverTreasure: {
			int i = FMath::RandRange(0, M.mapRooms.Num() - 1);
			M.SPAWN_ID = M.mapRooms[i].ID;
			break;
		}
	}
}

bool PD_MG_MapGenerationUtils::EnemiesGeneration_v02(MapProceduralInfo &M, PD_MatchConfigManager* MatchConfigMan, int numPlayers) { 

	/*	1 - Calculamos cuantos enemigos tiene la mazmorra en función del número de jugaores, número de salas, y dificultad.
		2 - Determinamos el tipo de misión.
		3 - Hacemos el calculo del reparto de enemigos por habitación.
		4 - Recorremos las salas, generando a los enemigos y guardandolos en la estructura NETMAPDATA.	
	*/
	int difficulty = DifficultyDungeon(MatchConfigMan->Get_Difficulty());
	int totalEnemies = (numPlayers * difficulty * M.mapRooms.Num())/2;///Esto hay que ir balanceandolo
	int enemy;
	MATCHCONFIG_MISSIONTYPE matchConfig_MATCHCONFIG_MISSIONTYPE = MatchConfigMan->Get_MissionType();
	TArray <uint8> EnemiesPerRoom;
	
	switch (matchConfig_MATCHCONFIG_MISSIONTYPE)
	{
		case MATCHCONFIG_MISSIONTYPE::DefeatBoss: {
			UE_LOG(LogTemp, Log, TEXT("Poniendo enemigo"));
			for (int i = 1; i < M.mapRooms.Num(); i++)
			{
				int enemiesIn = totalEnemies / (M.mapRooms.Num() - 1);
				enemiesIn = enemiesIn - (enemiesIn*(1 / (M.mapRooms.Num()-1))*((M.mapRooms.Num() - 1) / 2 - i));
				EnemiesPerRoom.Add(enemiesIn);
			}
			for (int i = 0; i < EnemiesPerRoom.Num(); i++)
			{
				int j = 0;//enemigos generados en esta sala
				int k = i + 1;//sala en la que nos encontramos
				while (j < EnemiesPerRoom[i])
				{
					int pos = FMath::RandRange(0, M.mapRooms[k].NORMAL_TILES.Num() - 1);//Cogemos una posicion aleatoria
					if (!M.enemies.Contains(M.mapRooms[k].BOUNDING_BOX_TOP_LEFT + M.mapRooms[k].NORMAL_TILES[pos])) {
						UE_LOG(LogTemp, Log, TEXT("Poniendo enemigo %i en sala %d"), j, M.mapRooms[k].ID);
						//UE_LOG(LogTemp, Log, TEXT("PD_MG_MapGenerationUtils::MarkARoomAsSpawingRoom testing Spawn point on (%d,%d), inicio de la sala (%d,%d)"), M.mapRooms[keys[i]].BOUNDING_BOX_TOP_LEFT.GetX(), M.mapRooms[keys[i]].BOUNDING_BOX_TOP_LEFT.GetY());
						enemy = rand() % 3 + 2;///Con esto conseguimos un enemigo aleatorio de la lista de enemigos
						M.enemies.Add(M.mapRooms[k].BOUNDING_BOX_TOP_LEFT + M.mapRooms[k].NORMAL_TILES[pos], enemy);///falta pasar de posición local a posicion global en el mapa
						j++;
					}
				}
			}
			//Instanciamos el boss
			bool bossinstanciated = false;
			while (!bossinstanciated) {
				int pos = FMath::RandRange(0, M.mapRooms[M.mapRooms.Num() - 1].NORMAL_TILES.Num() - 1);
				if (!M.enemies.Contains(M.mapRooms[M.mapRooms.Num() - 1].BOUNDING_BOX_TOP_LEFT + M.mapRooms[M.mapRooms.Num() - 1].NORMAL_TILES[pos])) {
					UE_LOG(LogTemp, Log, TEXT("Poniendo boss en sala %d"), M.mapRooms[M.mapRooms.Num() - 1].ID);
					//UE_LOG(LogTemp, Log, TEXT("PD_MG_MapGenerationUtils::MarkARoomAsSpawingRoom testing Spawn point on (%d,%d), inicio de la sala (%d,%d)"), M.mapRooms[keys[i]].BOUNDING_BOX_TOP_LEFT.GetX(), M.mapRooms[keys[i]].BOUNDING_BOX_TOP_LEFT.GetY());
					enemy = 5;///Con esto conseguimos un enemigo aleatorio de la lista de enemigos
					M.enemies.Add(M.mapRooms[M.mapRooms.Num() - 1].BOUNDING_BOX_TOP_LEFT + M.mapRooms[M.mapRooms.Num() - 1].NORMAL_TILES[pos], enemy);///falta pasar de posición local a posicion global en el mapa
					bossinstanciated = true;
				}
			}
			return true;
			break;
		}
		case MATCHCONFIG_MISSIONTYPE::DefeatAll:
			return true;
			break;
		case MATCHCONFIG_MISSIONTYPE::RecoverTreasure:
			return true;
			break;
		default:
			return false;
			break;
	}
}

bool PD_MG_MapGenerationUtils::InteractuablesGeneration_v02(MapProceduralInfo & M, PD_MatchConfigManager * MatchConfigMan, int numPlayers, TArray<LogicPositionAmplified> LInteractuables)
{
	return false;
}

#pragma endregion


#pragma region PROCEDURAL GENERATION UTILS 

int PD_MG_MapGenerationUtils::DifficultyDungeon(MATCHCONFIG_DIFFICULTY matchConfig_MATCHCONFIG_MAPDIFFICULTY) {
	switch (matchConfig_MATCHCONFIG_MAPDIFFICULTY)
		{
		case MATCHCONFIG_DIFFICULTY::EASY_DIFFICULTY:
			return 1;
		case MATCHCONFIG_DIFFICULTY::NORMAL_DIFFICULTY:
			return 2;
		case MATCHCONFIG_DIFFICULTY::DIFFICULT_DIFFICULTY:
			return 3;
		default:
			return 1;
		}
}


int PD_MG_MapGenerationUtils::NumberOfRoomsOnMatchConfig(MATCHCONFIG_MAPSIZE matchConfig_MATCHCONFIG_MAPSIZE, int numberOfPlayers)
{
	switch (matchConfig_MATCHCONFIG_MAPSIZE) {
	case MATCHCONFIG_MAPSIZE::SMALL_SIZE: {
		return  1 * numberOfPlayers + 2;
	}
	case MATCHCONFIG_MAPSIZE::NORMAL_SIZE: {
		return 1.5f * numberOfPlayers + 2;
	}
	case MATCHCONFIG_MAPSIZE::LARGE_SIZE: {
		return 2 * numberOfPlayers + 2;
	}
	default:
		return 4;
	}
}


MapSkinType PD_MG_MapGenerationUtils::ChoosesSkinRules(RoomTemplateInfo &R, TArray<MapSkinType> & ChoosableMapSkins)
{
	TArray<MapSkinType> choosables = TArray<MapSkinType>();

	for (int i = 0; i < ChoosableMapSkins.Num(); i++) {
		UE_LOG(LogTemp, Error, TEXT("PD_MG_MapGenerationUtils::ChoosesSkinRules -> ChoosableMapSkins %d"), (int)ChoosableMapSkins[i]);

		if (R.MatchSkins(R.ChoosedTag, ChoosableMapSkins[i])) {
			choosables.Add(ChoosableMapSkins[i]);
		}
	}
	MapSkinType m = choosables[FMath::RandRange(0, choosables.Num() - 1)];
	UE_LOG(LogTemp, Error, TEXT("PD_MG_MapGenerationUtils::ChoosesSkinRules -> choosed %d"), (int)m);
	return m;
}



// comprueba que la habitacion R se puede colocar en el mapa M haciendo coincidientes los puntos C y R_pivot, siendo C un punto en el mapa y R_pivot una posicion local a R.
bool PD_MG_MapGenerationUtils::Hard_Check_CanBeColocated(MapProceduralInfo &M, RoomTemplateInfo &R, PD_MG_LogicPosition C, PD_MG_LogicPosition R_pivot) {

	if (!MapCanContainsRoom(M, R, C, R_pivot)) return false;


	PD_MG_LogicPosition W = Translate_LocalPosInRoom_To_MapPosition(R.BOUNDING_BOX_TOP_LEFT, C, R_pivot);


	PD_MG_LogicPosition Waux = Translate_LocalPosInRoom_To_MapPosition(R.BOUNDING_BOX_DOWN_RIGHT, C, R_pivot);
	UE_LOG(LogTemp, Log, TEXT("PD_MG_MapGenerationUtils::Hard_Check_CanBeColocated testing from /\\(%d,%d) to \\/(%d,%d)"), W.GetX(), W.GetY(), Waux.GetX(), Waux.GetY());

	for (int i = 0; i < R.LOCAL_LOGIC_POSITIONS_ON_ROOM.Num(); i++) {

		W = Translate_LocalPosInRoom_To_MapPosition(R.LOCAL_LOGIC_POSITIONS_ON_ROOM[i], C, R_pivot);
		//UE_LOG(LogTemp, Log, TEXT("PD_MG_MapGenerationUtils::Hard_Check_CanBeColocated testing W(%d,%d)"), W.GetX(), W.GetY());

		if (M.mapElements.Contains(W) &&
			(M.mapElements[W] == StaticMapElementInRoomTemplate::NORMAL_TILE ||
				M.mapElements[W] == StaticMapElementInRoomTemplate::SPECIAL_TILE ||
				M.mapElements[W] == StaticMapElementInRoomTemplate::EMPTY)) {
			return false;
		}
	}

	return true;

}

bool PD_MG_MapGenerationUtils::MapCanContainsRoom(MapProceduralInfo &M, RoomTemplateInfo &R, PD_MG_LogicPosition C, PD_MG_LogicPosition R_pivot) {

	PD_MG_LogicPosition cornerTopLeft = Translate_LocalPosInRoom_To_MapPosition(R.BOUNDING_BOX_TOP_LEFT, C, R_pivot);
	PD_MG_LogicPosition cornerDownRight = Translate_LocalPosInRoom_To_MapPosition(R.BOUNDING_BOX_DOWN_RIGHT, C, R_pivot);

	if (cornerTopLeft.GetX() < 0 || cornerTopLeft.GetY() < 0 ||
		cornerDownRight.GetX() >= (int)M.Total_Height || cornerDownRight.GetY() >= (int)M.Total_Width) {
		return false;
	}

	return true;
}

PD_MG_LogicPosition PD_MG_MapGenerationUtils::Translate_LocalPosInRoom_To_MapPosition(PD_MG_LogicPosition localPos, PD_MG_LogicPosition C, PD_MG_LogicPosition R_pivot) {
	return localPos - R_pivot + C;
}
bool PD_MG_MapGenerationUtils::Put_Door_Tryng_doubleDoor_at(MapProceduralInfo &M, PD_MG_LogicPosition W1, uint8 IDR, TArray<LogicPositionAmplified> &LD, TArray<LogicPositionAmplified> &LInteractuables) {

	//Actualizamos el array de adyacentes de la sala de la que le pasamos el identificador
	uint8 IDR2 = 0;
	TArray <uint8> adR1 = TArray <uint8>();
	for (int i = 0; i < M.mapRooms.Num(); i++)
	{
		for (int j = 0; j < M.mapRooms[i].OPEN_WALLS.Num() && (M.mapRooms[i].ID != IDR); j++)
		{
			if ((M.mapRooms[i].OPEN_WALLS[j] + M.mapRooms[i].BOUNDING_BOX_TOP_LEFT == W1)) {
				IDR2 = M.mapRooms[i].ID;
				if (M.mapAdj.Contains(IDR2)) {
					adR1 = M.mapAdj[IDR2];
					if (!adR1.Contains(IDR))
						adR1.Add(IDR);
				}
				else {
					adR1.Add(IDR);
				}
				M.mapAdj.Emplace(IDR2, adR1);

				//UE_LOG(LogTemp, Log, TEXT("Mi id muchachote %d, numero de rooms %d, la otra id es %d"), IDR2, M.mapRooms.Num(), IDR);
			}
		}
	}

	//Actualizamos el array de adyacentes de la otra sala
	TArray <uint8> adR2 = TArray <uint8>();
	if (M.mapAdj.Contains(IDR)) {
		adR2 = M.mapAdj[IDR];
		if (!adR2.Contains(IDR2))
			adR2.Add(IDR2);
	}
	else {
		adR2.Add(IDR2);
	}
	M.mapAdj.Emplace(IDR, adR2);
	//UE_LOG(LogTemp, Log, TEXT("Mi id muchachote %d,la otra id es %d"), IDR, IDR2);

	// pongo la puerta 
	M.mapElements[W1] = StaticMapElementInRoomTemplate::DOOR;

	// actualizamos la lista de puertas 
	LogicPositionAmplified d = LogicPositionAmplified(W1); // ATENSIO! Esta posicion no está trimeada ... Se transformará en el Fill_NERMAPDATA
	d.AddInfo(IDR);
	d.AddInfo(IDR2);

	UE_LOG(LogTemp, Error, TEXT("PD_MG_MapGenerationUtils::Put_Door_Tryng_doubleDoor_at - adding door (%d,%d) - %d - %d"), 
		d.logicPosition.GetX(), d.logicPosition.GetY(), d.info[0], d.info[1]);


	LD.Add(d);

	// actualizamos la lista de interactuables
	LogicPositionAmplified lpa = LogicPositionAmplified(W1);
	lpa.AddInfo(LD.Num()); // idInteractuable
	lpa.AddInfo((int)StaticMapElement::DOOR); // type interactuable


	UE_LOG(LogTemp, Error, TEXT("PD_MG_MapGenerationUtils::Put_Door_Tryng_doubleDoor_at - adding interactuable (%d,%d) - %d - %d"), 
		lpa.logicPosition.GetX(), lpa.logicPosition.GetY(), lpa.info[0], lpa.info[1]);

	LInteractuables.Add(lpa);


	// si puedo poner puerta doble ... 

	/*

	TArray<PD_MG_LogicPosition> adjac = W1.GenerateAdjacents(), wall_adjac = TArray<PD_MG_LogicPosition>();
	for (int i = 0; i < adjac.Num(); i++) {
		if (M.mapElements[adjac[i]] == StaticMapElementInRoomTemplate::WALL_OR_DOOR) {
			wall_adjac.Add(adjac[i]);
		}
	}


	if (wall_adjac.Num() > 0) {
		M.mapElements[wall_adjac[FMath::RandRange(0, wall_adjac.Num() - 1)]] = StaticMapElementInRoomTemplate::DOOR;
		return true;
	}
	*/

	return true;
}
// Te devuelve el StaticMapElement que puede tener en funcion del tipo de mapskin y del caracter que haya en el StaticMapElementInRoomTemplate
StaticMapElement PD_MG_MapGenerationUtils::GetStaticMapElementFrom(MapSkinType mst, StaticMapElementInRoomTemplate melem)
{
	// StaticMapElements que no dependen del MapSkinType a este nivel 
	if (melem == StaticMapElementInRoomTemplate::EMPTY)
		return StaticMapElement::EMPTY;

	if (melem == StaticMapElementInRoomTemplate::NORMAL_TILE)
		return StaticMapElement::NORMAL_TILE;

	if (melem == StaticMapElementInRoomTemplate::SPECIAL_TILE)
		return StaticMapElement::SPECIAL_TILE;

	if (melem == StaticMapElementInRoomTemplate::DOOR)
		return StaticMapElement::DOOR;









	switch (mst) {

		case MapSkinType::DUNGEON_NORMAL: {

			switch (melem) {
			case StaticMapElementInRoomTemplate::COLUMN:
			{
				TArray<StaticMapElement> posibles = TArray<StaticMapElement>();
				posibles.Add(StaticMapElement::TREE_OR_COLUMN_00);
				posibles.Add(StaticMapElement::TREE_OR_COLUMN_01);
				posibles.Add(StaticMapElement::TREE_OR_COLUMN_02);

				return posibles[FMath::RandRange(0, posibles.Num()-1)];
			}
			case StaticMapElementInRoomTemplate::PROP_CHEST:
				return StaticMapElement::PROP_CHEST;

			}
		}

		case MapSkinType::GARDEN: {

			switch (melem) {
				case StaticMapElementInRoomTemplate::COLUMN:
					return StaticMapElement::TREE_OR_COLUMN_00;
			}
		}
	}

	return StaticMapElement::NORMAL_TILE;
}


#pragma region COMPOSITIONS


uint16 PD_MG_MapGenerationUtils::MapSkinByRoomOf(int IdRoom, MapSkinType mapskintype) {
	return ((uint16)(IdRoom) << 8) | ((uint16)mapskintype & 0x00FF);
}

uint32 PD_MG_MapGenerationUtils::RoomCompositionOf(PD_MG_LogicPosition logpos, StaticMapElement sme, int IdRoom) {
	return ((uint32)(logpos.GetIn16bits()) << 16) | (((uint32)(sme) & 0xFF) << 8) | ((uint32)((IdRoom) & 0xFF));
}

uint32 PD_MG_MapGenerationUtils::WallCompositionOf(PD_MG_LogicPosition logpos, int IdRoomA, int IdRoomB)
{
	return ((uint32)(logpos.GetIn16bits()) << 16) | (((uint32) (IdRoomA) & 0xFF) << 8) | ( ((uint32)(IdRoomB)) & 0xFF);
}

uint32 PD_MG_MapGenerationUtils::DoorCompositionOf(PD_MG_LogicPosition logpos, int IdRoomA, int IdRoomB)
{
	return WallCompositionOf(logpos, IdRoomA, IdRoomB);
}

uint32 PD_MG_MapGenerationUtils::InteractuableIDOf(PD_MG_LogicPosition logpos, int IdInteractuable, int TypeInteractuable)
{
	return WallCompositionOf(logpos, IdInteractuable, TypeInteractuable);
}

uint16 PD_MG_MapGenerationUtils::InteractuableComposition(int IdInteractuable, int IdReactuable)
{
	return ((uint16)(IdInteractuable) << 8) | ((uint16)IdReactuable & 0x00FF);
}

uint32 PD_MG_MapGenerationUtils::EnemyCompositionOf(PD_MG_LogicPosition logpos, int IdEnemy, int TypeEnemy)
{
	return ((uint32)(logpos.GetIn16bits()) << 16) | ((uint32)((IdEnemy)& 0xFF) << 8) | ((uint32)((TypeEnemy)& 0xFF));
}


#pragma endregion


void PD_MG_MapGenerationUtils::Fill_NETMAPDATA_from(MapProceduralInfo &M, TArray<LogicPositionAmplified> walls, TArray<LogicPositionAmplified> doors, TArray<LogicPositionAmplified> interactuablesId) {

	//UE_LOG(LogTemp, Log, TEXT("PD_MG_MapGenerationUtils::Fill_NETMAPDATA_from spawn room "));
	
	// guardamos el idroom de la spawn room

	//UE_LOG(LogTemp, Log, TEXT("PD_MG_MapGenerationUtils::Fill_NETMAPDATA_from spawn room: M.Spawn %d "), M.SPAWN_ID);
	M.NETMAPDATA->IDRoomSpawn = (uint8)M.SPAWN_ID;

	//UE_LOG(LogTemp, Log, TEXT("PD_MG_MapGenerationUtils::Fill_NETMAPDATA_from spawn room: Netmapdata %d "), M.NETMAPDATA->IDRoomSpawn);
	M.NETMAPDATA->MAP_SIZE_IN_LOGIC_POSITIONS = (M.BOUNDING_BOX_DOWN_RIGHT - M.BOUNDING_BOX_TOP_LEFT).GetIn16bits();


	//UE_LOG(LogTemp, Log, TEXT("PD_MG_MapGenerationUtils::Fill_NETMAPDATA_from skin room "));
	// guardamos el skinbyroom
	for (int i = 0; i < M.mapRooms.Num(); i++) {
		M.NETMAPDATA->skinByRoom.Add(MapSkinByRoomOf(M.mapRooms[i].ID, M.mapRooms[i].ChoosedTag));
	}

	// guardamos la lista de habitaciones adyacentes

	//UE_LOG(LogTemp, Log, TEXT("PD_MG_MapGenerationUtils::Fill_NETMAPDATA_from mapadj"));
	TArray<uint8> mapAdj_keys;
	M.mapAdj.GenerateKeyArray(mapAdj_keys);
	for (int i = 0; i < mapAdj_keys.Num(); i++) {
		for (int j = 0; j < M.mapAdj[mapAdj_keys[i]].Num(); j++) {
			uint16 data = (uint16)((mapAdj_keys[i] & 0x00FF) << 8) | (uint8)(M.mapAdj[mapAdj_keys[i]][j] & 0x00FF);
			M.NETMAPDATA->roomsAdj.Add(data);

		}
	}

	//UE_LOG(LogTemp, Log, TEXT("PD_MG_MapGenerationUtils::Fill_NETMAPDATA_from room composition"));
	// guardamos el roomComposition
	for (int i = 0; i < M.mapRooms.Num(); i++) {
		TArray<PD_MG_LogicPosition> MAP_DATA_KEYS;
		M.mapRooms[i].MAP_DATA.GenerateKeyArray(MAP_DATA_KEYS);


		//UE_LOG(LogTemp, Log, TEXT("PD_MG_MapGenerationUtils::Fill_NETMAPDATA_from room composition - MAP_DATA_KEYS %d"), MAP_DATA_KEYS.Num());

		for (int j = 0; j < MAP_DATA_KEYS.Num(); j++) {


			//UE_LOG(LogTemp, Log, TEXT("PD_MG_MapGenerationUtils::Fill_NETMAPDATA_from room composition - MAP_DATA_KEYS value %d"), (int)M.mapRooms[i].MAP_DATA[MAP_DATA_KEYS[j]]);
			if (M.mapRooms[i].MAP_DATA[MAP_DATA_KEYS[j]] != StaticMapElementInRoomTemplate::WALL_ONLY &&
				M.mapRooms[i].MAP_DATA[MAP_DATA_KEYS[j]] != StaticMapElementInRoomTemplate::WALL_OR_DOOR &&
				M.mapRooms[i].MAP_DATA[MAP_DATA_KEYS[j]] != StaticMapElementInRoomTemplate::DOOR)
			{


				//UE_LOG(LogTemp, Log, TEXT("PD_MG_MapGenerationUtils::Fill_NETMAPDATA_from room composition - post if (%d, %d) - %d - %d"),
				//	(MAP_DATA_KEYS[j] + M.mapRooms[i].BOUNDING_BOX_TOP_LEFT).GetX(), (MAP_DATA_KEYS[j] + M.mapRooms[i].BOUNDING_BOX_TOP_LEFT).GetY(), 
				//	(int) GetStaticMapElementFrom(M.mapRooms[i].ChoosedTag, M.mapRooms[i].MAP_DATA[MAP_DATA_KEYS[j]]),
				//	M.mapRooms[i].ID);
				uint32 data = RoomCompositionOf(
					MAP_DATA_KEYS[j] + M.mapRooms[i].BOUNDING_BOX_TOP_LEFT - M.BOUNDING_BOX_TOP_LEFT,
					GetStaticMapElementFrom(M.mapRooms[i].ChoosedTag, M.mapRooms[i].MAP_DATA[MAP_DATA_KEYS[j]]),
					M.mapRooms[i].ID);
				//UE_LOG(LogTemp, Log, TEXT("PD_MG_MapGenerationUtils::Fill_NETMAPDATA_from room composition - data value %d"), data);

				M.NETMAPDATA->roomComposition.Add(data);
			}
		}
	}

	//UE_LOG(LogTemp, Log, TEXT("PD_MG_MapGenerationUtils::Fill_NETMAPDATA_from wall composition"));
	// guardamos el wallComposition
	for (int i = 0; i < walls.Num(); i++) {
		if (walls[i].info.Num() == 1) {
			M.NETMAPDATA->wallComposition.Add(
				WallCompositionOf(walls[i].logicPosition - M.BOUNDING_BOX_TOP_LEFT, walls[i].info[0], 0xFF)
			);
		}else if (walls[i].info.Num() == 2) {

			M.NETMAPDATA->wallComposition.Add(
				WallCompositionOf(walls[i].logicPosition - M.BOUNDING_BOX_TOP_LEFT, walls[i].info[0], walls[i].info[1])
			);
		}
		else if (walls[i].info.Num() == 3) {

			M.NETMAPDATA->wallComposition.Add(
				WallCompositionOf(walls[i].logicPosition - M.BOUNDING_BOX_TOP_LEFT, walls[i].info[0], walls[i].info[1])
			);

			M.NETMAPDATA->wallComposition.Add(
				WallCompositionOf(walls[i].logicPosition - M.BOUNDING_BOX_TOP_LEFT, walls[i].info[2], 0xFF)
			);
		}
		else if (walls[i].info.Num() == 4) {

			M.NETMAPDATA->wallComposition.Add(
				WallCompositionOf(walls[i].logicPosition - M.BOUNDING_BOX_TOP_LEFT, walls[i].info[0], walls[i].info[1])
			);

			M.NETMAPDATA->wallComposition.Add(
				WallCompositionOf(walls[i].logicPosition - M.BOUNDING_BOX_TOP_LEFT, walls[i].info[2], walls[i].info[3])
			);
		}
	}

	//UE_LOG(LogTemp, Log, TEXT("PD_MG_MapGenerationUtils::Fill_NETMAPDATA_from door composition"));
	// guardamos el doorComposition
	for (int i = 0; i < doors.Num(); i++) {
		M.NETMAPDATA->doorComposition.Add(DoorCompositionOf(doors[i].logicPosition - M.BOUNDING_BOX_TOP_LEFT, doors[i].info[0], doors[i].info[1]));
															// log pos											//IDRoomA			// IDRoom B
		//UE_LOG(LogTemp, Log, TEXT("PD_MG_MapGenerationUtils::Fill_NETMAPDATA_from doorComposition value add %d"), M.NETMAPDATA->doorComposition[M.NETMAPDATA->doorComposition.Num() - 1]);
	}

	for (int i = 0; i < M.NETMAPDATA->doorComposition.Num(); i++) {
		UE_LOG(LogTemp, Log, TEXT("PD_MG_MapGenerationUtils::Fill_NETMAPDATA_from doorComposition value add %d"), M.NETMAPDATA->doorComposition[M.NETMAPDATA->doorComposition.Num() - 1]);
	}

	// guardamos el interactuableId
	for (int i = 0; i < interactuablesId.Num(); i++) {
		M.NETMAPDATA->interactuableId.Add(	InteractuableIDOf(interactuablesId[i].logicPosition - M.BOUNDING_BOX_TOP_LEFT, interactuablesId[i].info[0], interactuablesId[i].info[1]));
											// log pos																		// idInteract				// typeInteract
		//UE_LOG(LogTemp, Log, TEXT("PD_MG_MapGenerationUtils::Fill_NETMAPDATA_from interactuablesId value add %d"), M.NETMAPDATA->interactuableId[M.NETMAPDATA->interactuableId.Num() - 1]);
		if (interactuablesId[i].info.Num() > 2) {
			for (int j = 2; j < interactuablesId[i].info.Num(); j++) {
				M.NETMAPDATA->interactuableComposition.Add(InteractuableComposition(interactuablesId[i].info[0], interactuablesId[i].info[j]));
																					// idInteractuable			// idReactuable
				UE_LOG(LogTemp, Log, TEXT("PD_MG_MapGenerationUtils::Fill_NETMAPDATA_from interactuableComposition value add %d"), M.NETMAPDATA->interactuableComposition[M.NETMAPDATA->interactuableComposition.Num() - 1]);
			}
		}
	}


	for (int i = 0; i < M.NETMAPDATA->interactuableId.Num(); i++) {
		UE_LOG(LogTemp, Log, TEXT("PD_MG_MapGenerationUtils::Fill_NETMAPDATA_from doorComposition value add %d"), M.NETMAPDATA->interactuableId[M.NETMAPDATA->interactuableId.Num() - 1]);
	}
	

	TArray<PD_MG_LogicPosition> lp;
	M.enemies.GenerateKeyArray(lp);
	for (int i = 0; i <lp.Num() ; i++)
	{
		//UE_LOG(LogTemp, Log, TEXT("PD_MG_MapGenerationUtils::Tipo de enemigo %d"), M.enemies[lp[i]]);
		M.NETMAPDATA->enemyComposition.Add(EnemyCompositionOf(lp[i] - M.BOUNDING_BOX_TOP_LEFT, i, M.enemies[lp[i]]));
	}
	//UE_LOG(LogTemp, Log, TEXT("PD_MG_MapGenerationUtils::Fill_NETMAPDATA_from fin"));
}

#pragma endregion

