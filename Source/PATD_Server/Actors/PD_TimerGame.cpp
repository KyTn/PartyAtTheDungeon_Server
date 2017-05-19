// Fill out your copyright notice in the Description page of Project Settings.

#include "PATD_Server.h"
#include "PD_TimerGame.h"
#include "GM_Game/PD_GM_GameManager.h"

// Sets default values
APD_TimerGame::APD_TimerGame()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void APD_TimerGame::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void APD_TimerGame::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );

}

void APD_TimerGame::InitTimer(float time) {
	timerRunning = true;
	GetWorldTimerManager().SetTimer(timer, this, &APD_TimerGame::OnTimerEnds, time, false);
}

void APD_TimerGame::OnTimerEnds() {
	timerRunning = false;
	gameManager->OnTimerEnd();
}