// Fill out your copyright notice in the Description page of Project Settings.

#include "PATD_Server.h"
#include "PD_IA_TaskFleeCreateOrder.h"


#include "PATD_Server/Actors/Enemies/PD_AIController.h"
#include "GM_Game/LogicCharacter/PD_GM_LogicCharacter.h"
#include "Actors/PD_E_Character.h"
#include "GM_Game/Pathfinder/PD_GM_Pathfinder.h"

EBTNodeResult::Type UPD_IA_TaskFleeCreateOrder::ExecuteTask(UBehaviorTreeComponent & OwnerComp, uint8 * NodeMemory)
{






	CreateMoveOrders(OwnerComp);




	return EBTNodeResult::Succeeded;
}


void UPD_IA_TaskFleeCreateOrder::CreateMoveOrders(UBehaviorTreeComponent& OwnerComp) {

	UE_LOG(LogTemp, Log, TEXT("UPD_IA_TaskFleeCreateOrder:: CreateMoveOrders"));

	APD_AIController* AIController = (APD_AIController*)OwnerComp.GetAIOwner();
	PD_GM_LogicCharacter* logicCharacter = ((APD_E_Character*)AIController->GetPawn())->logic_character;
	FStructTurnOrders* turnStruct = AIController->GetTurnOrders();

	//Movimiento
	TArray<PD_MG_LogicPosition> listPathPosition;
	listPathPosition = AIController->GetPathFinder()->getPathFromTo(logicCharacter->GetCurrentLogicalPosition(), AIController->turnTargetPosition);

	for (PD_MG_LogicPosition pathPosition : listPathPosition) {
		UE_LOG(LogTemp, Log, TEXT("UPD_IA_TaskFleeCreateOrder:: Añadiendo movimiento:%d , %d "), pathPosition.GetX(), pathPosition.GetY());
		FStructLogicPosition logicPositionStruct = FStructLogicPosition();

		logicPositionStruct.positionX = pathPosition.GetX();
		logicPositionStruct.positionY = pathPosition.GetY();

		turnStruct->positionsToMove.Add(logicPositionStruct);
	}
}

