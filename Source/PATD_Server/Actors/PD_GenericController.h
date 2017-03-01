// Fill out your copyright notice in the Description page of Project Settings.

#pragma once


//forward declarations
#include "Structs/PD_ServerEnums.h"

//include de unreal
#include "AIController.h"
#include "PD_GenericController.generated.h"

/**
 * 
 */
UCLASS()
class PATD_SERVER_API APD_GenericController : public AAIController
{


	EAnimationType animationOn;
	int maxLengthAnimation = 30;
	int currentTimeAnimation;
	FVector moveTargetPosition;
	float toleranceMove = 2.0;

public:
	GENERATED_BODY()

	APD_GenericController();

	//Usado para el control del timing de las animaciones (Decidir cuando ha acabado de moverse por ejemplo)
	void Tick(float DeltaTime)override;
	void OnAnimationEnd();
	bool IsAtAnimation();
		/*
		//Para mover al personaje de forma visual por el mapa
		Recibe:
		- La posicion fisica en X e Y
		Devuelve true si se ha completado con exito, de otra forma false
		*/
		virtual bool MoveTo(float x, float y);


	/*
	//Para realizar una accion ya sea habilidad, ataque o interaccion
	Recibe:
	- La posicion fisica en X e Y de donde tiene que lanzar la accion
	- el ID con la accion a realizar
	Devuelve true si se ha completado con exito, de otra forma false
	*/
	virtual bool ActionTo(float x, float y, uint8 id_action);


	/*
	//Sirve para lanzar la animacion determinada del character
	Recibe:
	- La posicion fisica en X e Y
	Devuelve true si se ha completado con exito, de otra forma false
	*/
	virtual bool Animate(uint8 typeAnimation);
	/*
	1. IDLE
	2. Death
	3. Damage
	4. Victory
	5. Consume Item
	6. Cast Spell / Skill
	7. Atack
	8. State N
	etc.
	*/
	
	
	
};
