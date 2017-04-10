// Fill out your copyright notice in the Description page of Project Settings.

#include "PATD_Server.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/Blackboard/BlackboardKeyAllTypes.h"
#include "PATD_Server/Actors/Enemies/PD_AIController.h"
#include "PD_T_Move.h"
#include "GM_Game/PD_GM_MapManager.h"
#include "PD_ServerGameInstance.h"
#include "PD_PlayersManager.h"
#include "GM_Game/Pathfinder/PD_GM_Pathfinder.h"
#include "MapGeneration/PD_MG_LogicPosition.h"
#include "Structs/PD_ServerStructs.h"

EBTNodeResult::Type UPD_T_Move::ExecuteTask(UBehaviorTreeComponent & OwnerComp, uint8 * NodeMemory) {

	int ap = OwnerComp.GetBlackboardComponent()->GetValueAsInt("AP");
	if (ap > 0)
	{
		//recorrerse el array de jugadores y hacer el pathfinding a sus logicposition, si hay alguno dentro de rango, ap-1 (para poder atacar) nos movemos a su lado, y sino...
		
		//true areplayersnear, y rellenar con la direccion
		TArray<PD_MG_LogicPosition> minim, aux;
		APD_AIController* AIController = (APD_AIController*)OwnerComp.GetAIOwner();
		UPD_ServerGameInstance* SGI = Cast<UPD_ServerGameInstance>(AIController->GetGameInstance());
		TArray<StructPlayer*> playerList = SGI->playersManager->GetDataPlayers();
		for (StructPlayer* playerStruct : playerList) {
			aux = AIController->GetPathFinder()->getPathFromTo(AIController->GetLogicCharacter()->GetCurrentLogicalPosition(),playerStruct->logic_Character->GetCurrentLogicalPosition());
			if ((minim.Num()==0 || aux.Num()<minim.Num()) && aux.Num()!=0) {
				minim.Empty();
				minim = aux;
				aux.Empty();
				OwnerComp.GetBlackboardComponent()->SetValueAsString("idCharacterAttacked", playerStruct->logic_Character->GetIDCharacter());
			}
		}
		minim.RemoveAt(minim.Num()-1);
		if (ap > minim.Num()) {

			FStructTurnOrders* turnStruct = AIController->GetTurnOrders();

			for (PD_MG_LogicPosition logicPos : minim)
			{
				FStructLogicPosition logicPositionStruct = FStructLogicPosition();

				logicPositionStruct.positionX = logicPos.GetX();
				logicPositionStruct.positionY = logicPos.GetY();
			
				turnStruct->positionsToMove.Add(logicPositionStruct);
			}
			OwnerComp.GetBlackboardComponent()->SetValueAsInt("AP", ap-minim.Num());
		}
		else
			;//aleatorio
		OwnerComp.GetBlackboardComponent()->SetValueAsBool("Moved", true);
		return EBTNodeResult::Succeeded;
	}
	else
		return EBTNodeResult::Failed;
}
