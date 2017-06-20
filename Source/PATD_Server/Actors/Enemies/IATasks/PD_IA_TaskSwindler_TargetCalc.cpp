// Fill out your copyright notice in the Description page of Project Settings.

#include "PATD_Server.h"
#include "PD_IA_TaskSwindler_TargetCalc.h"

#include "PATD_Server/Actors/Enemies/PD_AIController.h"
#include "GM_Game/LogicCharacter/PD_GM_LogicCharacter.h"
#include "Actors/PD_E_Character.h"
#include "MapGeneration/PD_MG_LogicPosition.h"

EBTNodeResult::Type UPD_IA_TaskSwindler_TargetCalc::ExecuteTask(UBehaviorTreeComponent & OwnerComp, uint8 * NodeMemory)
{
	if (CalculateTurnTarget(OwnerComp)) {
		return EBTNodeResult::Succeeded;
	}
	else {
		return EBTNodeResult::Failed;
	}

}


bool UPD_IA_TaskSwindler_TargetCalc::CalculateTurnTarget(UBehaviorTreeComponent& OwnerComp) {
	UE_LOG(LogTemp, Log, TEXT("UPD_IA_TaskAttackTargetCalc:: Entrando a comportamiento de Swindler "));

	APD_AIController* AIController = (APD_AIController*)OwnerComp.GetAIOwner();
	PD_GM_LogicCharacter* logicCharacter = ((APD_E_Character*)AIController->GetPawn())->logic_character;

	PD_MG_LogicPosition posToGo = AIController->GetActivationPosition(AIController->goalInteractuable);
	TArray<PD_MG_LogicPosition> listPathPosition;

	if (posToGo == logicCharacter->GetCurrentLogicalPosition()) {
		AIController->activateThisTurn = true;
		AIController->turnsForGoal = 0;
	}
	else {
		listPathPosition = AIController->GetPathFinder()->getPathFromTo(logicCharacter->GetCurrentLogicalPosition(), posToGo);

		if (listPathPosition.Num() == 0) {
			return false;
		}
		else {


			int AP = logicCharacter->GetTotalStats()->APTotal;
			UE_LOG(LogTemp, Log, TEXT("UPD_IA_TaskSwindler_TargetCalc:: AP iniciales:%d "), AP);

			int indexPath = 0;
			while (AP > 0 && indexPath < listPathPosition.Num()) {
				PD_MG_LogicPosition pathPosition = listPathPosition[indexPath];
				//AIController->CheckInRangeAtPosition()
				indexPath++;
				AP--;
				//Coge la ultima a la que llege por el ap, pero si hay una en la que ya tenga rango, aunque le queden AP sale del bucle.
				if (AIController->CanEndMoveAtPosition(pathPosition)) {
					AIController->turnTargetPosition = pathPosition;
				}

			}

			if (posToGo == AIController->turnTargetPosition) {
				AIController->activateThisTurn = true;
				AIController->turnsForGoal = 0;
			}
		}
	}

	return true;
}