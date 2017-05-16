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

//Includes Forward
#include "PD_SplineActors.h"

// Sets default values

APD_GenericController::APD_GenericController()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

APD_GenericController::APD_GenericController(FString type) {

}

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
		
		//FTimerHandle handleForPong;
		//GetWorldTimerManager().SetTimer(handleForPong, this, &APD_GenericController::MoveWithSpline, 2.00f, false);
		MoveWithSpline();
		

			/*if (FVector::PointsAreNear(moveTargetPosition, GetPawn()->GetActorLocation(), toleranceMove)) {
				UE_LOG(LogTemp, Log, TEXT("APD_GenericController::Tick: Finalizando animacion de movimiento correctamente."));
				isMoving = false;
				OnAnimationEnd();
			}
		
			currentTimeAnimation += DeltaTime;
			if (currentTimeAnimation > maxLengthAnimation) {
				//currentTimeAnimation -= maxLengthAnimation;
				currentTimeAnimation = 0;
				UE_LOG(LogTemp, Warning, TEXT("APD_GenericController::Tick: Finalizando animacion de movimiento. El character no logro llegar a su destino!"));
				isMoving = false;
				OnAnimationEnd();

			}
			*/
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

		if (currentState.ToString().Compare(idleStateName) == 0 && !isMoving) return false;
		else return true;
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
	UE_LOG(LogTemp, Log, TEXT("APD_GenericController::MoveTo"));
	//moveTargetPosition.X = x;
	//moveTargetPosition.Y = y;
	//moveTargetPosition.Z = GetPawn()->GetActorLocation().Z; //Todo en el mismo plano
	isMoving = true;

	
	return true;
}

bool APD_GenericController::ActionTo(FStructTargetToAction action)
{
	UE_LOG(LogTemp, Log, TEXT("APD_GenericController::ActionTo"));


	PD_GM_EnemyManager* enemyManager = Cast<UPD_ServerGameInstance>(GetGameInstance())->gameManager->enemyManager;
	PD_PlayersManager* playersManager = Cast<UPD_ServerGameInstance>(GetGameInstance())->gameManager->playersManager;


	for (int i = 0; i < action.id_character.Num(); i++) {
		if (enemyManager->GetCharacterByID(action.id_character[i])) {
			enemyManager->GetCharacterByID(action.id_character[i])->UpdateHPCurrent(150);
		}
	}

	for (int i = 0; i < action.id_character.Num(); i++) {
		if (playersManager->GetCharacterByID(action.id_character[i])) {
			playersManager->GetCharacterByID(action.id_character[i])->UpdateHPCurrent(150);
		}
	}



	//Activar enableAttack en el BP de anim
	UAnimInstance* AnimInst = GetCharacter()->GetMesh()->GetAnimInstance();
	if (AnimInst) {
		UBoolProperty* MyFloatProp = FindField<UBoolProperty>(AnimInst->GetClass(), "EnableAttack");
		if (MyFloatProp != NULL) {
			bool FloatVal = MyFloatProp->GetPropertyValue_InContainer(AnimInst);
			MyFloatProp->SetPropertyValue_InContainer(AnimInst, true);
			FloatVal = MyFloatProp->GetPropertyValue_InContainer(AnimInst);
		}
	}
	else {
		OnAnimationEnd();
	}




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
void APD_GenericController::MoveWithSpline()
{




	FVector lastPosition;
	FVector currentPosition;
	if (spline->GetSplineComponent()->GetNumberOfSplinePoints() != 0) {
	
		lastPosition = spline->GetSplineComponent()->GetLocationAtSplinePoint(spline->GetSplineComponent()->GetNumberOfSplinePoints(), ESplineCoordinateSpace::World);
		currentPosition = GetPawn()->GetActorLocation();
	}
	else {
		UE_LOG(LogTemp, Log, TEXT("APD_GenericController::MoveWithSpline:Error moviendose con 0 puntos en la spline %s"),*this->GetName());
		return;
	}

	//UE_LOG(LogTemp, Log, TEXT("APD_GenericController::MoveWithSpline() LastPosition - %s"),*lastPosition.ToString());
	//UE_LOG(LogTemp, Log, TEXT("APD_GenericController::MoveWithSpline() currentPosition - %s"),*currentPosition.ToString());

	//Se comparan las posiciones entre la actual del Character y su posicion final para ver si ha llegado a su destino y tiene que dejar de moverse

	if (!lastPosition.Equals(currentPosition, 15.0)) //Compara con un offset de error, (Por pruebas se ha determinado que 15, pero pueden ser mas o menos)
	{

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

		GetPawn()->SetActorRotation(spline->GetSplineComponent()->GetRotationAtDistanceAlongSpline(distance, ESplineCoordinateSpace::World)); //Actualizamos la rotacion del character

		distance = distance + 2; //actualizamos el variable

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
		}

	}

}

APD_SplineActors* APD_GenericController::GetSpline()
{
	if (spline)
		return spline;
	else
		return nullptr;
}

void APD_GenericController::SetSpline(APD_SplineActors* newSpline)
{
	spline = newSpline;
}

void APD_GenericController::Animation_BasicAttack()
{
	UAnimInstance* AnimInst = GetCharacter()->GetMesh()->GetAnimInstance();
	if (AnimInst) {
		UBoolProperty* BoolProperty = FindField<UBoolProperty>(AnimInst->GetClass(), "NormalAttack");
		if (BoolProperty != NULL) {
			bool enable = BoolProperty->GetPropertyValue_InContainer(AnimInst);
			BoolProperty->SetPropertyValue_InContainer(AnimInst, true);
			enable = BoolProperty->GetPropertyValue_InContainer(AnimInst);
		}
	}
	else {
		OnAnimationEnd();
	}

}
void APD_GenericController::Animation_CriticalBasicAttack()
{
	UAnimInstance* AnimInst = GetCharacter()->GetMesh()->GetAnimInstance();
	if (AnimInst) {
		UBoolProperty* BoolProperty = FindField<UBoolProperty>(AnimInst->GetClass(), "CriticalAttack");
		if (BoolProperty != NULL) {
			bool enable = BoolProperty->GetPropertyValue_InContainer(AnimInst);
			BoolProperty->SetPropertyValue_InContainer(AnimInst, true);
			enable = BoolProperty->GetPropertyValue_InContainer(AnimInst);
		}
	}
	else {
		OnAnimationEnd();
	}

}