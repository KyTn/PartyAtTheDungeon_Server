// Fill out your copyright notice in the Description page of Project Settings.

#include "PATD_Server.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/Blackboard/BlackboardKeyAllTypes.h"
#include "PATD_Server/Actors/Enemies/PD_AIController.h"
#include "PD_T_Idle.h"

EBTNodeResult::Type UPD_T_Idle::ExecuteTask(UBehaviorTreeComponent & OwnerComp, uint8 * NodeMemory)
{
	int ap = OwnerComp.GetBlackboardComponent()->GetValueAsInt("AP");
	bool moved = OwnerComp.GetBlackboardComponent()->GetValueAsBool("Moved");
	if (ap == 0) //al ser una secuencia y esta el primer task, al devolver falso entrará en bucle aquí
	{
		if (moved)
		{
			OwnerComp.GetBlackboardComponent()->SetValueAsBool("Moved", false);
			APD_AIController* AIController = (APD_AIController*)OwnerComp.GetAIOwner();
			//llamar al callback
		}
		return EBTNodeResult::Failed;
	}
	else
		return EBTNodeResult::Succeeded;
}