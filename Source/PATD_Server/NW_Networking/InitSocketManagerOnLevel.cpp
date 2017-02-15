// Fill out your copyright notice in the Description page of Project Settings.

#include "PATD_Server.h"
#include "InitSocketManagerOnLevel.h"

//Includes of forward declaration
#include "NW_Networking/Socket/PD_NW_SocketManager.h"
#include "PD_ServerGameInstance.h"

// Sets default values
AInitSocketManagerOnLevel::AInitSocketManagerOnLevel()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AInitSocketManagerOnLevel::BeginPlay()
{
	Super::BeginPlay();
	
	UPD_ServerGameInstance* SGI = Cast<UPD_ServerGameInstance>(GetGameInstance());
	if (SGI)
	{
		UE_LOG(LogTemp, Warning, TEXT("Actor de spawneo  %s"), *GetWorld()->GetMapName());

		SGI->InitServerActoWhenLoadMap();

		FString currentMapName = SGI->levelsNameDictionary.GetMapName(4, SGI->GetWorld()->IsPlayInEditor());
		if (GetWorld()->GetMapName() == currentMapName) {
			UE_LOG(LogTemp, Warning, TEXT("If del LVL_4_Gamemap"));
			SGI->InitGameMap();
		}
		/*Esto lo que hace es como el open level carga en un hilo diferente o algo asi, y las 
		siguientes instrucciones que haya tras llamarlo, no se ejecutan con el mapa 
		ya cargado, tenemos este actor para inicializar cosas, que se instancia junto al mapa. (esta en el)
		El cuando se le llame a su begin play, va a llamar a InitGameMap que spawnea el actor de parser.
		*/
	}

}

// Called every frame
void AInitSocketManagerOnLevel::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );

}

