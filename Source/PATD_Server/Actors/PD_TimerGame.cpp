// Fill out your copyright notice in the Description page of Project Settings.

#include "PATD_Server.h"
#include "PD_TimerGame.h"

#include "GM_Game/PD_GM_GameManager.h"
#include "PD_ServerGameInstance.h"

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

void APD_TimerGame::InitTimerPodium(float time, UPD_ServerGameInstance* inSGI) {
	SGI = inSGI;
	timerRunning = true;
	GetWorldTimerManager().SetTimer(timer, this, &APD_TimerGame::OnTimerEndPodiums, time, false);
}

void APD_TimerGame::OnTimerEndPodiums() {
	timerRunning = false;
	UE_LOG(LogTemp, Log, TEXT("APD_TimerGame::OnTimerEndPodiums - Tiempo se acabo en el podium "));
	SGI->OnTimerPodiumEnds();
}


//To control the flow of the game and never stay in one phase--> allows to force to continue the flow, ou yeah!
void APD_TimerGame::InitTimerAnimationsToEnd(float time)
{
	UE_LOG(LogTemp, Log, TEXT("APD_TimerGame::InitTimerAnimationsToEnd -"));

	timerAnimationRunning = true;
	GetWorld()->GetTimerManager().ClearTimer(timerForAnimations);
	GetWorldTimerManager().SetTimer(timerForAnimations, this, &APD_TimerGame::OnTimerEndForAnimationsToEnd, time, false);
}

void APD_TimerGame::OnTimerEndForAnimationsToEnd()
{

	UE_LOG(LogTemp, Log, TEXT("APD_TimerGame::OnTimerEndForAnimationsToEnd - the timer of animations end... call to gamemanager"));

	timerAnimationRunning = false;
	//time to all animations end for anyone(players and enemies)
	gameManager->OnTimerAnimationEnd();
}

void APD_TimerGame::CancelTimeAanimation()
{
	UE_LOG(LogTemp, Log, TEXT("APD_TimerGame::CancelTimeAanimation - stop the timer"));

	timerAnimationRunning = false;
	GetWorld()->GetTimerManager().ClearTimer(timerForAnimations);
}
