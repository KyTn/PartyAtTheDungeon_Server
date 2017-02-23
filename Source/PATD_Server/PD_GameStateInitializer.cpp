// Fill out your copyright notice in the Description page of Project Settings.

#include "PATD_Server.h"
#include "PD_GameStateInitializer.h"

//Includes of forward declaration
#include "PD_ServerGameInstance.h"

void APD_GameStateInitializer:: BeginPlay() {
	Super::BeginPlay();


	UPD_ServerGameInstance* SGI = Cast<UPD_ServerGameInstance>(GetGameInstance());
	SGI->OnMapFinishLoad();
}


