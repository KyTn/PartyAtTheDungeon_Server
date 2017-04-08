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
			
			FStructOrderAction attackOrder = FStructOrderAction();
			attackOrder.orderType = static_cast<uint8>(EOrderAction::Attack);
			FStructLogicPosition logicPositionStruct = FStructLogicPosition();
			logicPositionStruct.positionX = logicCharacterAttacked->GetCurrentLogicalPosition().GetX();
			logicPositionStruct.positionY = logicCharacterAttacked->GetCurrentLogicalPosition().GetY();
			attackOrder.targetLogicPosition = logicPositionStruct;

			turnStruct->listAttack.Add(attackOrder);
			
			/*int direccionAttack = OwnerComp.GetBlackboardComponent()->GetValueAsInt("direccionAttack");
			switch (direccionAttack) {
				case 1:
					;
					//atacar arriba
					break;
				case 2:
					;
					//atacar abajo
					break;
				case 3:
					;
					//atacar derecha
					break;
				case 4:
					;
					//atacar izquierda
					break;
				default:
					;
					break;
			}*/

			return EBTNodeResult::Succeeded;
		}
		else
			return EBTNodeResult::Failed;
	}
	else
		return EBTNodeResult::Failed;
}