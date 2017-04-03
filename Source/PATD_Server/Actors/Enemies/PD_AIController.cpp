// Fill out your copyright notice in the Description page of Project Settings.

#include "PATD_Server.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/Blackboard/BlackboardKeyAllTypes.h"
#include "PATD_Server/Actors/PD_E_Character.h"
#include "PATD_Server/GM_Game/LogicCharacter/PD_GM_LogicCharacter.h"
#include "PD_ServerGameInstance.h"
#include "PD_AIController.h"
#include <math.h>

APD_AIController::APD_AIController(){
	BlackboardComp = CreateDefaultSubobject<UBlackboardComponent>("BlackboardComp");
	BehaviorTreeComp = CreateDefaultSubobject<UBehaviorTreeComponent>("BehaviorTreeComp");
}

void APD_AIController::Possess(APawn* charac)
{
Super::Possess(charac);
APD_E_Character* ch = Cast<APD_E_Character>(charac);

	if (ch && ch->behaviorTree)
	{
		BlackboardComp->InitializeBlackboard(*ch->behaviorTree->BlackboardAsset);
		BlackboardComp->SetValueAsInt("AP",0);
		BehaviorTreeComp->StartTree(*ch->behaviorTree);
	}
}

void APD_AIController::StartTurn(PD_GM_MapManager* refMap, PD_GM_LogicCharacter* logicCharacter) 
{
	mapMng = refMap;
	currentcharac = logicCharacter;

	actions->listMove.Empty();
	actions->listAttack.Empty();
	BlackboardComp->SetValueAsInt("AP", 5);
}

/*
void APD_AIController::endTurn() {

	UPD_ServerGameInstance* SGI = Cast<UPD_ServerGameInstance>(GetGameInstance());
//instance->game(->iamanager)
}*/