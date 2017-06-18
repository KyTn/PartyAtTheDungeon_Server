// Fill out your copyright notice in the Description page of Project Settings.

#include "PATD_Server.h"
#include "PD_IA_TaskSwindler_CreateOrder.h"




#include "PATD_Server/Actors/Enemies/PD_AIController.h"
#include "GM_Game/LogicCharacter/PD_GM_LogicCharacter.h"
#include "Actors/PD_E_Character.h"
#include "GM_Game/Pathfinder/PD_GM_Pathfinder.h"
#include "Actors/Interactuables/PD_E_Door.h"
EBTNodeResult::Type UPD_IA_TaskSwindler_CreateOrder::ExecuteTask(UBehaviorTreeComponent & OwnerComp, uint8 * NodeMemory)
{






	CreateMoveOrders(OwnerComp);
	CreateInteractuableOrders(OwnerComp);



	return EBTNodeResult::Succeeded;
}

void UPD_IA_TaskSwindler_CreateOrder::CreateInteractuableOrders(UBehaviorTreeComponent& OwnerComp) {
	APD_AIController* AIController = (APD_AIController*)OwnerComp.GetAIOwner();
	if (AIController->activateThisTurn) {
		FStructTurnOrders* turnStruct = AIController->GetTurnOrders();
		
		turnStruct->interactuablesToInteract.Add(AIController->goalInteractuable->ID_Interactuable);
	}
}