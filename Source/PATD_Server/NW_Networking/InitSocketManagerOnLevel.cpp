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
		SGI->InitServerActoWhenLoadMap();
	}

}

// Called every frame
void AInitSocketManagerOnLevel::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );

}

