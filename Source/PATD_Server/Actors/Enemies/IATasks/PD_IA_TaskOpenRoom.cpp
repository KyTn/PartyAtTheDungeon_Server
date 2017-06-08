// Fill out your copyright notice in the Description page of Project Settings.

#include "PATD_Server.h"
#include "PD_IA_TaskOpenRoom.h"

#include "GM_Game/LogicCharacter/PD_GM_LogicCharacter.h"
#include "PATD_Server/Actors/Enemies/PD_AIController.h"
#include "GM_Game/PD_GM_MapManager.h"
#include "MapInfo/PD_MM_MapInfo.h"

EBTNodeResult::Type UPD_IA_TaskOpenRoom::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) {

	APD_AIController* AIController = (APD_AIController*)OwnerComp.GetAIOwner();

	PD_GM_LogicCharacter* logicCharacter = ((APD_E_Character*)AIController->GetPawn())->logic_character;

	PD_MM_Room* room = AIController->GetMapMng()->MapInfo->roomByLogPos[logicCharacter->GetCurrentLogicalPosition()];

	if (room->IsOpen) {//la habitacion está abierta
		if (AIController->canAction) {
			UE_LOG(LogTemp, Log, TEXT("UPD_IA_TaskOpenRoom:: room is Open"));

			return EBTNodeResult::Succeeded;
		}
		else {//es el primer turno despues de que este abierta
			UE_LOG(LogTemp, Log, TEXT("UPD_IA_TaskOpenRoom:: room is Open, pero no puede hacer accion"));

			AIController->canAction = true;
		}
	}
	else {
		UE_LOG(LogTemp, Log, TEXT("UPD_IA_TaskOpenRoom:: room is Close"));

	}
	AIController->turnTargetCharacter = nullptr;
	AIController->turnNumAttacks = 0;
	AIController->EndAITurnCalculation();
	return EBTNodeResult::Failed;
}
