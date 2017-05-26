// Fill out your copyright notice in the Description page of Project Settings.

#include "PATD_Server.h"
#include "PD_IA_TaskDefenseCreateOrder.h"



#include "PATD_Server/Actors/Enemies/PD_AIController.h"
#include "GM_Game/LogicCharacter/PD_GM_LogicCharacter.h"
#include "Actors/PD_E_Character.h"
#include "GM_Game/Pathfinder/PD_GM_Pathfinder.h"

EBTNodeResult::Type UPD_IA_TaskDefenseCreateOrder::ExecuteTask(UBehaviorTreeComponent & OwnerComp, uint8 * NodeMemory)
{






	CreateMoveOrders(OwnerComp);
	CreateAttackOrders(OwnerComp);
	CreateDefenseOrders(OwnerComp);


	return EBTNodeResult::Succeeded;
}


void UPD_IA_TaskDefenseCreateOrder::CreateMoveOrders(UBehaviorTreeComponent& OwnerComp) {

	APD_AIController* AIController = (APD_AIController*)OwnerComp.GetAIOwner();
	PD_GM_LogicCharacter* logicCharacter = ((APD_E_Character*)AIController->GetPawn())->logic_character;
	FStructTurnOrders* turnStruct = AIController->GetTurnOrders();

	//Movimiento
	TArray<PD_MG_LogicPosition> listPathPosition;
	listPathPosition = AIController->GetPathFinder()->getPathFromTo(logicCharacter->GetCurrentLogicalPosition(), AIController->turnTargetPosition);

	for (PD_MG_LogicPosition pathPosition : listPathPosition) {
		UE_LOG(LogTemp, Log, TEXT("UPD_IA_TaskDefenseCreateOrder:: Añadiendo movimiento:%d , %d "), pathPosition.GetX(), pathPosition.GetY());
		FStructLogicPosition logicPositionStruct = FStructLogicPosition();

		logicPositionStruct.positionX = pathPosition.GetX();
		logicPositionStruct.positionY = pathPosition.GetY();

		turnStruct->positionsToMove.Add(logicPositionStruct);
	}
}

void UPD_IA_TaskDefenseCreateOrder::CreateAttackOrders(UBehaviorTreeComponent& OwnerComp) {
	APD_AIController* AIController = (APD_AIController*)OwnerComp.GetAIOwner();
	PD_GM_LogicCharacter* logicCharacter = ((APD_E_Character*)AIController->GetPawn())->logic_character;
	FStructTurnOrders* turnStruct = AIController->GetTurnOrders();

	//Ataque
	for (int iAttack = 0; iAttack < AIController->turnNumAttacks; iAttack++) {
		UE_LOG(LogTemp, Log, TEXT("UPD_IA_TaskDefenseCreateOrder:: Añadiendo ataque:%s "), *AIController->turnTargetCharacter->GetIDCharacter());
		FStructTargetToAction targetToActionStruct;
		targetToActionStruct.id_action = 0; //ataque
		targetToActionStruct.id_character.Add(AIController->turnTargetCharacter->GetIDCharacter());
		turnStruct->actions.Add(targetToActionStruct);
	}
}

void UPD_IA_TaskDefenseCreateOrder::CreateDefenseOrders(UBehaviorTreeComponent& OwnerComp) {
	APD_AIController* AIController = (APD_AIController*)OwnerComp.GetAIOwner();
	FStructTurnOrders* turnStruct = AIController->GetTurnOrders();

	UE_LOG(LogTemp, Log, TEXT("UPD_IA_TaskDefenseCreateOrder:: Añadiendo DEFENSA: "));
	FStructTargetToAction targetToActionStruct;
	targetToActionStruct.id_action = 1;//defensa
	turnStruct->actions.Add(targetToActionStruct);
}
