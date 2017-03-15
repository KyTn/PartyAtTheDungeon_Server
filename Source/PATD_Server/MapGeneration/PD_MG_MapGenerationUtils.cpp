// Fill out your copyright notice in the Description page of Project Settings.

#include "PATD_Server.h"
#include "PD_MG_MapGenerationUtils.h"
#include "PATD_Server/MapGeneration/Static/PD_MG_StaticMap.h"

PD_MG_MapGenerationUtils::PD_MG_MapGenerationUtils()
{
}

PD_MG_MapGenerationUtils::~PD_MG_MapGenerationUtils()
{
}


#pragma region READ ROOMS FROM FILE AND FROM CHORIZO


//TESTING : Content/DungeonTestingMaps/test1.rooms



/// Lee desde fichero el contenido de un fichero y lo parsea como un contenedor de habitaciones
bool PD_MG_MapGenerationUtils::ReadAndPrepareRoomTemplateInfosFromFile(FString filepath, TArray<RoomTemplateInfo> &roomTemplates) {

	// Leamos el fichero
	FString FilePath = FPaths::GameDir() + *filepath;
	FString FileData = "";

	if (FFileHelper::LoadFileToString(FileData, *FilePath))
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
	TArray<FString> TAGS;
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
void PD_MG_MapGenerationUtils::ParseTags(TArray<FString> &tags, FString braquets) {
	tags.Empty();

	TArray<FString> s;
	braquets.ParseIntoArray(s, TEXT(","), true);

	if (s.Num() == 0) {
		tags.Add("None");
		return;
	}

	for (int i = 0; i < s.Num(); i++) {
		tags.Add(s[i]);
	}

}

#pragma endregion

#pragma region PRELOADED ROOMS

/// Si la lectura del fichero falla, llamamos a esta funcion que nos da algunas habitaciones para que no pete el tema 
bool PD_MG_MapGenerationUtils::GetPreloadedData(TArray<RoomTemplateInfo> &roomTemplates) {

	TArray<FString> tags = TArray<FString>();
	tags.Add("NORMAL");
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



#pragma region PROCEDURAL GENERATION


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


bool PD_MG_MapGenerationUtils::GenerateRandomStaticMap(TArray<RoomTemplateInfo> & LR, int _Total_Height, int _Total_Width) {

	MapProceduralInfo M = MapProceduralInfo(_Total_Height, _Total_Width); //M - mapa
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

	LW.Empty();
	LWC.Empty();
	LWT.Empty();
	LRT.Empty();
	LCT.Empty();


	do {
		// 2 Elegir una casilla C no elegida de M ...

		UE_LOG(LogTemp, Warning, TEXT("PD_MG_MapGenerationUtils::GenerateRandomStaticMap - Randon searching on (%d,%d) -> (%d,%d) "),
			(M.Total_Height / 3), (2 * M.Total_Height / 3),
			(M.Total_Width / 3), (2 * M.Total_Width / 3));

		int tile_x = FMath::RandRange( (int)(M.Total_Height / 3), (int)(2 * M.Total_Height / 3));
		int tile_y = FMath::RandRange((int)(M.Total_Width / 3), (int)(2 * M.Total_Width / 3));
		C = PD_MG_LogicPosition(tile_x, tile_y);

		UE_LOG(LogTemp, Warning, TEXT("PD_MG_MapGenerationUtils::GenerateRandomStaticMap - Found: (%d,%d) !"),
			C.GetX(), C.GetY());

		LCT.Add(C);

		// 3 Elegir una habitación R de la lista LR no elegida ...
		/*
		if (LRT.Num() == LR.Num()) {
			return false;
		}
		do {
			R = LR[rand() % LR.Num()];
			
		} while (!LRT.Contains(R));
		*/
		R = LR[FMath::RandRange(0, LR.Num()-1)];
		LRT.Add(R);


		TArray<PD_MG_LogicPosition> posiblesPivotesR;
		R.MAP_DATA.GenerateKeyArray(posiblesPivotesR);
		R_pivot = posiblesPivotesR[FMath::RandRange(0, posiblesPivotesR.Num()-1)];

		// comprobar que se puede colocar, si no volvemos a generar todo lo anterior
	} while (!Hard_Check_CanBeColocated(M, R, C, R_pivot));

	UE_LOG(LogTemp, Log, TEXT("PD_MG_MapGenerationUtils::GenerateRandomStaticMap Name of first room: %s - Adding to map ..."), *(R.NAME));

	AddRoomToMapAtLocation(M, R, C, R_pivot);




	return false;
}



bool PD_MG_MapGenerationUtils::Hard_Check_CanBeColocated(MapProceduralInfo &M, RoomTemplateInfo &R, PD_MG_LogicPosition C, PD_MG_LogicPosition R_pivot) {

	if (!MapCanContainsRoom(M, R, C)) return false;
		
	for (int i = 0; i < R.LOCAL_LOGIC_POSITIONS_ON_ROOM.Num(); i++) {
		if (M.mapElements.Contains(C - R.LOCAL_LOGIC_POSITIONS_ON_ROOM[i])) {
			return false;
		}
	}
	
	return true;
	
}

bool PD_MG_MapGenerationUtils::MapCanContainsRoom(MapProceduralInfo &M, RoomTemplateInfo &R, PD_MG_LogicPosition C) {

	PD_MG_LogicPosition cornerTopLeft = C - R.BOUNDING_BOX_TOP_LEFT;
	PD_MG_LogicPosition cornerDownRight = C - R.BOUNDING_BOX_DOWN_RIGHT;

	if (cornerTopLeft.GetX() < 0 || cornerTopLeft.GetY() < 0 ||
		cornerDownRight.GetX() >= (int)M.Total_Height || cornerDownRight.GetY() >= (int)M.Total_Width) {
		return false;
	}

	return true;
}

PD_MG_LogicPosition PD_MG_MapGenerationUtils::Translate_LocalPosInRoom_To_MapPosition(PD_MG_LogicPosition localPos, PD_MG_LogicPosition C, PD_MG_LogicPosition R_pivot) {
	return localPos - R_pivot + C;
}


void PD_MG_MapGenerationUtils::AddRoomToMapAtLocation(MapProceduralInfo &M, RoomTemplateInfo &R, PD_MG_LogicPosition C, PD_MG_LogicPosition R_pivot) {
	M.AddRoomToMapAtLocation(R, C, R_pivot);
}

#pragma endregion