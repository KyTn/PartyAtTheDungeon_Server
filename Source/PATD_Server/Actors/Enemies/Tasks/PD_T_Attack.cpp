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
	APD_AIController* AIController = (APD_AIController*)OwnerComp.GetAIOwner();

	int ap = OwnerComp.GetBlackboardComponent()->GetValueAsInt("AP");
	UE_LOG(LogTemp, Log, TEXT("PD_T_Attack:: task attack empezando .ap:%d idEnemy:%s"), ap, *AIController->GetLogicCharacter()->GetIDCharacter());
	
	if (ap>0)
	{
		//	UE_LOG(LogTemp, Log, TEXT("PD_T_Attack:: task attack tiene ap .ap:%d idEnemy:%s"), ap, *AIController->GetLogicCharacter()->GetIDCharacter());
		//bool arePlayersNear = OwnerComp.GetBlackboardComponent()->GetValueAsBool("ArePlayersNear");
		//if (arePlayersNear) 
		if(true)
		{


			FString idCharacterAttacked = OwnerComp.GetBlackboardComponent()->GetValueAsString("idCharacterAttacked");
	

			UPD_ServerGameInstance* SGI = Cast<UPD_ServerGameInstance>(AIController->GetGameInstance());
			PD_GM_LogicCharacter* logicCharacterAttacked =SGI->getPlayerManager()->GetCharacterByID(idCharacterAttacked);
			
			FStructTurnOrders* turnStruct =AIController->GetTurnOrders();
			
			FStructTargetToAction attackOrder = FStructTargetToAction();
			attackOrder.id_action = 0; //Ataque basico
			attackOrder.id_character.Add(idCharacterAttacked);
			turnStruct->actions.Add(attackOrder);

			//Poner el ap a 0 indica que ha acabado el calculo.
			//UE_LOG(LogTemp, Log, TEXT("PD_T_Attack:: task attack con player near - finalizando .ap:%d idEnemy:%s"), ap, *AIController->GetLogicCharacter()->GetIDCharacter());
			OwnerComp.GetBlackboardComponent()->SetValueAsInt("AP", 0);
			return EBTNodeResult::Succeeded;
		}
		else {
			//UE_LOG(LogTemp, Log, TEXT("PD_T_Attack:: task attack sin player near - finalizando .ap:%d idEnemy:%s"), ap, *AIController->GetLogicCharacter()->GetIDCharacter());
			OwnerComp.GetBlackboardComponent()->SetValueAsInt("AP", 0);
			return EBTNodeResult::Succeeded;
		}
			
	}
	else {
		//UE_LOG(LogTemp, Log, TEXT("PD_T_Attack:: task attack sin ap - finalizando  .ap:%d idEnemy:%s"), ap, *AIController->GetLogicCharacter()->GetIDCharacter());
		return EBTNodeResult::Succeeded;
	}
		
}