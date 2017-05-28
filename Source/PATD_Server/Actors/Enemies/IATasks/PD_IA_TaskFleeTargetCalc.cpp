// Fill out your copyright notice in the Description page of Project Settings.

#include "PATD_Server.h"
#include "PD_IA_TaskFleeTargetCalc.h"

#include "PATD_Server/Actors/Enemies/PD_AIController.h"
#include "GM_Game/LogicCharacter/PD_GM_LogicCharacter.h"
#include "Actors/PD_E_Character.h"
#include "MapGeneration/PD_MG_LogicPosition.h"

EBTNodeResult::Type UPD_IA_TaskFleeTargetCalc::ExecuteTask(UBehaviorTreeComponent & OwnerComp, uint8 * NodeMemory)
{
	if (CalculateTurnTarget(OwnerComp)) {
		return EBTNodeResult::Succeeded;
	}
	else {
		return EBTNodeResult::Failed;
	}
}


bool UPD_IA_TaskFleeTargetCalc::CalculateTurnTarget(UBehaviorTreeComponent& OwnerComp) {
	UE_LOG(LogTemp, Log, TEXT("UPD_IA_TaskFleeTurnCalc:: CalculateTurnTarget"));
	APD_AIController* AIController = (APD_AIController*)OwnerComp.GetAIOwner();
	PD_GM_LogicCharacter* logicCharacter = ((APD_E_Character*)AIController->GetPawn())->logic_character;

	//Por si acaso se llamase sin setear, por ejemplo el
	//if(!AIController->goalCharacter) AIController->goalCharacter= AIController->GetClosestPlayer();

	TArray<PD_MG_LogicPosition> listPathPosition;

	bool noMove=false;

	if (AIController->goalCharacter) {
		listPathPosition = AIController->GetPathFinder()->getPathFromTo(logicCharacter->GetCurrentLogicalPosition(), AIController->goalCharacter->GetCurrentLogicalPosition());
		if (listPathPosition.Num() == 1) { //Esta justo al lado
			noMove = true;
		}
		listPathPosition.RemoveAt(listPathPosition.Num() - 1); //Quitamos la ultima porque seria para ponerse encima, para chocar.
	}
	else {
		if (logicCharacter->GetCurrentLogicalPosition() == AIController->goalPosition) {
			//ya esta en la posicion
			noMove = true;
		}
		else {
			listPathPosition = AIController->GetPathFinder()->getPathFromTo(logicCharacter->GetCurrentLogicalPosition(), AIController->goalPosition);
			
		}
	}
	
	if (listPathPosition.Num() == 0 && noMove==false) { 
		//Error no puede llegar.
		return false;
	}

	if (noMove == false) {
		int AP = logicCharacter->GetTotalStats()->APTotal;
		UE_LOG(LogTemp, Log, TEXT("UPD_IA_TaskFleeTurnCalc:: AP iniciales:%d "), AP);

		int indexPath = 0;
		while (AP > 0 && indexPath < listPathPosition.Num()) {
				PD_MG_LogicPosition pathPosition = listPathPosition[indexPath];
				//AIController->CheckInRangeAtPosition()
				indexPath++;
				AP--;
				//Coge la ultima a la que llege por el ap, pero si hay una en la que ya tenga rango, aunque le queden AP sale del bucle.
				AIController->turnTargetPosition = pathPosition;

			}
	}
	else { //En este caso no necesita moverse para llegar al objetivo.
		AIController->turnTargetPosition = AIController->goalPosition;
	}

	if (AIController->turnTargetPosition == AIController->goalPosition) {
		//si ha llegado al punto en este turno, reseteamos turnsForGoal para recibir nuevo comportamiento.
		AIController->turnsForGoal = 0;
	}

	return true;
}