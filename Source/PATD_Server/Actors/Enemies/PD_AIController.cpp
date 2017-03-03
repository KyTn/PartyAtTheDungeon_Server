// Fill out your copyright notice in the Description page of Project Settings.

#include "PATD_Server.h"
#include "Runtime/AIModule/Classes/BehaviorTree/BlackboardComponent.h"
#include "Runtime/AIModule/Classes/BehaviorTree/BehaviorTree.h"
#include "PATD_Server/GM_Game/PD_GM_MapManager.h"
#include "PATD_Server/Structs/PD_NetStructs.h"
#include "PATD_Server/MapGeneration/PD_MG_LogicPosition.h"
#include "PD_AIController.h"

APD_AIController::APD_AIController() {


}

APD_AIController::APD_AIController(const FString type) {
	Super();

	//TCHAR* bbChar = new TCHAR();
	///Crear el tchar con el nombre añadido para llamar a la blackboard y al behavior tree correspondiente
	//"BlackboardData'/Game/BluePrints/BlackBoard/" + type.GetCharArray() + "." + type.GetCharArray() + "'";
	//static ConstructorHelpers::FObjectFinder<UBlackboardData> bb(TEXT("BlackboardData'/Game/BluePrints/BT_Building.BT_Building'"));///
	//static ConstructorHelpers::FObjectFinder<UBehaviorTree> bt(TEXT("BehaviorTree'/Game/BluePrints/BT_Building.BT_Building'"));///
	//Blackboard = CreateDefaultSubobject<UBlackboardComponent>("Blackboard");
	//blackboardData = bb.Object;
	//behaviorTree = bt.Object;
}


void APD_AIController::StartTurn(PD_GM_MapManager* refMap, PD_MG_LogicPosition inCurrentPos) {


	this->mapMng = refMap;
	this->currentPos = inCurrentPos;
	/*Esto de aqui se llama al empezar el turno de cada enemigo

	*/

	/*blackboardData->*/
	///RunBehaviorTree(behaviorTree);///poner el tree en marcha
	///Blackboard->InitializeBlackboard(*blackboardData);///Inicializar la Black board

	///Blackboard->SetValueAsFloat("AP", 5);
	///Blackboard->SetValueAsFloat("ArePlayersNear", false);


	/*if (!Blackboard->InitializeBlackboard(*blackboardData))
	{
	GEngine->AddOnScreenDebugMessage(-1, 1, FColor::Red, "Could not load Blackboard");
	return;
	}
	Blackboard->SetValueAsFloat("AttackCooldown", 0.0f);

	AInvadersActor* npc = Cast<AInvadersActor>(GetPawn());
	if (npc)
	{
	Blackboard->SetValueAsObject("SelfActor", GetPawn());
	if (!RunBehaviorTree(behaviorTree))
	GEngine->AddOnScreenDebugMessage(-1, 1, FColor::Red, "Could not run behavior");
	else
	GEngine->AddOnScreenDebugMessage(-1, 1, FColor::Green, "Run Behavior Tree");
	}*/

	///Esto es por si falla, pone en algunos sitios que apartir de la 4.11 no va bien sin esto, ademas a lo mejor debemos pasarle el Acharacter para que lo posea
	//BlackboardComponent->InitializeBlackboard(*AICharacter->AIBehavior->BlackboardAsset);
	//BehaviourComponent->StartTree(*AICharacter->AIBehavior);


	//http://blog.spissa.com/2014/10/23/introduccion-a-la-ia-en-ue4-variante-en-c/

	///Ahí muestra como se hace las funciones, para que luego se le llamen desde la behaviorTree y como se actualizan valores en la blackboard.
	///Basicamente hay que hacer que se puedan llamar desde blueprints
}

void APD_AIController::ArePlayersNear() {
	///dentro de este seteariamos variables de la black board, para indicarlo al decorator del behavior tree
	///Creo que tiene que heredar de AIcontroller
	if (mapMng->IsTherePlayer(currentPos.GetX() + 1, currentPos.GetY())) {
		//Blackboard->SetValueAsFloat("ArePlayersNear", true);
		ActionPos.SetX(currentPos.GetX() + 1);
		ActionPos.SetY(currentPos.GetY());
	}
	else if (mapMng->IsTherePlayer(currentPos.GetX() - 1, currentPos.GetY())) {
		//Blackboard->SetValueAsFloat("ArePlayersNear", true);
		ActionPos.SetX(currentPos.GetX() - 1);
		ActionPos.SetY(currentPos.GetY());
	}
	else if (mapMng->IsTherePlayer(currentPos.GetX(), currentPos.GetY() + 1)) {
		//Blackboard->SetValueAsFloat("ArePlayersNear", true);
		ActionPos.SetX(currentPos.GetX());
		ActionPos.SetY(currentPos.GetY() + 1);
	}
	else if (mapMng->IsTherePlayer(currentPos.GetX(), currentPos.GetY() - 1)) {
		//Blackboard->SetValueAsFloat("ArePlayersNear", true);
		ActionPos.SetX(currentPos.GetX());
		ActionPos.SetY(currentPos.GetY() - 1);
	}
	else
		;//Blackboard->SetValueAsFloat("ArePlayersNear", false);	
}


void APD_AIController::AddAttack() {
	//uint32 AP = Blackboard->GetValueAsFloat("AP");
	//AP--;
	//Blackboard->SetValueAsFloat("AP", AP);
	//Crear accion ataque, y añadirsela a la lista de acciones con ActionPos
}

void APD_AIController::AddMove() {
	//uint32 AP = Blackboard->GetValueAsFloat("AP");
	//AP--;
	//Blackboard->SetValueAsFloat("AP", AP);
	//Crear accion mover(aleatorio), y añadirsela a la lista de acciones con ActionPos
}

