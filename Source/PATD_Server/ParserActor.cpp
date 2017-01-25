// Fill out your copyright notice in the Description page of Project Settings.

#include "PATD_Server.h"
#include "ParserActor.h"


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

	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, "Starting Dungeon Parser v0.1");

	FString GameDir = FPaths::GameDir();
	FString CompleteFilePath = GameDir + "Content/DungeonTestingMaps/test1.dungeon";

	FString FileData = "";
	if (FFileHelper::LoadFileToString(FileData, *CompleteFilePath)) {

		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FileData);


		FString A, B;


		FileData.Split("\n", &A, &B, ESearchCase::CaseSensitive, ESearchDir::FromStart);

		int32 fil = FCString::Atoi(*A);
		B.Split("\n", &A, &B, ESearchCase::CaseSensitive, ESearchDir::FromStart);

		int32 col = FCString::Atoi(*A);


		TArray<TArray<TCHAR>> map;

		map.Empty(20);
		/*
		map[0][0] = 'a';


		for (int i = 0; i < fil; i++)
		{
		for (int j = 0; j < col; j++)
		{

		}
		}
		*/

		int i = 0;


		while (B.Split("\n", &A, &B, ESearchCase::CaseSensitive, ESearchDir::FromStart))
		{
			map.Add(A.GetCharArray());
		}
		map.Add(B.GetCharArray());


		for (i = map.Num() - 1; i >= 0; i--)
		{
			FString s;

			for (int j = 0; j < map[i].Num(); j++)
			{
				s += map[i][j];
			}

			GEngine->AddOnScreenDebugMessage(-1, 5000000.f, FColor::Red, s);
		}
		/**/
		//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, A + "----");
		//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, B);

	}

}

// Called every frame
void AParserActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}