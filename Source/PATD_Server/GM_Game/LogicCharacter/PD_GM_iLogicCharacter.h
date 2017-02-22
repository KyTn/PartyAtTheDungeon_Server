// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "Structs/PD_ServerStructs.h"
/**
 * 
 */


class PATD_SERVER_API PD_GM_iLogicCharacter
{
public:
	PD_GM_iLogicCharacter();
	~PD_GM_iLogicCharacter();

//	PD_MG_LogicPosition* position;

	//Aqui irian funciones comunes a todos los characters, como moverse atacar y demas
	//Pero para poder calcular el daño, el rango, etc, serian funciones virutales, 
	//pues los enemigos y los personajes lo calculan de diferente manera (unos llevan armas y dependera de esta y otros no, es fijo)

	//Tambien guardaremos punteros a las cosas que nos interesen, como el actor fisico que lo represente.

	void ProcessMoveOrder(FStructOrderAction order) {
		if (EOrderAction(order.orderType) == EOrderAction::Move) {
			//Actualizar logicPosition.
			//Pero para saber si invadimos la casilla de otro objeto, necesitamos una referencia al mapa, y que en el mapa se guarde
			//por posiciones logicas, que characters hay (solo uno en principio, pero puede que necesitemos varios).
			//Siendo asi, al moverse tendria que actualizar las referencias en el mapa aparte de su propia logicPosition.
		}
	};

	//CheckAndChangeLogicPosition() {

	//}

	//ResolveConflicts() {

	//}
};
