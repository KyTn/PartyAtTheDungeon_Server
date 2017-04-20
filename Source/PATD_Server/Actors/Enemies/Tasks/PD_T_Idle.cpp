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
	APD_AIController* AIController = (APD_AIController*)OwnerComp.GetAIOwner();

	int ap = OwnerComp.GetBlackboardComponent()->GetValueAsInt("AP");
	bool moved = OwnerComp.GetBlackboardComponent()->GetValueAsBool("Moved");
//	UE_LOG(LogTemp, Log, TEXT("PD_T_Idle:: ap:%d moved:%d"),ap,moved);
	if (ap == 0) //al ser una secuencia y esta el primer task, al devolver falso entrará en bucle aquí
	{
	//	UE_LOG(LogTemp, Log, TEXT("PD_T_Idle:: Entrando en ap==0"));
		if (moved)
		{
			UE_LOG(LogTemp, Log, TEXT("PD_T_Idle:: ap:%d moved:%d  idEnemy:%s"), ap, moved, *AIController->GetLogicCharacter()->GetIDCharacter());
			UE_LOG(LogTemp, Log, TEXT("PD_T_Idle:: Entrando en EndAITurnCalculation idEnemy:%s"), *AIController->GetLogicCharacter()->GetIDCharacter());
			OwnerComp.GetBlackboardComponent()->SetValueAsBool("Moved", false);
			AIController->EndAITurnCalculation();
		}
		return EBTNodeResult::Failed;
	}
	else {
		UE_LOG(LogTemp, Log, TEXT("PD_T_Idle:: comenzando IA ap:%d moved:%d  idEnemy:%s"), ap, moved, *AIController->GetLogicCharacter()->GetIDCharacter()); 
		return EBTNodeResult::Succeeded;
	}
}