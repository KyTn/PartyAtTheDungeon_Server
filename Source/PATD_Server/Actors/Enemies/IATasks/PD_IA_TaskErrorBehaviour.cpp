// Fill out your copyright notice in the Description page of Project Settings.

#include "PATD_Server.h"
#include "PD_IA_TaskErrorBehaviour.h"

#include "PATD_Server/Actors/Enemies/PD_AIController.h"
#include "GM_Game/LogicCharacter/PD_GM_LogicCharacter.h"
#include "Actors/PD_E_Character.h"

EBTNodeResult::Type UPD_IA_TaskErrorBehaviour::ExecuteTask(UBehaviorTreeComponent & OwnerComp, uint8 * NodeMemory)
{
	APD_AIController* AIController = (APD_AIController*)OwnerComp.GetAIOwner();
	PD_GM_LogicCharacter* logicCharacter = ((APD_E_Character*)AIController->GetPawn())->logic_character;

	UE_LOG(LogTemp, Log, TEXT("UPD_IA_TaskErrorBehaviour:: Entrando en error behaviour %s"), *logicCharacter->GetIDCharacter());
	return EBTNodeResult::Succeeded;
}