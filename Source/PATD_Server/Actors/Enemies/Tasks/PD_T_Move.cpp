// Fill out your copyright notice in the Description page of Project Settings.

#include "PATD_Server.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/Blackboard/BlackboardKeyAllTypes.h"
#include "PATD_Server/Actors/Enemies/PD_AIController.h"
#include "PD_T_Move.h"



EBTNodeResult::Type UPD_T_Move::ExecuteTask(UBehaviorTreeComponent & OwnerComp, uint8 * NodeMemory) {

	int ap = OwnerComp.GetBlackboardComponent()->GetValueAsInt("AP");
	if (ap > 0)
	{
		//recorrerse el array de jugadores y hacer el pathfinding a sus logicposition, si hay alguno dentro de rango, ap-1 (para poder atacar) nos movemos a su lado, y sino...
		//true areplayersnear, y rellenar con la direccion
		APD_AIController* AIController = (APD_AIController*)OwnerComp.GetAIOwner();
		OwnerComp.GetBlackboardComponent()->SetValueAsBool("Moved", true);
		return EBTNodeResult::Succeeded;
	}
	else
		return EBTNodeResult::Failed;
}
