// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "PD_TimerGame.generated.h"
class PD_GM_GameManager;
class UPD_ServerGameInstance;

UCLASS()
class PATD_SERVER_API APD_TimerGame : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	APD_TimerGame();

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	// Called every frame
	virtual void Tick( float DeltaSeconds ) override;

	UPD_ServerGameInstance* SGI;

	FTimerHandle timer;

	PD_GM_GameManager* gameManager;

	bool timerRunning=false;
	bool isTimerRunning() { return timerRunning; };

	void setGameManager(PD_GM_GameManager* inGameManager) { gameManager = inGameManager; };
	
	void InitTimer(float time);
	void OnTimerEnds();

	void InitTimerPodium(float time, UPD_ServerGameInstance* inSGI);
	void OnTimerEndPodiums();
};
