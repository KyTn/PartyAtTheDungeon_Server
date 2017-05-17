// Fill out your copyright notice in the Description page of Project Settings.

#pragma once


//forward declarations
#include "Structs/PD_ServerEnums.h"
#include "Structs/PD_NetStructs.h"
class APD_SplineActors;

//include de unreal
#include "AIController.h"
#include "Components/SplineComponent.h"
#include "PD_GenericController.generated.h"


/**
 * 
 */
UCLASS()
class PATD_SERVER_API APD_GenericController : public AAIController
{
	//Este nombre es el que tienen que tener las maquinas de estados del BP de anim (animInstance)
	FString animStateMachineName = "BipedoStateMachine";
	//Este es el nombre que tiene que tener el estado de Idle para reconocerlo como tal
	FString idleStateName = "IDLE";

	bool isMoving = false;
	bool CallbackAnimationEndToGM = false;
	int maxLengthAnimation = 5;
	float currentTimeAnimation=0;
	FVector moveTargetPosition;
	float toleranceMove = 50.0;

	//For Spline
	APD_SplineActors*  spline = nullptr;
	float distance = 0.0f;

public:
	GENERATED_BODY()

	APD_GenericController();

	APD_GenericController(FString type);

	void SetSpline(APD_SplineActors* newSpline);
	APD_SplineActors* GetSpline();


	//Funcion para moverse por el mundo con splines
	void MoveWithSpline();

	//Funcion para actualizar la rotacion del personaje
	void UpdateRotationCharacterToEnemy(FVector target);

	//Usado para el control del timing de las animaciones (Decidir cuando ha acabado de moverse por ejemplo)
	void Tick(float DeltaTime)override;

	UFUNCTION(BlueprintCallable, Category = PD_Animation)
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
	- FStructTargetToAction action: toda la info necesaria para resolver el ataque
	Devuelve true si se ha completado con exito, de otra forma false
	*/
	virtual bool ActionTo(FStructTargetToAction action);

	void Animation_BasicAttack();
	void Animation_CriticalBasicAttack();
	void Animation_UseConsumable();
	void Animation_UseInteractable();
	void Animation_CastSkill();
	void Animation_GetHurt();
	void Animation_DeathChar();
	void Animation_DefenseChar();
	
	void SetTypeCharanimation(int typeChar);
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
	
	//	UFUNCTION(BlueprintCallable, Category = PD_Animation)
	//	void CallbackAttackEnd();
	
};
