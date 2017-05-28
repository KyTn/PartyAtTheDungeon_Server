// Fill out your copyright notice in the Description page of Project Settings.

#include "PATD_Server.h"
#include "MyPD_IA_TaskBossAttackTargetCalc.h"

#include "PATD_Server/Actors/Enemies/PD_AIController.h"
#include "GM_Game/LogicCharacter/PD_GM_LogicCharacter.h"
#include "Actors/PD_E_Character.h"
#include "GM_Game/Pathfinder/PD_GM_Pathfinder.h"


EBTNodeResult::Type UMyPD_IA_TaskBossAttackTargetCalc::ExecuteTask(UBehaviorTreeComponent & OwnerComp, uint8 * NodeMemory)
{
	CalculateTurnTarget(OwnerComp);
	CalculateFireball(OwnerComp);
	SetMinionsBehaviour(OwnerComp);


return EBTNodeResult::Succeeded;
	

}

bool UMyPD_IA_TaskBossAttackTargetCalc::CalculateFireball(UBehaviorTreeComponent& OwnerComp) {
	APD_AIController* AIController = (APD_AIController*)OwnerComp.GetAIOwner();
	PD_GM_LogicCharacter* logicCharacter = ((APD_E_Character*)AIController->GetPawn())->logic_character;
	FStructTurnOrders* turnStruct = AIController->GetTurnOrders();

	UE_LOG(LogTemp, Log, TEXT("UMyPD_IA_TaskBossAttackTargetCalc:: Empezando a ver si hay bola de fuego "));

	if (AIController->turnNumAttacks == 0) {
		UE_LOG(LogTemp, Log, TEXT("UMyPD_IA_TaskBossAttackTargetCalc:: No hay ataques, probando con bola de fuego "));
		if (AIController->CheckInRangeFromPositionToCharacter(AIController->turnTargetPosition, AIController->goalCharacter, logicCharacter->skills->listActiveSkills[0].range)) {
			UE_LOG(LogTemp, Log, TEXT("UMyPD_IA_TaskBossAttackTargetCalc:: Añadiendo bola de fuego "));
			FStructTargetToAction targetToActionStruct;
			targetToActionStruct.id_action = logicCharacter->skills->listActiveSkills[0].ID_Skill;
			targetToActionStruct.id_character.Add(AIController->goalCharacter->GetIDCharacter());
			turnStruct->actions.Add(targetToActionStruct);
		
		}
	}
	//turnStruct->
	return true;
}

bool UMyPD_IA_TaskBossAttackTargetCalc::SetMinionsBehaviour(UBehaviorTreeComponent& OwnerComp) {
	UE_LOG(LogTemp, Log, TEXT("UMyPD_IA_TaskBossAttackTargetCalc:: SetMinionsBehaviour"));
	APD_AIController* AIController = (APD_AIController*)OwnerComp.GetAIOwner();
	PD_GM_LogicCharacter* logicCharacter = ((APD_E_Character*)AIController->GetPawn())->logic_character;

	//Get_LogicPosition_Diagonals_And_Adyacents_To()
	TArray<PD_MG_LogicPosition> listAdyacentPosition = AIController->GetValidPositionsAdyacentsTo(AIController->goalCharacter->GetCurrentLogicalPosition());
	TArray<PD_GM_LogicCharacter*> listMinions = AIController->GetEnemiesInRange(rangeControlMinions);

	UE_LOG(LogTemp, Log, TEXT("UPD_IA_TaskBossRetreatTargetCalc:: SetMinionsBehaviour listAdyacentPosition: %d  listMinions: %d"), listAdyacentPosition.Num(), listMinions.Num());
	int iEnemy = 0;
	for (PD_MG_LogicPosition positionToCover : listAdyacentPosition) {
		UE_LOG(LogTemp, Log, TEXT("UPD_IA_TaskBossRetreatTargetCalc:: SetMinionsBehaviour iEnemy %d  position:%d,%d"), iEnemy, positionToCover.GetX(), positionToCover.GetY());

		if (iEnemy >= listMinions.Num()) break;

		if (positionToCover == AIController->turnTargetPosition) {

		}
		else { //solo usamos a un minion si la posicion no es al a que vamos a ir nosotros
			APD_AIController* minionController = ((APD_AIController*)listMinions[iEnemy]->GetController());
			minionController->selectedBehaviour = EIABehaviour::Attack;
			minionController->turnsForGoal = 2;
			minionController->SetGoalCharacterAndPosition(AIController->goalCharacter,positionToCover);
			iEnemy++;

		}



	}

	return true;
}