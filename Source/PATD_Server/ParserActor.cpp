// Fill out your copyright notice in the Description page of Project Settings.

#include "PATD_Server.h"
#include "ParserActor.h"
#include "PD_MG_StaticMap.h"


// Sets default values
AParserActor::AParserActor()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AParserActor::BeginPlay()
{
	Super::BeginPlay();
	StaticMapRef = new PD_MG_StaticMap();

	//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, "Starting Dungeon Parser v0.1");
	
	FString CompleteFilePath = FPaths::GameDir() + "Content/DungeonTestingMaps/test1.dungeon";

	FString FileData = "";
	if (FFileHelper::LoadFileToString(FileData, *CompleteFilePath)) {

		// Muesta por pantalla el contenido del fichero pasado al parser
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FileData);



		// Obtenemos el numero de filas y de columnas 
		FString A, B;

		uint32 fil, col;

		FileData.Split("\n", &A, &B, ESearchCase::CaseSensitive, ESearchDir::FromStart);
		fil = FCString::Atoi(*A);

		B.Split("\n", &A, &B, ESearchCase::CaseSensitive, ESearchDir::FromStart);
		col = FCString::Atoi(*A);
		////////////////


		// Obtenemos 
		TArray<TArray<TCHAR>> map;

		map.Empty(20);
		
		while (B.Split("\n", &A, &B, ESearchCase::CaseSensitive, ESearchDir::FromStart))
		{
			map.Add(A.GetCharArray());
		}
		map.Add(B.GetCharArray());


		
		FString s;

		for (int i = 0; i < map.Num(); i++)
		{
			s = "";
			
			for (int j = 0; j < map[i].Num(); j++)
			{
				s += map[i][j];

				StaticMapRef->AddNewLogicPosition(i, j);

				FString salida = " LP : (";
				salida.AppendInt(i);
				salida.Append(", ");
				salida.AppendInt(j);
				salida.Append(")");

				GEngine->AddOnScreenDebugMessage(-1, 5000000.f, FColor::Red, salida);
			}

			//GEngine->AddOnScreenDebugMessage(-1, 5000000.f, FColor::Red, s);
			
		}
	}

}

// Called every frame
void AParserActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}