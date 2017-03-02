// Fill out your copyright notice in the Description page of Project Settings.

#include "PATD_Server.h"
#include "Runtime/AIModule/Classes/BehaviorTree/BlackboardComponent.h"
#include "Runtime/AIModule/Classes/BehaviorTree/BehaviorTree.h"
#include "PATD_Server/GM_Game/PD_GM_MapManager.h"
#include "PATD_Server/Structs/PD_NetStructs.h"
#include "PATD_Server/MapGeneration/PD_MG_LogicPosition.h"
#include "PD_EnemyController.h"

APD_EnemyController::APD_EnemyController() {


}

APD_EnemyController::APD_EnemyController(const FString type) {
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

bool APD_EnemyController::MoveTo(float x, float y)
{
	FVector newPosition = FVector(x, y, 0);

	MoveToLocation(newPosition, -1.0f, true, false, false, true, 0, true);

	return true;
}

bool APD_EnemyController::ActionTo(float x, float y, uint8 id_action)
{
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, "AL ATAQUERRRRRR !!!!!");

	return true;

}

bool APD_EnemyController::Animate(uint8 typeAnimation)
{
	return true;
}

void APD_EnemyController::StartTurn(PD_GM_MapManager* refMap, PD_MG_LogicPosition inCurrentPos){
	

	this->mapMng = refMap;
	this->currentPos = inCurrentPos;
	/*Esto de aqui se llama al empezar el turno de cada enemigo
			
	*/
	
	/*blackboardData->*/
	///RunBehaviorTree(behaviorTree);///poner el tree en marcha
	///Blackboard->InitializeBlackboard(*blackboardData);///Inicializar la Black board
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

	/*Blackboard.TargetKeyID = BlackboardComponent->GetKeyID("Target");
	Blackboard.LastSensedLocationID = BlackboardComponent->GetKeyID("LastSensedLocation");
	Blackboard.NextWeaponID = BlackboardComponent->GetKeyID("NextWeapon");
	Blackboard.NextWeaponLocationID = BlackboardComponent->GetKeyID("NextWeaponLocation");

	Blackboard.ShouldProcessCommandsID = BlackboardComponent->GetKeyID("ShouldProcessCommands");
	Blackboard.CommandTypeID = BlackboardComponent->GetKeyID("CommandType");
	Blackboard.CommandActorID = BlackboardComponent->GetKeyID("CommandActor");
	Blackboard.CommandLocationID = BlackboardComponent->GetKeyID("CommandLocation");
	*/
	//BehaviourComponent->StartTree(*AICharacter->AIBehavior);


	//http://blog.spissa.com/2014/10/23/introduccion-a-la-ia-en-ue4-variante-en-c/

	///Ahí muestra como se hace las funciones, para que luego se le llamen desde la behaviorTree y como se actualizan valores en la blackboard.
	///Basicamente hay que hacer que se puedan llamar desde blueprints
}

bool APD_EnemyController::AreEnemiesNear() {
	///dentro de este seteariamos variables de la black board, para indicarlo al decorator del behavior tree
	///Creo que tiene que heredar de AIcontroller
	
}
