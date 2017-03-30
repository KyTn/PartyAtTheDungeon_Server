// Fill out your copyright notice in the Description page of Project Settings.

#include "PATD_Server.h"
#include "PD_GenericController.h"

//Includes de uso
#include "PD_ServerGameInstance.h"
#include "GM_Game/PD_GM_GameManager.h"
#include "Animation/AnimInstance.h"

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
	
	//Control del final de movimiento (el movimiento es la unica animacion que se controla en el controller y no en la maquina de estados del BP)
	if (isMoving)
	{
		
			if (FVector::PointsAreNear(moveTargetPosition, GetPawn()->GetActorLocation(), toleranceMove)) {
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
	}
	
}

void APD_GenericController::OnAnimationEnd() {
	UE_LOG(LogTemp, Log, TEXT("APD_GenericController::OnAnimationEnd"));
	UPD_ServerGameInstance* SGI = Cast<UPD_ServerGameInstance>(GetGameInstance());
	SGI->getGameManager()->OnAnimationEnd();
	
}
bool APD_GenericController::IsAtAnimation() {
	UE_LOG(LogTemp, Log, TEXT("APD_GenericController::IsAtAnimation")); 
	//Comprobamos el estado de la maquina de estados y si esta isMoving=true
	UAnimInstance* AnimInst = GetCharacter()->GetMesh()->GetAnimInstance();
	int stateMachineIndex = -1;
		AnimInst->GetStateMachineIndexAndDescription(FName(*animStateMachineName), stateMachineIndex,nullptr);
	FName currentState= AnimInst->GetCurrentStateName(stateMachineIndex);

	

	UE_LOG(LogTemp, Log, TEXT("APD_GenericController::IsAtAnimation antes de comprobar: currentState:%s  isMoving:%d "), *currentState.ToString(), isMoving);
	
	if (currentState.ToString().Compare(idleStateName)==0 && !isMoving) return false;
	else return true;
}

bool APD_GenericController::MoveTo(float x, float y)
{
	UE_LOG(LogTemp, Log, TEXT("APD_GenericController::MoveTo"));
	moveTargetPosition.X = x;
	moveTargetPosition.Y = y;
	moveTargetPosition.Z = GetPawn()->GetActorLocation().Z; //Todo en el mismo plano
	isMoving = true;

	
	return true;
}

bool APD_GenericController::ActionTo(float x, float y, uint8 id_action)
{
	UE_LOG(LogTemp, Log, TEXT("APD_GenericController::ActionTo"));
	
	//Activar enableAttack en el BP de anim
	UAnimInstance* AnimInst = GetCharacter()->GetMesh()->GetAnimInstance();
	UBoolProperty* MyFloatProp = FindField<UBoolProperty>(AnimInst->GetClass(), "EnableAttack");
	if (MyFloatProp != NULL) {
		bool FloatVal = MyFloatProp->GetPropertyValue_InContainer(AnimInst);
		MyFloatProp->SetPropertyValue_InContainer(AnimInst, true);
		FloatVal = MyFloatProp->GetPropertyValue_InContainer(AnimInst);
	}
	;
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