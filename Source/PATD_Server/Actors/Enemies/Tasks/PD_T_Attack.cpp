// Fill out your copyright notice in the Description page of Project Settings.

#include "PATD_Server.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/Blackboard/BlackboardKeyAllTypes.h"
#include "PATD_Server/Actors/PD_E_Character.h"
#include "PD_T_Attack.h"

#include "PATD_Server/Actors/Enemies/PD_AIController.h"
#include "PD_ServerGameInstance.h"
#include "PD_PlayersManager.h"
#include "GM_Game/LogicCharacter/PD_GM_LogicCharacter.h"

EBTNodeResult::Type UPD_T_Attack::ExecuteTask(UBehaviorTreeComponent & OwnerComp, uint8 * NodeMemory)
{
	int ap = OwnerComp.GetBlackboardComponent()->GetValueAsInt("AP");
	if (ap>0)
	{
		bool arePlayersNear = OwnerComp.GetBlackboardComponent()->GetValueAsBool("ArePlayersNear");
		if (arePlayersNear) 
		{


			FString idCharacterAttacked = OwnerComp.GetBlackboardComponent()->GetValueAsString("idCharacterAttacked");
			APD_AIController* AIController = (APD_AIController*)OwnerComp.GetAIOwner();

			UPD_ServerGameInstance* SGI = Cast<UPD_ServerGameInstance>(AIController->GetGameInstance());
			PD_GM_LogicCharacter* logicCharacterAttacked =SGI->getPlayerManager()->GetCharacterByID(idCharacterAttacked);
			
			FStructTurnOrders* turnStruct =AIController->GetTurnOrders();
			
			FStructTargetToAction attackOrder = FStructTargetToAction();
			attackOrder.id_action = 0; //Ataque basico
			attackOrder.id_character.Add(idCharacterAttacked);
			turnStruct->actions.Add(attackOrder);

			return EBTNodeResult::Succeeded;
		}
		else
			return EBTNodeResult::Failed;
	}
	else
		return EBTNodeResult::Failed;
}