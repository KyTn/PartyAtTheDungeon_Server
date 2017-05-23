// Fill out your copyright notice in the Description page of Project Settings.

#include "PATD_Server.h"
#include "PD_IA_TaskBehaviourSelector.h"

#include "PATD_Server/Actors/Enemies/PD_AIController.h"
#include "GM_Game/LogicCharacter/PD_GM_LogicCharacter.h"
#include "Actors/PD_E_Character.h"

EBTNodeResult::Type UPD_IA_TaskBehaviourSelector::ExecuteTask(UBehaviorTreeComponent & OwnerComp, uint8 * NodeMemory)
{
	APD_AIController* AIController = (APD_AIController*)OwnerComp.GetAIOwner();
	PD_GM_LogicCharacter* logicCharacter = ((APD_E_Character*)AIController->GetPawn())->logic_character;
	float percentHP = (logicCharacter->GetTotalStats()->HPCurrent / logicCharacter->GetTotalStats()->HPTotal) * 100;
	TArray<PD_GM_LogicCharacter*> playersInRangeList = AIController->GetPlayersInRange();

	//mele 
	if (logicCharacter->GetTypeCharacter() == ECharacterType::OrcMelee) {

		float minHPDefenseCalc = minHPDefense;
		float minHPFleeCalc = minHPFlee;
		float minHPBerserkerCalc = minHPBerserker;
		if (logicCharacter->GetIAPersonality() == EIAPersonality::Coward) {
			 minHPDefenseCalc = coward_minHPDefense;
			 minHPFleeCalc =coward_minHPFlee;
			 minHPBerserkerCalc = coward_minHPBerserker;
		}
		else if (logicCharacter->GetIAPersonality() == EIAPersonality::Warlike) {
			 minHPDefenseCalc = warlike_minHPDefense;
			 minHPFleeCalc = warlike_minHPFlee;
			 minHPBerserkerCalc = warlike_minHPBerserker;
		}

		//Standard
		if (percentHP > minHPDefenseCalc) {
			
			AIController->selectedBehaviour = EIABehaviour::Attack;
		
		}else if (percentHP < minHPDefenseCalc && percentHP>minHPFleeCalc) {
			
			AIController->selectedBehaviour = EIABehaviour::Defense;
		}
		else if (percentHP < minHPFleeCalc && percentHP>minHPBerserkerCalc) {
			
			AIController->selectedBehaviour = EIABehaviour::Flee;
		}
		else if (percentHP < minHPBerserkerCalc) {
			AIController->selectedBehaviour = EIABehaviour::Berserker;
		}

		//Casos especiales

	}
	//Rango
	else if (logicCharacter->GetTypeCharacter() == ECharacterType::OrcBow 
		|| logicCharacter->GetTypeCharacter() == ECharacterType::OrcGuns) {


	}


/*
	switch (logicCharacter->GetIAPersonality())
	{
		
	default:
		break;
	}*/

	return EBTNodeResult::Succeeded;

}

/*
UPD_IA_TaskBehaviourSelector::UPD_IA_TaskBehaviourSelector(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	NodeName = "UPD_IA_TaskBehaviourSelector";

	// empty KeySelector = allow everything
}*/