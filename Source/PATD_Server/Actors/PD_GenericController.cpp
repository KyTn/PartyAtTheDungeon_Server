// Fill out your copyright notice in the Description page of Project Settings.

#include "PATD_Server.h"
#include "PD_GenericController.h"

//Includes de uso
#include "PD_ServerGameInstance.h"
#include "GM_Game/PD_GM_GameManager.h"
#include "Animation/AnimInstance.h"
#include "GM_Game/PD_GM_EnemyManager.h"
#include "PD_PlayersManager.h"
#include "GM_Game/LogicCharacter/PD_GM_LogicCharacter.h"
#include "Runtime/Engine/Classes/Kismet/KismetMathLibrary.h"
#include "GM_Game/PD_GM_GameManager.h"
#include"Actors/PD_E_Character.h"
#include "GM_Game/LogicCharacter/PD_GM_LogicCharacter.h"
#include "GM_Game/PD_GM_SplineManager.h"
//Includes Forward
#include "PD_SplineActors.h"

// Sets default values


/*
APD_GenericController::APD_GenericController(FString type) {

}*/

void APD_GenericController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime); // Call parent class tick function  
	
	if (CallbackAnimationEndToGM)
	{
		//Llamar a GameManager para indicar que se ha acabado la funcion
		UPD_ServerGameInstance* SGI = Cast<UPD_ServerGameInstance>(GetGameInstance());
		SGI->getGameManager()->OnAnimationEnd();
		CallbackAnimationEndToGM = false;
	}


	//Control del final de movimiento (el movimiento es la unica animacion que se controla en el controller y no en la maquina de estados del BP)
	if (isMoving)
	{
		//UE_LOG(LogTemp, Log, TEXT("APD_GenericController::Se va a mover IsMOving %s"), *GetCharacter()->GetName());

		//FTimerHandle handleForPong;
		//GetWorldTimerManager().SetTimer(handleForPong, this, &APD_GenericController::MoveWithSpline, 2.00f, false);
		MoveWithSpline(DeltaTime);
		
	}
	
}

void APD_GenericController::OnAnimationEnd() {
	UE_LOG(LogTemp, Log, TEXT("APD_GenericController::OnAnimationEnd"));
	CallbackAnimationEndToGM = true;
	//UPD_ServerGameInstance* SGI = Cast<UPD_ServerGameInstance>(GetGameInstance());
	//SGI->getGameManager()->OnAnimationEnd();

	
}
bool APD_GenericController::IsAtAnimation() {
	UE_LOG(LogTemp, Log, TEXT("APD_GenericController::IsAtAnimation")); 
	//Comprobamos el estado de la maquina de estados y si esta isMoving=true
	UAnimInstance* AnimInst = GetCharacter()->GetMesh()->GetAnimInstance();
	int stateMachineIndex = -1;
	if (AnimInst) {
		AnimInst->GetStateMachineIndexAndDescription(FName(*animStateMachineName), stateMachineIndex, nullptr);
		//UE_LOG(LogTemp, Log, TEXT("APD_GenericController::IsAtAnimation: stateMachineIndex: %d"), stateMachineIndex);
		FName currentState = AnimInst->GetCurrentStateName(stateMachineIndex);

		UE_LOG(LogTemp, Log, TEXT("APD_GenericController::IsAtAnimation antes de comprobar: currentState:%s  isMoving:%d "), *currentState.ToString(), isMoving);

		if (currentState.ToString().Compare(idleStateName,ESearchCase::IgnoreCase) == 0 && !isMoving) 
			return false;
		else 
			return true;
	}
	else {
		UE_LOG(LogTemp, Log, TEXT("APD_GenericController::IsAtAnimation: ERROR: No hay AnimInstance para este character"));
		UPD_ServerGameInstance* SGI = Cast<UPD_ServerGameInstance>(GetGameInstance());
		if (SGI->gameManager->structGamePhase->enumGamePhase == EServerPhase::MoveTick) { //Si esta en la fase de movimiento, devuelve si esta movimiendo o no
			return isMoving;
		}
		return false; //si no tiene animInst no deberia poder estar haciendo animaciones
	}
	//return true;
}

bool APD_GenericController::MoveTo(float x, float y)
{
	UE_LOG(LogTemp, Log, TEXT("APD_GenericController::MoveTo %s"), *GetCharacter()->GetName());
	
	isMoving = true;

	
	return true;
}

bool APD_GenericController::ActionTo(FStructTargetToAction action)
{
	
	return true;

}

bool APD_GenericController::Animate(uint8 typeAnimation)
{
	return true;
}

/*void APD_GenericController::CallbackAttackEnd() {
	UE_LOG(LogTemp, Log, TEXT("Attack End!!!!!!!!!!"));
	
	OnAnimationEnd();
	///llamar una función del game manager 
}*/


//Funcion para mover al Character mediante Splines
void APD_GenericController::MoveWithSpline(float deltaTime)
{
	//UE_LOG(LogTemp, Log, TEXT("APD_GenericController::MoveWithSpline %s"), *GetCharacter()->GetName());

	FVector lastPosition;
	FVector currentPosition;
	if (spline->GetSplineComponent()->GetNumberOfSplinePoints() != 0) {
	
		lastPosition = spline->GetSplineComponent()->GetLocationAtSplinePoint(spline->GetSplineComponent()->GetNumberOfSplinePoints(), ESplineCoordinateSpace::World);
		currentPosition = GetPawn()->GetActorLocation();

		//UE_LOG(LogTemp, Log, TEXT("APD_GenericController::MoveWithSpline() LastPosition - %s"),*lastPosition.ToString());
		//UE_LOG(LogTemp, Log, TEXT("APD_GenericController::MoveWithSpline() currentPosition - %s"),*currentPosition.ToString());

		//Se comparan las posiciones entre la actual del Character y su posicion final para ver si ha llegado a su destino y tiene que dejar de moverse

		if (!lastPosition.Equals(currentPosition, 15.0)) //Compara con un offset de error, (Por pruebas se ha determinado que 15, pero pueden ser mas o menos)
		{
			//IsCalculatingMovePath = true;
			//Animacion de andar
			UAnimInstance* AnimInst = GetCharacter()->GetMesh()->GetAnimInstance();
			if (AnimInst) {
				UBoolProperty* BoolProperty = FindField<UBoolProperty>(AnimInst->GetClass(), "MoveChar");
				if (BoolProperty != NULL) {
					bool enable = BoolProperty->GetPropertyValue_InContainer(AnimInst);
					BoolProperty->SetPropertyValue_InContainer(AnimInst, true);
					enable = BoolProperty->GetPropertyValue_InContainer(AnimInst);
				}
			}
			else {
				OnAnimationEnd();
			}


			//Simulamos la fisica para que se mueva solo la CABESA!
			FName boneName = "cabesa";
			bool newSimulate = true;
			bool includeSelf = true;
			//GetCharacter()->GetMesh()->SetAllBodiesBelowSimulatePhysics(boneName, newSimulate, includeSelf);
			//GetCharacter()->GetMesh()->AddForceToAllBodiesBelow(FVector(100.0f, 0.0f, 0.0f), boneName, false, true);

			//GetPawn()->GetMovementComponent()->Velocity	= FVector(100.0f, 100.0f, 10.0f);

			GetPawn()->AddMovementInput(GetActorForwardVector(), 0.0, false); //add entrada de movimiento

			GetPawn()->SetActorLocation(spline->GetSplineComponent()->GetLocationAtDistanceAlongSpline(distance, ESplineCoordinateSpace::World)); //Actualizamos la posicion del character

			Cast<APD_E_Character>(GetPawn())->logic_character->SetCurrentLogicalPositionFromFVector(GetPawn()->GetActorLocation()); //Actualizamos la posicion logical del character
			 
			GetPawn()->SetActorRotation(spline->GetSplineComponent()->GetRotationAtDistanceAlongSpline(distance, ESplineCoordinateSpace::World)); //Actualizamos la rotacion del character

			distance = distance + velocity*deltaTime; //actualizamos el variable

		}
		else
		{
			FName boneName = "cabesa";
			bool newSimulate = false;
			bool includeSelf = true;
			//GetCharacter()->GetMesh()->SetAllBodiesBelowSimulatePhysics(boneName, newSimulate, includeSelf);
			//Setear la velocidad a 0, para que deje de moverse en la animacion y vuelva al estado IDLE
			//GetPawn()->GetMovementComponent()->Velocity = FVector(0.0f, 0.0f, 0.0f);
			isMoving = false;
			distance = 0;
			OnAnimationEnd();
			IsCalculatingMovePath = false;
			UAnimInstance* AnimInst = GetCharacter()->GetMesh()->GetAnimInstance();
			if (AnimInst) {
				UBoolProperty* BoolProperty = FindField<UBoolProperty>(AnimInst->GetClass(), "BackToIdle");
				if (BoolProperty != NULL) {
					bool enable = BoolProperty->GetPropertyValue_InContainer(AnimInst);
					BoolProperty->SetPropertyValue_InContainer(AnimInst, true);
					enable = BoolProperty->GetPropertyValue_InContainer(AnimInst);
				}
			}
			else {
				OnAnimationEnd();
				IsCalculatingMovePath = false;
			}

			PD_GM_LogicCharacter* logic_character = nullptr;
			logic_character = Cast<APD_E_Character>(GetCharacter())->GetLogicCharacter();
			if (logic_character)
			{
				if (logic_character->GetIsStoppingByCollision())
				{
					Animation_GetHurt((int)ActiveSkills::GetStunnedByCollision);
				}
			}
		}
	}
	else {
		UE_LOG(LogTemp, Log, TEXT("APD_GenericController::MoveWithSpline:Error moviendose con 0 puntos en la spline %s"),*this->GetName());
		IsCalculatingMovePath = false;
		OnAnimationEnd();
		return;
	}
}

void APD_GenericController::StopMoving()
{
	UE_LOG(LogTemp, Log, TEXT("APD_GenericController::StopMoving:"));

	isMoving = false;
	distance = 0;
	OnAnimationEnd();
	UAnimInstance* AnimInst = GetCharacter()->GetMesh()->GetAnimInstance();
	if (AnimInst) {
		UBoolProperty* BoolProperty = FindField<UBoolProperty>(AnimInst->GetClass(), "BackToIdle");
		if (BoolProperty != NULL) {
			bool enable = BoolProperty->GetPropertyValue_InContainer(AnimInst);
			BoolProperty->SetPropertyValue_InContainer(AnimInst, true);
			enable = BoolProperty->GetPropertyValue_InContainer(AnimInst);
		}
	}
	else {
		OnAnimationEnd();
		IsCalculatingMovePath = false;
	}
}

void APD_GenericController::UpdateRotationCharacterToEnemy(FVector target)
{
	FVector locationCharacter = GetCharacter()->GetActorLocation();
	FRotator newRotationTo = UKismetMathLibrary::FindLookAtRotation(locationCharacter, target);

	GetCharacter()->SetActorRotation(newRotationTo);
}



APD_SplineActors* APD_GenericController::GetSpline()
{
	if (spline)
	{
		return spline;
	}
	else //Si no existe, pide uno al Spline Manager ->  devuelve ese
	{
		UPD_ServerGameInstance* SGI = Cast<UPD_ServerGameInstance>(GetCharacter()->GetGameInstance());
		if (SGI)
		{
			SetSpline(SGI->gameManager->splineManager->GetSpline());
			return spline;
		}
		else
			return nullptr;
	}
}

void APD_GenericController::SetSpline(APD_SplineActors* newSpline)
{
	spline = newSpline;
}

/* ====================
ANIMACIONES - FUNCIONES
==================== */

bool APD_GenericController::CheckCanAnimate() {
	if (GetCharacter() && GetCharacter()->GetMesh() && GetCharacter()->GetMesh()->GetAnimInstance()) return true;
	else return false;
}

void APD_GenericController::Animation_BasicAttack(int ID_Skill)
{
	if (CheckCanAnimate()) {
		UAnimInstance* AnimInst = GetCharacter()->GetMesh()->GetAnimInstance();
		if (AnimInst) {
			UBoolProperty* BoolProperty = FindField<UBoolProperty>(AnimInst->GetClass(), "NormalAttack");
			UIntProperty* IntProperty = FindField<UIntProperty>(AnimInst->GetClass(), "IDSKill");
			if (IntProperty != NULL) {
				int typeCharacter = IntProperty->GetPropertyValue_InContainer(AnimInst);
				IntProperty->SetPropertyValue_InContainer(AnimInst, ID_Skill);
				typeCharacter = IntProperty->GetPropertyValue_InContainer(AnimInst);
			}

			if (BoolProperty != NULL) {
				bool enable = BoolProperty->GetPropertyValue_InContainer(AnimInst);
				BoolProperty->SetPropertyValue_InContainer(AnimInst, true);
				enable = BoolProperty->GetPropertyValue_InContainer(AnimInst);

			}
		}
		
	}
	else {
		OnAnimationEnd();
	}
}

void APD_GenericController::Animation_CriticalBasicAttack(int ID_Skill)
{
	if (CheckCanAnimate()) {
		UAnimInstance* AnimInst = GetCharacter()->GetMesh()->GetAnimInstance();
		if (AnimInst) {

			UIntProperty* IntProperty = FindField<UIntProperty>(AnimInst->GetClass(), "IDSKill");
			if (IntProperty != NULL) {
				int typeCharacter = IntProperty->GetPropertyValue_InContainer(AnimInst);
				IntProperty->SetPropertyValue_InContainer(AnimInst, ID_Skill);
				typeCharacter = IntProperty->GetPropertyValue_InContainer(AnimInst);
			}

			UBoolProperty* BoolProperty = FindField<UBoolProperty>(AnimInst->GetClass(), "CriticalAttack");
			if (BoolProperty != NULL) {
				bool enable = BoolProperty->GetPropertyValue_InContainer(AnimInst);
				BoolProperty->SetPropertyValue_InContainer(AnimInst, true);
				enable = BoolProperty->GetPropertyValue_InContainer(AnimInst);

			}
			

		}
	}
	else {
		OnAnimationEnd();
	}
}

void APD_GenericController::SetTypeCharanimation(int typeChar)
{
	UE_LOG(LogTemp, Log, TEXT("APD_GenericController::SetTypeCharanimation: - "));

	if (CheckCanAnimate()) {
		if (GetCharacter())
		{
			UE_LOG(LogTemp, Log, TEXT("APD_GenericController::SetTypeCharanimation: - SI hay character"));

			if (GetCharacter()->GetMesh())
			{
				UE_LOG(LogTemp, Log, TEXT("APD_GenericController::SetTypeCharanimation: - SI hay mesh"));
				UAnimInstance* AnimInst = GetCharacter()->GetMesh()->GetAnimInstance();
				if (AnimInst) {
					UIntProperty* IntProperty = FindField<UIntProperty>(AnimInst->GetClass(), "TipoChar");
					if (IntProperty != NULL) {
						int typeCharacter = IntProperty->GetPropertyValue_InContainer(AnimInst);
						IntProperty->SetPropertyValue_InContainer(AnimInst, typeChar);
						typeCharacter = IntProperty->GetPropertyValue_InContainer(AnimInst);
					}
				}

			}
		}
	}
	else {
		OnAnimationEnd();
	}
}


void APD_GenericController::Animation_UseConsumable(int ID_Skill)
{
	if (CheckCanAnimate()) {
		UAnimInstance* AnimInst = GetCharacter()->GetMesh()->GetAnimInstance();
		if (AnimInst) {
			UBoolProperty* BoolProperty = FindField<UBoolProperty>(AnimInst->GetClass(), "useConsumable");

			UIntProperty* IntProperty = FindField<UIntProperty>(AnimInst->GetClass(), "IDSKill");
			if (IntProperty != NULL) {
				int typeCharacter = IntProperty->GetPropertyValue_InContainer(AnimInst);
				IntProperty->SetPropertyValue_InContainer(AnimInst, ID_Skill);
				typeCharacter = IntProperty->GetPropertyValue_InContainer(AnimInst);
			}

			if (BoolProperty != NULL) {
				bool enable = BoolProperty->GetPropertyValue_InContainer(AnimInst);
				BoolProperty->SetPropertyValue_InContainer(AnimInst, true);
				enable = BoolProperty->GetPropertyValue_InContainer(AnimInst);
			}

		}
	}
	else {
		OnAnimationEnd();
	}
}

void APD_GenericController::Animation_UseInteractable(int ID_Skill)
{
	if (CheckCanAnimate()) {
		UAnimInstance* AnimInst = GetCharacter()->GetMesh()->GetAnimInstance();
		if (AnimInst) {

			UIntProperty* IntProperty = FindField<UIntProperty>(AnimInst->GetClass(), "IDSKill");
			if (IntProperty != NULL) {
				int typeCharacter = IntProperty->GetPropertyValue_InContainer(AnimInst);
				IntProperty->SetPropertyValue_InContainer(AnimInst, ID_Skill);
				typeCharacter = IntProperty->GetPropertyValue_InContainer(AnimInst);
			}

			UBoolProperty* BoolProperty = FindField<UBoolProperty>(AnimInst->GetClass(), "useInteractable");
			if (BoolProperty != NULL) {
				bool enable = BoolProperty->GetPropertyValue_InContainer(AnimInst);
				BoolProperty->SetPropertyValue_InContainer(AnimInst, true);
				enable = BoolProperty->GetPropertyValue_InContainer(AnimInst);

			}

		}
	}
	else {
		OnAnimationEnd();
	}
}

void APD_GenericController::Animation_CastSkill(int ID_Skill)
{
	if (CheckCanAnimate()) {
		UAnimInstance* AnimInst = GetCharacter()->GetMesh()->GetAnimInstance();
		if (AnimInst) {

			UIntProperty* IntProperty = FindField<UIntProperty>(AnimInst->GetClass(), "IDSKill");
			if (IntProperty != NULL) {
				int typeCharacter = IntProperty->GetPropertyValue_InContainer(AnimInst);
				IntProperty->SetPropertyValue_InContainer(AnimInst, ID_Skill);
				typeCharacter = IntProperty->GetPropertyValue_InContainer(AnimInst);
			}

			UBoolProperty* BoolProperty = FindField<UBoolProperty>(AnimInst->GetClass(), "CastSkill");
			if (BoolProperty != NULL) {
				bool enable = BoolProperty->GetPropertyValue_InContainer(AnimInst);
				BoolProperty->SetPropertyValue_InContainer(AnimInst, true);
				enable = BoolProperty->GetPropertyValue_InContainer(AnimInst);
			}

		}
	}
	else {
		OnAnimationEnd();
	}
}

void APD_GenericController::Animation_GetHurt(int ID_Skill)
{
	if (CheckCanAnimate()) {
		UAnimInstance* AnimInst = GetCharacter()->GetMesh()->GetAnimInstance();
		if (AnimInst) {

			UIntProperty* IntProperty = FindField<UIntProperty>(AnimInst->GetClass(), "IDSKill");
			if (IntProperty != NULL) {
				int typeCharacter = IntProperty->GetPropertyValue_InContainer(AnimInst);
				IntProperty->SetPropertyValue_InContainer(AnimInst, ID_Skill);
				typeCharacter = IntProperty->GetPropertyValue_InContainer(AnimInst);
			}

			UBoolProperty* BoolProperty = FindField<UBoolProperty>(AnimInst->GetClass(), "GetHurt");
			if (BoolProperty != NULL) {
				bool enable = BoolProperty->GetPropertyValue_InContainer(AnimInst);
				BoolProperty->SetPropertyValue_InContainer(AnimInst, true);
				enable = BoolProperty->GetPropertyValue_InContainer(AnimInst);

			}
			
		}
	}
	else {
		OnAnimationEnd();
	}
}

void APD_GenericController::Animation_DeathChar(int ID_Skill)
{
	if (CheckCanAnimate()) {

		UAnimInstance* AnimInst = GetCharacter()->GetMesh()->GetAnimInstance();
		if (AnimInst) {

			UIntProperty* IntProperty = FindField<UIntProperty>(AnimInst->GetClass(), "IDSKill");
			if (IntProperty != NULL) {
				int typeCharacter = IntProperty->GetPropertyValue_InContainer(AnimInst);
				IntProperty->SetPropertyValue_InContainer(AnimInst, ID_Skill);
				typeCharacter = IntProperty->GetPropertyValue_InContainer(AnimInst);
			}

			UBoolProperty* BoolProperty = FindField<UBoolProperty>(AnimInst->GetClass(), "DeathChar");
			if (BoolProperty != NULL) {
				bool enable = BoolProperty->GetPropertyValue_InContainer(AnimInst);
				BoolProperty->SetPropertyValue_InContainer(AnimInst, true);
				enable = BoolProperty->GetPropertyValue_InContainer(AnimInst);

			}
			
		}
	}
	else {
		OnAnimationEnd();
	}
}

void APD_GenericController::Animation_DefenseChar(int ID_Skill)
{
	if (CheckCanAnimate()) {

		UAnimInstance* AnimInst = GetCharacter()->GetMesh()->GetAnimInstance();
		if (AnimInst) {

			UIntProperty* IntProperty = FindField<UIntProperty>(AnimInst->GetClass(), "IDSKill");
			if (IntProperty != NULL) {
				int typeCharacter = IntProperty->GetPropertyValue_InContainer(AnimInst);
				IntProperty->SetPropertyValue_InContainer(AnimInst, ID_Skill);
				typeCharacter = IntProperty->GetPropertyValue_InContainer(AnimInst);
			}

			UBoolProperty* BoolProperty = FindField<UBoolProperty>(AnimInst->GetClass(), "DefenseChar");
			if (BoolProperty != NULL) {
				bool enable = BoolProperty->GetPropertyValue_InContainer(AnimInst);
				BoolProperty->SetPropertyValue_InContainer(AnimInst, true);
				enable = BoolProperty->GetPropertyValue_InContainer(AnimInst);

			}

		}
	}
	else {
		OnAnimationEnd();
	}
}

void APD_GenericController::Animation_Idle()
{
	if (CheckCanAnimate()) {
		UAnimInstance* AnimInst = GetCharacter()->GetMesh()->GetAnimInstance();
		if (AnimInst) {
			UBoolProperty* BoolProperty = FindField<UBoolProperty>(AnimInst->GetClass(), "BackToIdle");
			if (BoolProperty != NULL) {
				bool enable = BoolProperty->GetPropertyValue_InContainer(AnimInst);
				BoolProperty->SetPropertyValue_InContainer(AnimInst, true);
				enable = BoolProperty->GetPropertyValue_InContainer(AnimInst);
			}
		}
		else {
			OnAnimationEnd();
			IsCalculatingMovePath = false;
		}
	}
	else {
		OnAnimationEnd();
		IsCalculatingMovePath = false;
	}
}
