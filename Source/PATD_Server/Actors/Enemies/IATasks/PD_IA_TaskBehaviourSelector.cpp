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


	UE_LOG(LogTemp, Log, TEXT("UPD_IA_TaskBehaviourSelector:: Id:%s  PeronalityCode:%d  EnemyType:%d"), *logicCharacter->GetIDCharacter(), (int)logicCharacter->GetIAPersonality(), (int)logicCharacter->GetTypeCharacter());
	SelectBehaviour(OwnerComp);
	SelectGoals(OwnerComp);
	
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

void UPD_IA_TaskBehaviourSelector::SelectBehaviour(UBehaviorTreeComponent & OwnerComp) {


	APD_AIController* AIController = (APD_AIController*)OwnerComp.GetAIOwner();
	PD_GM_LogicCharacter* logicCharacter = ((APD_E_Character*)AIController->GetPawn())->logic_character;
	float percentHP = (logicCharacter->GetTotalStats()->HPCurrent / logicCharacter->GetTotalStats()->HPTotal) * 100;

	TArray<TEnumAsByte<EIABehaviour>> validBehaviours;
	//mele 
//	if (logicCharacter->GetTypeCharacter() == ECharacterType::OrcMelee) {

		//Se elige entre atacar, defender, huir y berserker por porcentaje de vida parametizable inicialmente

		//Aqui se ponen en comun los parametros dependiendo de la personalidad
		/*float minHPDefenseCalc = minHPDefense;
		float minHPFleeCalc = minHPFlee;
		float minHPBerserkerCalc = minHPBerserker;
		if (logicCharacter->GetIAPersonality() == EIAPersonality::Coward) {
			minHPDefenseCalc = coward_minHPDefense;
			minHPFleeCalc = coward_minHPFlee;
			minHPBerserkerCalc = coward_minHPBerserker;
		}
		else if (logicCharacter->GetIAPersonality() == EIAPersonality::Warlike) {
			minHPDefenseCalc = warlike_minHPDefense;
			minHPFleeCalc = warlike_minHPFlee;
			minHPBerserkerCalc = warlike_minHPBerserker;
		}*/

		

		//Aqui se comprueba el porcentaje de vida real y se elige el comportamiento
		if (percentHP > minHPHalfLife) { // Set de comportamientos a full vida

		//	
			validBehaviours = behaviourSetFullHP;
			/*validBehaviours.Add(EIABehaviour::Flee);
			validBehaviours.Add(EIABehaviour::Attack);
			validBehaviours.Add(EIABehaviour::Defense);
			validBehaviours.Add(EIABehaviour::Swindler);*/
			UE_LOG(LogTemp, Log, TEXT("UPD_IA_TaskBehaviourSelector:: SelectedBehaviour: bloque FULL_LIFE"));
			
		}
		else if (percentHP < minHPHalfLife && percentHP>minHPAlmostDead) { //Set de comportamientos a media vida

			validBehaviours = behaviourSetHalfLife;

			/*validBehaviours.Add(EIABehaviour::Attack);
			validBehaviours.Add(EIABehaviour::Defense);
			validBehaviours.Add(EIABehaviour::Swindler);
			validBehaviours.Add(EIABehaviour::Flee);*/
			
			
			UE_LOG(LogTemp, Log, TEXT("UPD_IA_TaskBehaviourSelector:: SelectedBehaviour: bloque HALF_LIFE"));
		}
		else if (percentHP < minHPAlmostDead) { //Set de comportamientos casi muerto

			validBehaviours = behaviourSetAlmostDead;

			/*validBehaviours.Add(EIABehaviour::Defense);
			validBehaviours.Add(EIABehaviour::Flee);
			validBehaviours.Add(EIABehaviour::Berserker);*/
			UE_LOG(LogTemp, Log, TEXT("UPD_IA_TaskBehaviourSelector:: SelectedBehaviour: bloque ALMOST_DEAD"));
		}

		//Casos especiales

	//}
	//Rango
/*	else if (logicCharacter->GetTypeCharacter() == ECharacterType::OrcBow
		|| logicCharacter->GetTypeCharacter() == ECharacterType::OrcGuns) {


	}
	*/

	SelectInSetBehaviour(OwnerComp, validBehaviours);
}

void UPD_IA_TaskBehaviourSelector::SelectInSetBehaviour(UBehaviorTreeComponent & OwnerComp, TArray<TEnumAsByte<EIABehaviour>> validBehaviours) {
	APD_AIController* AIController = (APD_AIController*)OwnerComp.GetAIOwner();
	PD_GM_LogicCharacter* logicCharacter = ((APD_E_Character*)AIController->GetPawn())->logic_character;

	bool behaviourSelected = false;
	for (int iBehaviours = 0; iBehaviours < validBehaviours.Num(); iBehaviours++) {
		
		if (behaviourSelected) break;

		int roll= logicCharacter->GetARoll(1, 100);
		switch (validBehaviours[iBehaviours].GetValue()) {
		case EIABehaviour::Attack: 
		{
			UE_LOG(LogTemp, Log, TEXT("UPD_IA_TaskBehaviourSelector:: SelectInSetBehaviour: probando con Attack"));
			if (logicCharacter->GetIAPersonality() == EIAPersonality::Warlike) {
				roll += 10;
			}

			//if para seleccionar
			if (roll < attackProb) {
				AIController->selectedBehaviour = EIABehaviour::Attack;
				behaviourSelected = true;
			}
			break;
		}
		case EIABehaviour::Defense:
		{
			UE_LOG(LogTemp, Log, TEXT("UPD_IA_TaskBehaviourSelector:: SelectInSetBehaviour: probando con Defense"));

			if (roll < defenseProb) {
				AIController->selectedBehaviour = EIABehaviour::Defense;
				behaviourSelected = true;
			}
			break;
		}
		case EIABehaviour::Flee:
		{
			UE_LOG(LogTemp, Log, TEXT("UPD_IA_TaskBehaviourSelector:: SelectInSetBehaviour: probando con Flee"));

			if (roll < fleeProb) {
				AIController->selectedBehaviour = EIABehaviour::Flee;
				behaviourSelected = true;
			}
			break;
		}
		case EIABehaviour::Swindler:
		{
			UE_LOG(LogTemp, Log, TEXT("UPD_IA_TaskBehaviourSelector:: SelectInSetBehaviour: probando con Swindler"));

			if (roll < swindlerProb) {
				AIController->selectedBehaviour = EIABehaviour::Swindler;
				behaviourSelected = true;
			}
			break;
		}
		case EIABehaviour::Berserker:
		{
			UE_LOG(LogTemp, Log, TEXT("UPD_IA_TaskBehaviourSelector:: SelectInSetBehaviour: probando con Berserker"));

			if (roll < berserkerProb) {
				AIController->selectedBehaviour = EIABehaviour::Berserker;
				behaviourSelected = true;
			}
			break;
		}
		default:
			break;
		}
	}

	if (!behaviourSelected) {
		if (logicCharacter->GetIAPersonality() == EIAPersonality::Coward) {
			AIController->selectedBehaviour =  cowardDefaultBehaviour.GetValue();

		}
		else if (logicCharacter->GetIAPersonality() == EIAPersonality::Warlike) {
			AIController->selectedBehaviour = warlikeDefaultBehaviour.GetValue();

		}
		else if (logicCharacter->GetIAPersonality() == EIAPersonality::Smart) {
			AIController->selectedBehaviour = smartDefaultBehaviour.GetValue();

		}
		else if (logicCharacter->GetIAPersonality() == EIAPersonality::Neutral) {
			AIController->selectedBehaviour = neutralDefaultBehaviour.GetValue();

		}

	}

}



void UPD_IA_TaskBehaviourSelector::SelectGoals(UBehaviorTreeComponent & OwnerComp) {
	APD_AIController* AIController = (APD_AIController*)OwnerComp.GetAIOwner();
	PD_GM_LogicCharacter* logicCharacter = ((APD_E_Character*)AIController->GetPawn())->logic_character;
	//TArray<PD_GM_LogicCharacter*>listPlayersInRange = AIController->GetPlayersInRange();


	switch (AIController->selectedBehaviour)
	//switch(EIABehaviour::Attack)
	{
		case EIABehaviour::Attack:
		{
			UE_LOG(LogTemp, Log, TEXT("UPD_IA_TaskBehaviourSelector:: SelectGoals: Attack"));

			//Standard
			AIController->goalCharacter = AIController->GetClosestPlayer();
			AIController->turnsForGoal = 1; //En standard ponemos solo 1 turno para la accion, porque el objetivo es el mas proximo, que se deberia recalcular.

			if (logicCharacter->GetIAPersonality() == EIAPersonality::Coward) {
			}
			break;
		}
		case EIABehaviour::Defense:
		{
			UE_LOG(LogTemp, Log, TEXT("UPD_IA_TaskBehaviourSelector:: SelectGoals: Defense"));
			AIController->goalCharacter = AIController->GetClosestPlayer();
			AIController->turnsForGoal = 1; //En standard ponemos solo 1 turno para la accion, porque el objetivo es el mas proximo, que se deberia recalcular.
			break;
		}
		case EIABehaviour::Flee:
		{
			UE_LOG(LogTemp, Log, TEXT("UPD_IA_TaskBehaviourSelector:: SelectGoals: Flee"));
			//PD_MG_LogicPosition positionToFlee = AIController->GetClosestDoorPosition();
			if (AIController->GetMostHPEnemy()->GetIDCharacter()!= logicCharacter->GetIDCharacter()){
				PD_MG_LogicPosition positionToFlee = AIController->GetMostHPEnemy()->GetCurrentLogicalPosition();
				AIController->goalPosition = positionToFlee;
			}
			else {
				//Si no hay nadie con mas vida pasa a modo berserker
				AIController->selectedBehaviour = EIABehaviour::Defense;
				SelectGoals( OwnerComp);
			}

			if (logicCharacter->GetIAPersonality() == EIAPersonality::Coward) {
				//Falta que puedan elegir otras posiciones, como el inteligente. o aleatoria el cobarde
			}
			AIController->turnsForGoal = 1;
			break;
			
		}
		case EIABehaviour::Berserker:
		{
			UE_LOG(LogTemp, Log, TEXT("UPD_IA_TaskBehaviourSelector:: SelectGoals: Berserker"));
			//Standard
			AIController->goalCharacter = AIController->GetLeastHPPlayer();
			//AIController->goalCharacter = AIController->GetClosestPlayer();
			//AIController->turnsForGoal = 1; //En standard ponemos solo 1 turno para la accion, porque el objetivo es el mas proximo, que se deberia recalcular.
			AIController->turnsForGoal = 1000;

			if (logicCharacter->GetIAPersonality() == EIAPersonality::Smart) {
				 
				AIController->goalCharacter = AIController->GetLeastHPPlayer();

			}else if (logicCharacter->GetIAPersonality() == EIAPersonality::Warlike) {
				//AIController->turnsForGoal = 1000; //Ponemos esto para que nunca deje de perseguir a ese mismo objetivo
			}	
			else if (logicCharacter->GetIAPersonality() == EIAPersonality::Coward) {
				//AIController->turnsForGoal = 2; //Este comportamiento tiene 2 fases, una en la que ataca y otra en el que huye
			}
			break;
		}
		default:
			break;
	};


}