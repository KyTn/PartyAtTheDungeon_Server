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
	APD_AIController* AIController = (APD_AIController*)OwnerComp.GetAIOwner();

	int ap = OwnerComp.GetBlackboardComponent()->GetValueAsInt("AP");
	
		UE_LOG(LogTemp, Log, TEXT("PD_T_Move:: task move empezando .ap:%d idEnemy:%s"), ap, *AIController->GetLogicCharacter()->GetIDCharacter());
	
	//UE_LOG(LogTemp, Log, TEXT("PD_T_Move:: ap:%d"),ap);
	if (ap > 0)
	{
		UE_LOG(LogTemp, Log, TEXT("PD_T_Move:: task move con .ap%d idEnemy:%s"), ap,*AIController->GetLogicCharacter()->GetIDCharacter());
		//recorrerse el array de jugadores y hacer el pathfinding a sus logicposition, si hay alguno dentro de rango, ap-1 (para poder atacar) nos movemos a su lado, y sino...
		
		//true areplayersnear, y rellenar con la direccion
		TArray<PD_MG_LogicPosition> minim, aux;
		
		UPD_ServerGameInstance* SGI = Cast<UPD_ServerGameInstance>(AIController->GetGameInstance());
		TArray<StructPlayer*> playerList = SGI->playersManager->GetDataPlayers();
		for (StructPlayer* playerStruct : playerList) {
			aux = AIController->GetPathFinder()->getPathFromTo(AIController->GetLogicCharacter()->GetCurrentLogicalPosition(),playerStruct->logic_Character->GetCurrentLogicalPosition());
			if ((minim.Num()==0 || aux.Num()<minim.Num()) && aux.Num()!=0 ) {
				minim.Empty();
				minim = aux;
				aux.Empty();
				OwnerComp.GetBlackboardComponent()->SetValueAsString("idCharacterAttacked", playerStruct->logic_Character->GetIDCharacter());
			}
		}
		minim.RemoveAt(minim.Num()-1);
		if (ap > minim.Num()&& minim.Num()>0) {
			UE_LOG(LogTemp, Log, TEXT("PD_T_Move::ExecuteTask: Enemigo va a moverse con pathfinder .ap:%d idEnemy:%s"), ap, *AIController->GetLogicCharacter()->GetIDCharacter());
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
		else {
			UE_LOG(LogTemp, Log, TEXT("PD_T_Move:: Enemigo no se mueve por que esta lejos .ap:%d idEnemy:%s"), ap, *AIController->GetLogicCharacter()->GetIDCharacter());
			OwnerComp.GetBlackboardComponent()->SetValueAsInt("AP", 0);
			//aleatorio
		}
		UE_LOG(LogTemp, Log, TEXT("PD_T_Move:: Setting Moved true .ap:%d idEnemy:%s"), ap, *AIController->GetLogicCharacter()->GetIDCharacter());
		OwnerComp.GetBlackboardComponent()->SetValueAsBool("Moved", true);
		return EBTNodeResult::Succeeded;
	}
	else
		UE_LOG(LogTemp, Log, TEXT("PD_T_Move:: Terminando por no .ap:%d idEnemy:%s"), ap, *AIController->GetLogicCharacter()->GetIDCharacter());
		return EBTNodeResult::Failed;
}
