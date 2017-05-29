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
	UE_LOG(LogTemp, Log, TEXT("UPD_IA_TaskAttackCreateOrder:: Entrando a comportamiento de ataque "));

	APD_AIController* AIController = (APD_AIController*)OwnerComp.GetAIOwner();
	PD_GM_LogicCharacter* logicCharacter = ((APD_E_Character*)AIController->GetPawn())->logic_character;

	//Por si acaso se llamase sin setear, por ejemplo el
	//if(!AIController->goalCharacter) AIController->goalCharacter= AIController->GetClosestPlayer();
	TArray<PD_MG_LogicPosition> listPathPosition;
	bool noNeedToMove = false;


	if (AIController->useCharacter && !AIController->usePosition) {
		listPathPosition = AIController->GetPathFinder()->getPathFromTo(logicCharacter->GetCurrentLogicalPosition(), AIController->goalCharacter->GetCurrentLogicalPosition());
		
		if (listPathPosition.Num() == 1) {
			noNeedToMove = true;
		}
		
		listPathPosition.RemoveAt(listPathPosition.Num() - 1); //Quitamos la ultima porque seria para ponerse encima, para chocar.

	}
	else if (AIController->useCharacter && AIController->usePosition) {

		if (logicCharacter->GetCurrentLogicalPosition() == AIController->goalPosition) {
			//ya esta en la posicion
			noNeedToMove = true;
		}
		else {
			listPathPosition = AIController->GetPathFinder()->getPathFromTo(logicCharacter->GetCurrentLogicalPosition(), AIController->goalPosition);
		}
	}
	else {
		UE_LOG(LogTemp, Log, TEXT("UPD_IA_TaskAttackCreateOrder:: Return error por que no hay objetivos seteados"));
		return false; //sin objetivo character no podemos hacer el ataque de momento
	}


	if (AIController->GetPathFinder()->error) {
		UE_LOG(LogTemp, Log, TEXT("UPD_IA_TaskAttackCreateOrder:: ErrorEnPathFinder"));

	}

	if (listPathPosition.Num() == 0 && noNeedToMove==false) {
		return false;
	}



	
	int AP= logicCharacter->GetTotalStats()->APTotal;
	UE_LOG(LogTemp, Log, TEXT("UPD_IA_TaskAttackCreateOrder:: AP iniciales:%d "), AP);
	if (noNeedToMove) {
		AIController->turnTargetPosition = logicCharacter->GetCurrentLogicalPosition();
	}
	else {
		int indexPath = 0;

		if (listPathPosition.Num() > 0) {
			while (AP > 0 && indexPath < listPathPosition.Num()) {
				PD_MG_LogicPosition pathPosition = listPathPosition[indexPath];
				//AIController->CheckInRangeAtPosition()
				indexPath++;
				AP--;
				//Coge la ultima a la que llege por el ap, pero si hay una en la que ya tenga rango, aunque le queden AP sale del bucle.
				AIController->turnTargetPosition = pathPosition;
				if (!AIController->usePosition && AIController->CheckInRangeFromPositionToCharacter(pathPosition, AIController->goalCharacter, logicCharacter->weapon->RangeWeapon)) {

					break;
				}

			}

		} //Si salta este if, significa que estamos al lado del target, por lo que solo hacemos ataques.
	}

	if (AP > 0 && AIController->CheckInRangeFromPositionToCharacter(AIController->turnTargetPosition, AIController->goalCharacter, logicCharacter->weapon->RangeWeapon)) {
		AIController->turnTargetCharacter = AIController->goalCharacter;
		if (AP > 2) //ponemos un tope de maximo 2 ATK
		{
			AIController->turnNumAttacks = 2;
		}
		else
		{
			AIController->turnNumAttacks = AP;

		}
	}

	
	return true;
}