// Fill out your copyright notice in the Description page of Project Settings.

#include "PATD_Server.h"
#include "PD_NW_ServerActor.h"

//Includes of forward declaration
#include "NW_Networking/Socket/PD_NW_SocketManager.h"

// Sets default values
APD_NW_ServerActor::APD_NW_ServerActor()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void APD_NW_ServerActor::BeginPlay()
{
	Super::BeginPlay();

}

// Called every frame
void APD_NW_ServerActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void APD_NW_ServerActor::InitTimerActor()
{
	GetWorldTimerManager().SetTimer(TimerHandleActor, this, &APD_NW_ServerActor::CheckForReceivedData, 1.0f, true);

}


void APD_NW_ServerActor::CheckForReceivedData()
{
	SocketManager->TimerRefreshFunction();
}


void APD_NW_ServerActor::SetSocketManager(PD_NW_SocketManager* InSocketManager)
{
	SocketManager = InSocketManager;
}