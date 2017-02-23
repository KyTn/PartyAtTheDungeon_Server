// Fill out your copyright notice in the Description page of Project Settings.

#include "PATD_Server.h"
#include "PD_GM_iLogicCharacter.h"

//Includes del forward declaration 
#include "Structs/PD_ServerStructs.h" //Para todos los structs.
#include "MapGeneration/PD_MG_LogicPosition.h"

PD_GM_iLogicCharacter::PD_GM_iLogicCharacter()
{
}

PD_GM_iLogicCharacter::~PD_GM_iLogicCharacter()
{
}


PD_MG_LogicPosition* PD_GM_iLogicCharacter::getLogicPosition() {
	return position;
}

void PD_GM_iLogicCharacter::ProcessMoveOrder(FStructOrderAction order) {
	if (EOrderAction(order.orderType) == EOrderAction::Move) {
		//Actualizar logicPosition.
		//Pero para saber si invadimos la casilla de otro objeto, necesitamos una referencia al mapa, y que en el mapa se guarde
		//por posiciones logicas, que characters hay (solo uno en principio, pero puede que necesitemos varios).
		//Siendo asi, al moverse tendria que actualizar las referencias en el mapa aparte de su propia logicPosition.
	}
};