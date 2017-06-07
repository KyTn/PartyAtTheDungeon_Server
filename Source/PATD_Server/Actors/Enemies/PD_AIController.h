// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

//#include "PATD_Server/Structs/PD_ServerEnums.h"
#include "Actors/PD_GenericController.h"
#include "MapGeneration/PD_MG_LogicPosition.h"

//#include "Runtime/AIModule/Classes/BehaviorTree/BlackboardComponent.h"
//#include "Runtime/AIModule/Classes/BehaviorTree/BehaviorTree.h"
#include "PD_AIController.generated.h"


class APD_E_Character;
class PD_GM_LogicCharacter;

class PD_GM_MapManager;
class PD_GM_Pathfinder;
/**
*
*/
UCLASS()
class PATD_SERVER_API APD_AIController : public APD_GenericController
{
	GENERATED_BODY()
private:
		PD_GM_MapManager* mapMng;
		FStructTurnOrders actions;
		PD_GM_Pathfinder* pathfinder;


public:
		APD_AIController();

	UPROPERTY(transient)
		UBlackboardComponent* BlackboardComp;
	
	UPROPERTY(transient)
		UBehaviorTreeComponent* BehaviorTreeComp;

	void Possess(APawn* charac) override;

	void StartAITurnCalcultion(PD_GM_MapManager* refMap);
	//Callback desde la taskClear
	void EndAITurnCalculation();


	PD_GM_MapManager* GetMapMng() { return mapMng; };
	FStructTurnOrders* GetTurnOrders() { return &actions; };
	PD_GM_Pathfinder* GetPathFinder() { return pathfinder; };


	TArray<PD_MG_LogicPosition> GetValidPositionsAdyacentsTo(PD_MG_LogicPosition position);

	TArray<PD_GM_LogicCharacter*> GetEnemiesInRange(float range);
//	TArray<PD_GM_LogicCharacter*> GetPlayersInRange();
	PD_GM_LogicCharacter* GetClosestPlayer();
	PD_GM_LogicCharacter* GetLeastHPPlayer();
	PD_GM_LogicCharacter* GetMostHPEnemy();
	PD_MG_LogicPosition GetClosestDoorPosition();
	PD_MG_LogicPosition GetClosestPosition(TArray<PD_MG_LogicPosition> listPosition);


	bool CheckInRangeFromPositionToCharacter(PD_MG_LogicPosition positionFrom, PD_GM_LogicCharacter* character,int range);
	/*
	virtual bool MoveTo(float x, float y) override;
	virtual bool ActionTo(float x, float y, uint8 id_action) override;
	virtual bool Animate(uint8 typeAnimation) override;
	*/

	//Determina el comportamiento a ejecutar. Puede ser calculado en PD_IA_TaskBehaviourSelector
	EIABehaviour selectedBehaviour;

	//Determina si la orden de selectedBehaviour es propia o es impuesta
	bool orderImpossed;

	bool canAction;

	//Cuantos turnos se ha determinado que quedan hasta el goal. Lo calcula en PD_IA_TaskBehaviourSelector? o lo setea quien haga un orderImpossed.
	int turnsForGoal=0;
	//las calculamos en el PD_IA_TaskBehaviourSelector y en los comportamientos que setean a minions
	//Usado por Flee
	PD_MG_LogicPosition goalPosition;
	//Usado por Attack, Defense, Berserker 
	PD_GM_LogicCharacter* goalCharacter;
//Definen que goals han sido seteados.
	bool usePosition;
	bool useCharacter;
	//Funciones para setear
	void SetGoalPosition(PD_MG_LogicPosition inGoalPosition);
	void SetGoalCharacter(PD_GM_LogicCharacter* inGoalCharacter);
	void SetGoalCharacterAndPosition(PD_GM_LogicCharacter* inGoalCharacter, PD_MG_LogicPosition inGoalPosition);
	//Elemento interactuable goal
	//Se calculan en el TargetCalc de cada task y alimentan el CreateOrder
	
	//Elemento interactuable turnTarget
	PD_GM_LogicCharacter* turnTargetCharacter;


	PD_MG_LogicPosition turnTargetPosition;
	//Usado por Attack
	//Indica cuantos ataques va a hacer este turno. 0 si no hace ninguno. 
	int turnNumAttacks;



};
