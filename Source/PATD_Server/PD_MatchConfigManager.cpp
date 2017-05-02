// Fill out your copyright notice in the Description page of Project Settings.

#include "PATD_Server.h"
#include "PD_MatchConfigManager.h"

//Fordward declarations
#include "PD_MatchConfig.h"

PD_MatchConfigManager::PD_MatchConfigManager(UPD_ServerGameInstance * sgi)
{
	ActualMatchConfig = new PD_MatchConfig();
	SGI = sgi;
}

PD_MatchConfigManager::~PD_MatchConfigManager()
{
}
