// Fill out your copyright notice in the Description page of Project Settings.

#include "PATD_Server.h"
#include "PATD_Server/GM_Game/PD_GM_MapManager.h"
#include "PATD_Server/Structs/PD_NetStructs.h"
#include "PATD_Server/MapGeneration/PD_MG_LogicPosition.h"
#include "PATD_Server/Actors/PD_E_Character.h"
#include "PATD_Server/GM_Game/LogicCharacter/PD_GM_LogicCharacter.h"
#include <math.h>
#include "MyAIClass.h"

MyAIClass::MyAIClass()
{
}

MyAIClass::~MyAIClass()
{
}

void MyAIClass::HStartTurnZombie(PD_GM_MapManager * refMap, PD_MG_LogicPosition pos)
{

	//UE_LOG(LogTemp, Error, TEXT("Posición inicial enemigo: %f %f"), pos.GetX(), pos.GetY());
	actions = new FStructTurnOrders();
	mapMng = refMap;
	currentPos = pos;
	aviablePos.Init(true, 4);
	AP = 5;
	PlayersNear = false;
	Attacked = false;
	while (AP > 0) {
		if (!Attacked) {
			if (PlayersNear) {
				HAddAttack();
			}
			else
				AviablePosition();
				HAddMove();
		}
		else
			HAddMove();
	}
}

void MyAIClass::HArePlayersNear()
{
	if (mapMng->IsTherePlayer(currentPos.GetX(), currentPos.GetY() + 1)) {
		PlayersNear = true;
		ActionPos.SetX(currentPos.GetX());
		ActionPos.SetY(currentPos.GetY() + 1);
		aviablePos[0] = false;
	}
	else if (mapMng->IsTherePlayer(currentPos.GetX(), currentPos.GetY() - 1)) {
		PlayersNear = true;
		ActionPos.SetX(currentPos.GetX());
		ActionPos.SetY(currentPos.GetY() - 1);
		aviablePos[1] = false;
	}
	else if (mapMng->IsTherePlayer(currentPos.GetX() - 1, currentPos.GetY())) {
		PlayersNear = true;
		ActionPos.SetX(currentPos.GetX() - 1);
		ActionPos.SetY(currentPos.GetY());
		aviablePos[2] = false;
	}
	else if (mapMng->IsTherePlayer(currentPos.GetX() + 1, currentPos.GetY())) {
		PlayersNear = true;
		ActionPos.SetX(currentPos.GetX() + 1);
		ActionPos.SetY(currentPos.GetY());
		aviablePos[3] = false;
	}
	else
		PlayersNear = false;
}

void MyAIClass::AviablePosition() {
	if (mapMng->IsThereWall(currentPos.GetX(), currentPos.GetY() + 1)) {
		aviablePos[0] = false;
	}
	else if (mapMng->IsThereWall(currentPos.GetX(), currentPos.GetY() - 1)) {
		aviablePos[1] = false;
	}
	else if (mapMng->IsThereWall(currentPos.GetX() - 1, currentPos.GetY())) {
		aviablePos[2] = false;
	}
	else if (mapMng->IsThereWall(currentPos.GetX() + 1, currentPos.GetY())) {
		aviablePos[3] = false;
	}
}

void MyAIClass::HAddAttack()
{
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
//	actions->.Add(attack);
	AP--;
	Attacked = true;
	//UE_LOG(LogTemp, Error, TEXT("Añade ataque"));
}

void MyAIClass::HAddMove()
{
	FStructOrderAction move;
	move.orderType = 1;
	uint8 direc;
	do {
		direc = FMath::RandRange(0, 3);
	} while (!aviablePos[direc]);
	switch (direc) {
		case 0 :
			move.targetLogicPosition.positionX = currentPos.GetX();
			move.targetLogicPosition.positionY = currentPos.GetY() + 1;
			break;
		case 1 :
			move.targetLogicPosition.positionX = currentPos.GetX();
			move.targetLogicPosition.positionY = currentPos.GetY() - 1;
			break;
		case 2:
			move.targetLogicPosition.positionX = currentPos.GetX() - 1;
			move.targetLogicPosition.positionY = currentPos.GetY();
			break;
		case 3:
			move.targetLogicPosition.positionX = currentPos.GetX() + 1;
			move.targetLogicPosition.positionY = currentPos.GetY();
			break;
	}
	//Actualizamos la posición del enemigo

	//UE_LOG(LogTemp, Error, TEXT("Posición enemigo: %f %f"), currentPos.GetX(), currentPos.GetY());
	//UE_LOG(LogTemp, Error, TEXT("Movimiento enemigo: %f %f"), move.targetLogicPosition.positionX, move.targetLogicPosition.positionY);

	currentPos.SetX(move.targetLogicPosition.positionX);
	currentPos.SetY(move.targetLogicPosition.positionY);


	///actions->listMove.Add(move);
	AP--;
	//UE_LOG(LogTemp, Error, TEXT("Añade movimiento"));
}
