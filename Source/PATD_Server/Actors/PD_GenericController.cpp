// Fill out your copyright notice in the Description page of Project Settings.

#include "PATD_Server.h"
#include "PD_GenericController.h"

//Includes de uso
#include "PD_ServerGameInstance.h"
#include "GM_Game/PD_GM_GameManager.h"


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
	
	if (animationOn != EAnimationType::Iddle)
	{
	//Log invasivo con cada tick en animacion!
	//	UE_LOG(LogTemp, Log, TEXT("APD_GenericController::Tick: StateAnim:%d  : currentTimeAnimation: %f : posicion: %s"), static_cast<uint8>(animationOn), currentTimeAnimation, *(GetPawn()->GetActorLocation().ToString()));
		if (animationOn == EAnimationType::Attack) {
			
			currentTimeAnimation += DeltaTime;
			if (currentTimeAnimation > maxLengthAnimation) {
				//currentTimeAnimation -= maxLengthAnimation;
				currentTimeAnimation = 0;
				UE_LOG(LogTemp, Log, TEXT("APD_GenericController::Tick: Finalizando animacion de ataque correctamente."));
				OnAnimationEnd();

			}
			
		}else if (animationOn == EAnimationType::Move) {
			if (FVector::PointsAreNear(moveTargetPosition, GetPawn()->GetActorLocation(), toleranceMove)) {
				UE_LOG(LogTemp, Log, TEXT("APD_GenericController::Tick: Finalizando animacion de movimiento correctamente."));

				OnAnimationEnd();
			}
		
			currentTimeAnimation += DeltaTime;
			if (currentTimeAnimation > maxLengthAnimation) {
				//currentTimeAnimation -= maxLengthAnimation;
				currentTimeAnimation = 0;
				UE_LOG(LogTemp, Warning, TEXT("APD_GenericController::Tick: Finalizando animacion de movimiento. El character no logro llegar a su destino!"));

				OnAnimationEnd();

			}
		}
	}

}

void APD_GenericController::OnAnimationEnd() {
	UE_LOG(LogTemp, Log, TEXT("APD_GenericController::OnAnimationEnd"));
	UPD_ServerGameInstance* SGI = Cast<UPD_ServerGameInstance>(GetGameInstance());
	animationOn = EAnimationType::Iddle;
	SGI->getGameManager()->OnAnimationEnd();
	
}
bool APD_GenericController::IsAtAnimation() {
	UE_LOG(LogTemp, Log, TEXT("APD_GenericController::IsAtAnimation"));
	if (animationOn == EAnimationType::Iddle) return false;
	else return true;
}

bool APD_GenericController::MoveTo(float x, float y)
{
	UE_LOG(LogTemp, Log, TEXT("APD_GenericController::MoveTo"));
	moveTargetPosition.X = x;
	moveTargetPosition.Y = y;
	moveTargetPosition.Z = GetPawn()->GetActorLocation().Z; //Todo en el mismo plano
	animationOn = EAnimationType::Move;

	UE_LOG(LogTemp, Log, TEXT("APD_GenericController::MoveTo: StateAnim:%d"), static_cast<uint8>(animationOn));
	return true;
}

bool APD_GenericController::ActionTo(float x, float y, uint8 id_action)
{
	UE_LOG(LogTemp, Log, TEXT("APD_GenericController::ActionTo"));

	animationOn = EAnimationType::Attack;
	UE_LOG(LogTemp, Log, TEXT("APD_GenericController::ActionTo: StateAnim:%d"), static_cast<uint8>(animationOn));
	return true;

}

bool APD_GenericController::Animate(uint8 typeAnimation)
{
	return true;
}

