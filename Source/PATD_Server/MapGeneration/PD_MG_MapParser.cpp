// Fill out your copyright notice in the Description page of Project Settings.

#include "PATD_Server.h"
#include "PATD_Server/MapGeneration/PD_MG_MapParser.h"
#include "PATD_Server/MapGeneration/Static/PD_MG_StaticMap.h"
#include "PATD_Server/MapGeneration/Dynamic/PD_MG_DynamicMap.h"
#include "PATD_Server/Actors/Enemies/PD_E_EnemiesEnum.h"
#include "PATD_Server/GM_Game/LogicCharacter/PD_GM_LogicCharacter.h"

PD_MG_MapParser::PD_MG_MapParser()
{
}

PD_MG_MapParser::~PD_MG_MapParser()
{
}


#pragma region Parse From File

PD_MG_StaticMap* PD_MG_MapParser::StartParsingFromFile(FString* filepath)
{
	return StartParsingFromFile(filepath, new PD_MG_StaticMap(), new PD_MG_DynamicMap());
}

PD_MG_StaticMap* PD_MG_MapParser::StartParsingFromFile(FString* filepath, PD_MG_StaticMap* staticMapRef, PD_MG_DynamicMap* dynamicMapRef)
{
	staticMapRef->Clear();
	dynamicMapRef->Clear();
	// Leamos el fichero
	//Estamos metiendo el FPaths::GameDir() aqui y en el parserActor, asi que solo hay que ponerlo en uno de los sitios
	FString FilePath = FPaths::GameDir() + *filepath;
	FString FileData = "";

	if (FFileHelper::LoadFileToString(FileData, *FilePath))
	{
		//Agregado para el hito2 MCG
		UE_LOG(LogTemp, Warning, TEXT("PD_MG_MapParser::StartParsingFromFile::  Llamando a SetMapString . Path :%s  Mapa: %s" ), *FilePath, *FileData);
		staticMapRef->SetMapString(FileData);

		// Enviar a los clientes el mapa leido ... 


		// Obtenemos la version del parser que se debe usar. 

		TArray<FString> fileSplitted;
		FileData.ParseIntoArray(fileSplitted, TEXT("\n"), true);

		if (fileSplitted[0].Contains("v0.1") ) {
			//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, "Using parser version " + fileSplitted[0]);

			Parsing_v_0_1(fileSplitted, staticMapRef, dynamicMapRef);
		}
		else 
		{
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, "No parser version registered! Searching for " + fileSplitted[0]);
		}

	}
	else {
		UE_LOG(LogTemp, Error, TEXT("PD_MG_MapParser::StartParsingFromFile::  Error loading map! Failed to load file!. Path :%s "), *FilePath);

	}


	return staticMapRef;
}

#pragma endregion


#pragma region VERSION OF PARSERS

PD_MG_StaticMap* PD_MG_MapParser::Parsing_v_0_1(TArray<FString> fileReaded, PD_MG_StaticMap* staticMapRef, PD_MG_DynamicMap* dynamicMapRef)
{
	//if (!fileReaded[0].Equals("v0.1")) { return staticMapRef; }

	uint32 nextIndexRead = 0;
	uint32 mapIndex = 0;
	// Cual es el tipo de misi�n para este mapa?
	if (fileReaded[1].Contains("defeatBoss")) {
		staticMapRef->SetTypeMission(TypeOfMission::DefeatBoss);
	}
	else if (fileReaded[1].Contains("defeatAll")) {
		staticMapRef->SetTypeMission(TypeOfMission::DefeatAll);
	}
	else if (fileReaded[1].Contains("recoverTreasure")) {
		staticMapRef->SetTypeMission(TypeOfMission::RecoverTreasure);
	}

	// Tama�o de la matriz
	staticMapRef->SetHeight((uint32)FCString::Atoi(*(fileReaded[2])));
	dynamicMapRef->SetHeight((uint32)FCString::Atoi(*(fileReaded[2])));
	staticMapRef->SetWidth((uint32)FCString::Atoi(*(fileReaded[3])));
	dynamicMapRef->SetWidth((uint32)FCString::Atoi(*(fileReaded[3])));

	nextIndexRead = 4;
	mapIndex = nextIndexRead;
	// Cargamos los caractares planos sin referencias de comportamiento
	nextIndexRead = ReadRawMap(fileReaded, nextIndexRead, staticMapRef);
	
	//InstantiateStaticMap(staticMapRef);
	//Creamos el TMAP con los tipos de enemigos que hay
	nextIndexRead = ReadTypesEnemies(fileReaded, nextIndexRead, dynamicMapRef);

	//Cargamos la posici�n de los enemigos
	//ReadEnemiesMap(fileReaded, mapIndex, dynamicMapRef);

	return staticMapRef;
}

#pragma endregion 

//Rellena el diccionario interno del staticMapRef con la info en crudo del mapa, sin referencias de comportamiento. 
uint32 PD_MG_MapParser::ReadRawMap(TArray<FString> fileReaded, uint32 firstIndex, PD_MG_StaticMap* staticMapRef)
{

	for (uint32 i = firstIndex; i < staticMapRef->GetHeight() + firstIndex; i++) {
		TArray<TCHAR> ta = fileReaded[i].GetCharArray();
		for (uint32 j = 0; j < staticMapRef->GetWidth(); j++) {
			staticMapRef->AddNewLogicPosition(i- firstIndex, j, ta[j]);
		}
	}
	return staticMapRef->GetHeight() + firstIndex;
}

//Rellena la parte de los enemigos del DynamicMap pasado por parametros
uint32 PD_MG_MapParser::ReadTypesEnemies(TArray<FString> fileReaded, uint32 firstIndex, PD_MG_DynamicMap* dynamicMapRef) {
	TArray <TCHAR> enemyLine;
	FString f;
	uint32 Enemynum = (uint32)FCString::Atoi(*(fileReaded[firstIndex]));
	uint32 intType,x, y;
	int32 num,j;
	firstIndex++;
	EEnemiesType type;
	for (uint32 i = firstIndex; i < Enemynum + firstIndex;i++) {
		/*enemyValue = fileReaded[i].RightChop(2);
		enemyValue.RemoveAt(enemyValue.Len()-1);//Hay que hacer esto, dado que guardaba basura, un "\n", o un "\0" que se comportaba al hacer el cout como "\n"
		dynamicMapRef->AddEnemyDictionary(fileReaded[i].GetCharArray()[0], enemyValue);*/
		enemyLine = fileReaded[i].GetCharArray();
		enemyLine.RemoveAt(enemyLine.Num() - 1);
		enemyLine.RemoveAt(enemyLine.Num() - 1);
	
		j = 0; num = 1; intType=0; x = 0; y = 0;
		while (enemyLine[j]!=':')
		{
			intType = enemyLine[j]-'0' + (intType*num);
			j++;
			num = num * 10;
		}
		type = EEnemiesType(intType);
		num = 1; j++;
		while (enemyLine[j] != ',')
		{
			x = enemyLine[j] - '0' + (x*num);
			j++;
			num = num * 10;
		}
		num = 1; j++;
		while (j<enemyLine.Num())
		{
			y = enemyLine[j] - '0' + (y*num);
			j++;
			num = num * 10;
		}
		switch (type) {///En este switch metemos la IA l�gica de cada uno
			case EEnemiesType::Archer: {
				PD_GM_LogicCharacter* ch = new PD_GM_LogicCharacter();
				//dynamicMapRef->AddNewEnemy(x, y, ch, type);
				break;
		}
			case EEnemiesType::Zombie: {
				PD_GM_LogicCharacter* ch = new PD_GM_LogicCharacter();
				//dynamicMapRef->AddNewEnemy(x, y, ch, type);
				break;
			}
		}

//		dynamicMapRef->AddEnemyDictionary(fileReaded[i].GetCharArray()[0], fileReaded[i].RightChop(2).RemoveAt(fileReaded[i].Len()));

		///Comprobaci�n de que se escribe bien el TMAP de los enemigos
		/*enemyValue.Reset();
		enemyValue=dynamicMapRef->GetEnemy(fileReaded[i].GetCharArray()[0]);
		//enemyValue.InsertAt(0, bufferLine.RightChop(2));
		UE_LOG(LogTemp, Error, TEXT("Prueba :%s !!!!"), *enemyValue);
		*/
		///Forma m�s simple y con mas pasos de hacerlo
		/*
		bufferLine = fileReaded[i];
		enemyValue.Reset();
		enemyValue.InsertAt(0,bufferLine.RightChop(2));
		UE_LOG(LogTemp, Error, TEXT("Prueba :%s !!!!"), *enemyValue);
		*/
	}
	return firstIndex + Enemynum + 1;
}


