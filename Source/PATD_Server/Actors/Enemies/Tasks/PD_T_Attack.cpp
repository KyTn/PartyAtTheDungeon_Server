// Fill out your copyright notice in the Description page of Project Settings.

#include "PATD_Server.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/Blackboard/BlackboardKeyAllTypes.h"
#include "PATD_Server/Actors/PD_E_Character.h"
#include "PD_T_Attack.h"

EBTNodeResult::Type UPD_T_Attack::ExecuteTask(UBehaviorTreeComponent & OwnerComp, uint8 * NodeMemory)
{
	int ap = OwnerComp.GetBlackboardComponent()->GetValueAsInt("AP");
	if (ap>0)
	{
		bool arePlayersNear = OwnerComp.GetBlackboardComponent()->GetValueAsBool("ArePlayersNear");
		if (arePlayersNear) 
		{
			int direccionAttack = OwnerComp.GetBlackboardComponent()->GetValueAsInt("direccionAttack");
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
			}

			return EBTNodeResult::Succeeded;
		}
		else
			return EBTNodeResult::Failed;
	}
	else
		return EBTNodeResult::Failed;
}