// Fill out your copyright notice in the Description page of Project Settings.

#include "PATD_Server.h"
#include "PD_IA_TaskDefenseTargetCalc.h"
#include "PATD_Server/Actors/Enemies/PD_AIController.h"
#include "GM_Game/LogicCharacter/PD_GM_LogicCharacter.h"
#include "Actors/PD_E_Character.h"
#include "MapGeneration/PD_MG_LogicPosition.h"

EBTNodeResult::Type UPD_IA_TaskDefenseTargetCalc::ExecuteTask(UBehaviorTreeComponent & OwnerComp, uint8 * NodeMemory)
{
	if (CalculateTurnTarget(OwnerComp)) {
		return EBTNodeResult::Succeeded;
	}
	else {
		return EBTNodeResult::Failed;
	}
}


bool UPD_IA_TaskDefenseTargetCalc::CalculateTurnTarget(UBehaviorTreeComponent& OwnerComp) {
/*	APD_AIController* AIController = (APD_AIController*)OwnerComp.GetAIOwner();
	PD_GM_LogicCharacter* logicCharacter = ((APD_E_Character*)AIController->GetPawn())->logic_character;

	//Por si acaso se llamase sin setear, por ejemplo el
	//if(!AIController->goalCharacter) AIController->goalCharacter= AIController->GetClosestPlayer();

	if (!AIController->useCharacter) {
		return false;
	}

	TArray<PD_MG_LogicPosition> listPathPosition;
	listPathPosition = AIController->GetPathFinder()->getPathFromTo(logicCharacter->GetCurrentLogicalPosition(), AIController->goalCharacter->GetCurrentLogicalPosition());
	if (listPathPosition.Num() == 0) {
		//ERROR, mirar como lo tratamos
		return false;
	}

	int AP = logicCharacter->GetTotalStats()->APTotal;
	
	UE_LOG(LogTemp, Log, TEXT("UPD_IA_TaskAttackCreateOrder:: AP iniciales:%d "), AP);

	if (logicCharacter->GetIAPersonality() == EIAPersonality::Coward) {
		AP -= APDefenseCoward;
	}
	else {
		AP -= APDefense;
	}
	
	UE_LOG(LogTemp, Log, TEXT("UPD_IA_TaskAttackCreateOrder:: AP tras defensa:%d "), AP);

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
			if (AIController->CheckInRangeFromPositionToCharacter(pathPosition, AIController->goalCharacter, logicCharacter->weapon->RangeWeapon)) {

				break;
			}

		}

	} //Si salta este if, significa que estamos al lado del target, por lo que solo hacemos ataques.

	if (AP > 0) {
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

	if (AIController->turnNumAttacks > 0 && logicCharacter->GetIAPersonality() == EIAPersonality::Warlike) {
		//Si llega a atacar, despues se le olvida defender.
		AIController->turnNumAttacks += APDefense;
		AIController->turnTargetCharacter = AIController->goalCharacter;
	}
		
	return true;
	*/








	// COPIADO DEL ATAQUE Y MODIFICADO. LO QUE HABIA ANTES COMENTADO ARRIBA ESTA



	UE_LOG(LogTemp, Log, TEXT("UPD_IA_TaskDefenseTargetCalc:: Entrando a comportamiento de defensa "));

	APD_AIController* AIController = (APD_AIController*)OwnerComp.GetAIOwner();
	PD_GM_LogicCharacter* logicCharacter = ((APD_E_Character*)AIController->GetPawn())->logic_character;

	//Por si acaso se llamase sin setear, por ejemplo el
	//if(!AIController->goalCharacter) AIController->goalCharacter= AIController->GetClosestPlayer();
	TArray<PD_MG_LogicPosition> listPathPosition;
	bool noNeedToMove = false;

	//Modificacion sobre ataque
	int AP = logicCharacter->GetTotalStats()->APTotal - APDefense;
	if (logicCharacter->GetIAPersonality() == EIAPersonality::Coward) {
		AP = logicCharacter->GetTotalStats()->APTotal - APDefenseCoward;
	}


	if (AIController->useCharacter && !AIController->usePosition) { //Esto es cuando se setea solo un character como objetivo. Se usa cuando coge el objetivo el mismo

		if (AIController->CheckInRangeFromPositionToCharacter(logicCharacter->GetCurrentLogicalPosition(), AIController->goalCharacter, logicCharacter->weapon->RangeWeapon)) {
			//En este caso no necesita moverse
			AIController->turnTargetCharacter = AIController->goalCharacter;
			if (AP < APMaxAttack) {
				AIController->turnNumAttacks = AP;
			}
			else {
				AIController->turnNumAttacks = APMaxAttack;
				if (logicCharacter->GetIAPersonality() == EIAPersonality::Warlike) { //Si es warlike tiene mas ataques
					AIController->turnNumAttacks = APMaxAttackWarlike;
				}
			}
		}
		else {
			//el objetivo no esta a rango.
			listPathPosition = AIController->GetPathFinder()->getPathFromTo(logicCharacter->GetCurrentLogicalPosition(), AIController->goalCharacter->GetCurrentLogicalPosition());

			if (listPathPosition.Num() == 0) {
				UE_LOG(LogTemp, Log, TEXT("UPD_IA_TaskDefenseTargetCalc:: Return error porque no hay ruta"));
				return false; //sin objetivo character no podemos hacer el ataque de momento
			}
			/*if (listPathPosition.Num() == 1) {
			noNeedToMove = true;
			}*/
			listPathPosition.RemoveAt(listPathPosition.Num() - 1); //Quitamos la ultima porque seria para ponerse encima, para chocar.
			


			int indexPath = 0;
			while (AP > 0 && indexPath < listPathPosition.Num()) {
				PD_MG_LogicPosition pathPosition = listPathPosition[indexPath];
				//AIController->CheckInRangeAtPosition()
				indexPath++;
				AP--;
				//Coge la ultima a la que llege por el ap, pero si hay una en la que ya tenga rango, aunque le queden AP sale del bucle.
				AIController->turnTargetPosition = pathPosition;
				if (AIController->CheckInRangeFromPositionToCharacter(pathPosition, AIController->goalCharacter, logicCharacter->weapon->RangeWeapon)) {
					break;
				}

			}

			//Al salir del bucle nos pueden sobrar ap para atacar o no. Tambien podemos estar en rango o no.
			if (AP > 0 && AIController->CheckInRangeFromPositionToCharacter(AIController->turnTargetPosition, AIController->goalCharacter, logicCharacter->weapon->RangeWeapon)) {
				AIController->turnTargetCharacter = AIController->goalCharacter;
				if (AP < APMaxAttack) {
					AIController->turnNumAttacks = AP;
				}
				else {
					AIController->turnNumAttacks = APMaxAttack;
					if (logicCharacter->GetIAPersonality() == EIAPersonality::Warlike) { //Si es warlike tiene mas ataques
						AIController->turnNumAttacks = APMaxAttackWarlike;
					}
				}
			}


		}
	}
	else if (AIController->useCharacter && AIController->usePosition) { //Esto es cuando tiene un character y una posicion. Lo usa el jefe para dar ordenes

		if (logicCharacter->GetCurrentLogicalPosition() == AIController->goalPosition) {

			if (AIController->CheckInRangeFromPositionToCharacter(logicCharacter->GetCurrentLogicalPosition(), AIController->goalCharacter, logicCharacter->weapon->RangeWeapon)) {
				//En este caso no necesita moverse
				AIController->turnTargetCharacter = AIController->goalCharacter;
				AIController->turnNumAttacks = APMaxAttack;

				if (logicCharacter->GetIAPersonality() == EIAPersonality::Warlike) { //Si es warlike tiene mas ataques
					AIController->turnNumAttacks = APMaxAttackWarlike;
				}
			}
			else {
				UE_LOG(LogTemp, Log, TEXT("UPD_IA_TaskDefenseTargetCalc:: Error al dar orden con targetCharacter y targetPosition: Desde la posicion indicada no puede ser objetivo"));
				return false;
			}

		}
		else {
			//Hay que ir hasta la posicion
			listPathPosition = AIController->GetPathFinder()->getPathFromTo(logicCharacter->GetCurrentLogicalPosition(), AIController->goalPosition);

			if (listPathPosition.Num() == 0) {
				UE_LOG(LogTemp, Log, TEXT("UPD_IA_TaskDefenseTargetCalc:: Return error porque no hay ruta"));
				return false; //sin objetivo character no podemos hacer el ataque de momento
			}


			int indexPath = 0;
			while (AP > 0 && indexPath < listPathPosition.Num()) {
				PD_MG_LogicPosition pathPosition = listPathPosition[indexPath];
				//AIController->CheckInRangeAtPosition()
				indexPath++;
				AP--;
				//Coge la ultima a la que llege por el ap, pero si llega a la ultima le puede sobrar ap y atacar
				AIController->turnTargetPosition = pathPosition;
			}

			//Al salir del bucle nos pueden sobrar ap para atacar o no. Tambien podemos estar en rango o no.
			if (AP > 0 && AIController->CheckInRangeFromPositionToCharacter(AIController->turnTargetPosition, AIController->goalCharacter, logicCharacter->weapon->RangeWeapon)) {
				AIController->turnTargetCharacter = AIController->goalCharacter;
				if (AP < APMaxAttack) {
					AIController->turnNumAttacks = AP;
				}
				else {
					AIController->turnNumAttacks = APMaxAttack;
					if (logicCharacter->GetIAPersonality() == EIAPersonality::Warlike) { //Si es warlike tiene mas ataques
						AIController->turnNumAttacks = APMaxAttackWarlike;
					}
				}
			}

		}
	}
	else {
		UE_LOG(LogTemp, Log, TEXT("UPD_IA_TaskDefenseTargetCalc:: Return error porque no hay objetivos seteados"));
		return false; //sin objetivo character no podemos hacer el ataque de momento
	}


	if (AIController->turnNumAttacks > 0) {
		
		AIController->turnsForGoal = 0; //Si llega a hacer un ataque consideramos que ya ha cumplido su goal
	}



	return true;
}