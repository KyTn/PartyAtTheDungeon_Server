// Fill out your copyright notice in the Description page of Project Settings.

#include "PATD_Server.h"
#include "PD_IA_TaskAttackTargetCalc.h"

#include "PATD_Server/Actors/Enemies/PD_AIController.h"
#include "GM_Game/LogicCharacter/PD_GM_LogicCharacter.h"
#include "Actors/PD_E_Character.h"
#include "MapGeneration/PD_MG_LogicPosition.h"

EBTNodeResult::Type UPD_IA_TaskAttackTargetCalc::ExecuteTask(UBehaviorTreeComponent & OwnerComp, uint8 * NodeMemory)
{
	if (CalculateTurnTarget(OwnerComp)) {
		return EBTNodeResult::Succeeded;
	}
	else {
		return EBTNodeResult::Failed;
	}
	
}


bool UPD_IA_TaskAttackTargetCalc::CalculateTurnTarget(UBehaviorTreeComponent& OwnerComp) {
	APD_AIController* AIController = (APD_AIController*)OwnerComp.GetAIOwner();
	PD_GM_LogicCharacter* logicCharacter = ((APD_E_Character*)AIController->GetPawn())->logic_character;

	//Por si acaso se llamase sin setear, por ejemplo el
	//if(!AIController->goalCharacter) AIController->goalCharacter= AIController->GetClosestPlayer();

	TArray<PD_MG_LogicPosition> listPathPosition;
	listPathPosition = AIController->GetPathFinder()->getPathFromTo(logicCharacter->GetCurrentLogicalPosition(), AIController->goalCharacter->GetCurrentLogicalPosition());
	if (listPathPosition.Num() == 0) {
		return false;
	}

	int AP= logicCharacter->GetTotalStats()->APTotal;
	UE_LOG(LogTemp, Log, TEXT("UPD_IA_TaskAttackCreateOrder:: AP iniciales:%d "), AP);

	int indexPath = 0;
	listPathPosition.RemoveAt(listPathPosition.Num() - 1); //Quitamos la ultima porque seria para ponerse encima, para chocar.
	
	if (listPathPosition.Num()>0) {
		while (AP > 0 && indexPath<listPathPosition.Num()) {
			PD_MG_LogicPosition pathPosition = listPathPosition[indexPath];
			//AIController->CheckInRangeAtPosition()
			indexPath++;
			AP--;
			//Coge la ultima a la que llege por el ap, pero si hay una en la que ya tenga rango, aunque le queden AP sale del bucle.
			AIController->turnTargetPosition = pathPosition; 
			if (AIController->CheckInRangeFromPositionToCharacter(pathPosition,AIController->goalCharacter)) {
				
				break;
			}

		}

	} //Si salta este if, significa que estamos al lado del target, por lo que solo hacemos ataques.

	if (AP > 0) {
		AIController->turnTargetCharacter = AIController->goalCharacter;
		AIController->turnNumAttacks = AP;
	}

	
	return true;
}