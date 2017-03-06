// Fill out your copyright notice in the Description page of Project Settings.

#include "PATD_Server.h"
//#include "Runtime/AIModule/Classes/BehaviorTree/BlackboardComponent.h"
//#include "Runtime/AIModule/Classes/BehaviorTree/BehaviorTree.h"
#include "PATD_Server/GM_Game/PD_GM_MapManager.h"
#include "PATD_Server/Structs/PD_NetStructs.h"
#include "PATD_Server/MapGeneration/PD_MG_LogicPosition.h"
#include "PATD_Server/Actors/PD_E_Character.h"
#include "PATD_Server/GM_Game/LogicCharacter/PD_GM_LogicCharacter.h"
#include "PD_AIController.h"
#include <math.h>

APD_AIController::APD_AIController(){// : AAIController() {

	/*static ConstructorHelpers::FObjectFinder<UBehaviorTree> TreeBlueprint(TEXT("BehaviorTree'/Game/BluePrints/Enemies/BehaviorTree/Zombie_Tree.Zombie_Tree'"));
	if (TreeBlueprint.Object) {
		behaviorTree = TreeBlueprint.Object;
	}
	static ConstructorHelpers::FObjectFinder<UBlackboardData> BbBlueprint(TEXT("BlackboardData'/Game/BluePrints/Enemies/Blackboard/Zombie_Blackboard.Zombie_Blackboard'"));
	if (BbBlueprint.Object) {
		blackboardData = BbBlueprint.Object;
	}

	//behaviorTree = Cast<UBehaviorTree>(StaticLoadObject(UBehaviorTree::StaticClass(), nullptr, TEXT("Blueprint'/Game/BluePrints/Enemies/BehaviorTree/Zombie_Tree.Zombie_Tree'")));
	//blackboardData = Cast<UBlackboardData>(StaticLoadObject(UBlackboardData::StaticClass(), nullptr, TEXT("Blueprint'/Game/BluePrints/Enemies/BehaviorTree/Zombie_Blackboard.Zombie_Blackboard'")));
	Blackboard = CreateDefaultSubobject<UBlackboardComponent>("Blackboard");
	behaviorTreeComp = CreateDefaultSubobject<UBehaviorTreeComponent>("behaviorTree");*/
}

/*APD_AIController::APD_AIController(const FString type) {/// el type tiene que ser un fstring con el nombre del árbol(de hecho, si lo gestionamos por carpetas se podría hacer una minruta)

FString PathTree = "/Game/BluePrints/Enemies/BehaviorTree/" + type + "_tree." + type;
FString PathBB = "/Game/BluePrints/Enemies/Blackboard/" + type + "." + type;
behaviorTree = Cast<UBehaviorTree>(StaticLoadObject(UBehaviorTree::StaticClass(), nullptr, *PathTree));
blackboardData = Cast<UBlackboardData>(StaticLoadObject(UBlackboardData::StaticClass(), nullptr, *PathBB));
Blackboard = CreateDefaultSubobject<UBlackboardComponent>("Blackboard");
}*/

/*void APD_AIController::Possess(APawn* charac)
{
Super::Possess(charac);

ch = Cast<APD_E_Character>(charac);

// start behavior
if (ch && ch->BotBehavior)
{
Blackboard->InitializeBlackboard(*ch->BotBehavior->BlackboardAsset);

// Get the enemy blackboard ID, and store it to access that blackboard key later.


//behaviorTreeComp->StartTree(*ch->BotBehavior);
}
}*/
/*
void APD_AIController::StartTurnZombie(PD_GM_MapManager* refMap, PD_MG_LogicPosition pos) {

	//Blackboard = CreateDefaultSubobject<UBlackboardComponent>("Blackboard");
	//behaviorTreeComp = CreateDefaultSubobject<UBehaviorTreeComponent>("behaviorTree");*/

	//FOutputDeviceNull ar;
	//charac->GetCharacterBP()->CallFunctionByNameWithArguments(TEXT("GetTree"), ar, behaviorTree, true);
	//ch = Cast<APD_E_Character>(charac->GetCharacterBP());///Esto peta, aunque haciendo pruebas peta siempre lo primero que se ejecuta :S
	//blackboardData = ch->BotBehavior->BlackboardAsset;
	//behaviorTree = ch->BotBehavior;
	/*UE_LOG(LogTemp, Error, TEXT("Entra en la IA1"));
	mapMng = refMap;
	currentPos = pos;
	aviablePos.Init(true, 4);
	UE_LOG(LogTemp, Error, TEXT("Entra en la IA2"));
	if (!Blackboard->InitializeBlackboard(*blackboardData))
	{
		GEngine->AddOnScreenDebugMessage(-1, 1, FColor::Red, "Could not load Blackboard");
	}
	Blackboard->SetValueAsFloat("AP", 5);
	Blackboard->SetValueAsFloat("ArePlayersNear", 0);
	Blackboard->SetValueAsFloat("Attacked", 0);
	UE_LOG(LogTemp, Error, TEXT("Entra en la IA3"));
	behaviorTreeComp->StartTree(*behaviorTree);
	UE_LOG(LogTemp, Error, TEXT("Entra en la IA4"));
}

void APD_AIController::ArePlayersNear() {


	if (mapMng->IsTherePlayer(currentPos.GetX(), currentPos.GetY() + 1)) {
		Blackboard->SetValueAsFloat("ArePlayersNear", 1);
		ActionPos.SetX(currentPos.GetX());
		ActionPos.SetY(currentPos.GetY() + 1);
		aviablePos[0] = false;
	}
	else if (mapMng->IsTherePlayer(currentPos.GetX(), currentPos.GetY() - 1)) {
		Blackboard->SetValueAsFloat("ArePlayersNear", 1);
		ActionPos.SetX(currentPos.GetX());
		ActionPos.SetY(currentPos.GetY() - 1);
		aviablePos[1] = false;
	}
	else if (mapMng->IsTherePlayer(currentPos.GetX() - 1, currentPos.GetY())) {
		Blackboard->SetValueAsFloat("ArePlayersNear", 1);
		ActionPos.SetX(currentPos.GetX() - 1);
		ActionPos.SetY(currentPos.GetY());
		aviablePos[2] = false;
	}
	else if (mapMng->IsTherePlayer(currentPos.GetX() + 1, currentPos.GetY())) {
		Blackboard->SetValueAsFloat("ArePlayersNear", 1);
		ActionPos.SetX(currentPos.GetX() + 1);
		ActionPos.SetY(currentPos.GetY());
		aviablePos[3] = false;
	}
	else
		Blackboard->SetValueAsFloat("ArePlayersNear", 0);
}


void APD_AIController::AddAttack() {

	FStructOrderAction attack;
	attack.orderType = 2;
	if (currentPos.GetX() > ActionPos.GetX())
		attack.targetDirection = 3;//atacar izquierda
	else if (currentPos.GetX() < ActionPos.GetX())
		attack.targetDirection = 4;//atacar derecha
	else if (currentPos.GetY() > ActionPos.GetY())
		attack.targetDirection = 2;//atacar abajo
	else if (currentPos.GetX() < ActionPos.GetX())
		attack.targetDirection = 1;//atacar arriba
	actions->listAttack.Add(attack);
	UpdateAP();
	Blackboard->SetValueAsFloat("Attacked", 1);
	UE_LOG(LogTemp, Error, TEXT("Añade movimiento"));
}

void APD_AIController::AddMove() {
	FStructOrderAction move;
	move.orderType = 1;
	uint8 direc;
	do {
		direc = FMath::RandRange(0, 3);
	} while (!aviablePos[direc]);
	move.targetDirection = direc;
	actions->listMove.Add(move);
	UpdateAP();
	UE_LOG(LogTemp, Error, TEXT("Añade ataque"));
}

void APD_AIController::UpdateAP() {
	uint32 AP = Blackboard->GetValueAsFloat("AP");
	AP--;
	Blackboard->SetValueAsFloat("AP", AP);
}

bool APD_AIController::EndBehaviour() {

	uint32 AP = Blackboard->GetValueAsFloat("AP");
	if (AP == 0)
		return true;
	return false;

}*/