// Fill out your copyright notice in the Description page of Project Settings.

#include "PATD_Server.h"
#include "PD_IA_TaskBossRetreatTargetCalc.h"

#include "PATD_Server/Actors/Enemies/PD_AIController.h"
#include "GM_Game/LogicCharacter/PD_GM_LogicCharacter.h"
#include "Actors/PD_E_Character.h"
#include "GM_Game/Pathfinder/PD_GM_Pathfinder.h"

EBTNodeResult::Type UPD_IA_TaskBossRetreatTargetCalc::ExecuteTask(UBehaviorTreeComponent & OwnerComp, uint8 * NodeMemory)
{

	APD_AIController* AIController = (APD_AIController*)OwnerComp.GetAIOwner();
	PD_GM_LogicCharacter* logicCharacter = ((APD_E_Character*)AIController->GetPawn())->logic_character;

	this->CalculateTurnTarget(OwnerComp);
	int rollUseMinions = logicCharacter->GetARoll(1, 100);
	if (rollUseMinions > probToUseMinions) {
		this->SetMinionsBehaviour(OwnerComp);
	}
	this->SetHealConsumible(OwnerComp);



	return EBTNodeResult::Succeeded;
}

bool UPD_IA_TaskBossRetreatTargetCalc::SetMinionsBehaviour(UBehaviorTreeComponent& OwnerComp) {
	UE_LOG(LogTemp, Log, TEXT("UPD_IA_TaskBossRetreatTargetCalc:: SetMinionsBehaviour"));
	APD_AIController* AIController = (APD_AIController*)OwnerComp.GetAIOwner();
	PD_GM_LogicCharacter* logicCharacter = ((APD_E_Character*)AIController->GetPawn())->logic_character;

	//Get_LogicPosition_Diagonals_And_Adyacents_To()
	TArray<PD_MG_LogicPosition> listAdyacentPosition = AIController->GetValidPositionsAdyacentsTo(AIController->turnTargetPosition);
	TArray<PD_GM_LogicCharacter*> listMinions = AIController->GetEnemiesInRange(10);

	UE_LOG(LogTemp, Log, TEXT("UPD_IA_TaskBossRetreatTargetCalc:: SetMinionsBehaviour listAdyacentPosition: %d  listMinions: %d"),listAdyacentPosition.Num(), listMinions.Num());
	int iEnemy = 0;
	for (PD_MG_LogicPosition positionToCover:listAdyacentPosition) {
		UE_LOG(LogTemp, Log, TEXT("UPD_IA_TaskBossRetreatTargetCalc:: SetMinionsBehaviour iEnemy %d  position:%d,%d"), iEnemy, positionToCover.GetX(), positionToCover.GetY());

		if (iEnemy >= listMinions.Num()) break;

		APD_AIController* minionController = ((APD_AIController*)listMinions[iEnemy]->GetController());
		minionController->selectedBehaviour = EIABehaviour::Flee;
		minionController->turnsForGoal = 2;
		minionController->goalPosition = positionToCover;
		minionController->goalCharacter = nullptr;
		iEnemy++;
	}

		return true;
}


bool  UPD_IA_TaskBossRetreatTargetCalc::SetHealConsumible(UBehaviorTreeComponent& OwnerComp) {
	APD_AIController* AIController = (APD_AIController*)OwnerComp.GetAIOwner();
	PD_GM_LogicCharacter* logicCharacter = ((APD_E_Character*)AIController->GetPawn())->logic_character;
	FStructTurnOrders* turnStruct = AIController->GetTurnOrders();

	turnStruct->consumablesToConsume.Add((int)ConsumableItems::enemyHeal);
	return true;
}