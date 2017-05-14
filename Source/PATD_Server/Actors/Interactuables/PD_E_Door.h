// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Actors/Interactuables/PD_E_Interactuable.h"
#include "MapGeneration/PD_MG_LogicPosition.h"
#include "MapInfo/PD_MM_MapInfo.h"
#include "PD_E_Door.generated.h"


class PD_MM_Room;


/**
 * 
 */
UCLASS()
class PATD_SERVER_API APD_E_Door : public APD_E_Interactuable
{
	GENERATED_BODY()
public: 
		APD_E_Door();

		bool IsDoorOpen;
		PD_MG_LogicPosition LP_A, LP_B;
		PD_MM_Room* Room_A, Room_B;


	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// Called every frame
	virtual void Tick(float DeltaSeconds) override;

	// Se llamar� a esta funcion para activar el interactuable
	virtual void InteractToActivate(AActor* interactor, bool overwriteState = false);

	// Se llamar� a esta funci�n para desactivar el interactuable
	virtual void InteractToDeactivate(AActor* interactor, bool overwriteState = false);
	
};