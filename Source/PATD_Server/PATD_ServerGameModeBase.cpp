// Fill out your copyright notice in the Description page of Project Settings.

#include "PATD_Server.h"
#include "PATD_ServerGameModeBase.h"

#include "PD_GameStateInitializer.h"

APATD_ServerGameModeBase::APATD_ServerGameModeBase() {
	//Super();
	GameStateClass = APD_GameStateInitializer::StaticClass();
}
