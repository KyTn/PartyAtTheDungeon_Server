// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Actors/Interactuables/PD_E_Interactuable.h"
#include "PD_E_Door.generated.h"

/**
 * 
 */
UCLASS()
class PATD_SERVER_API APD_E_Door : public APD_E_Interactuable
{
	GENERATED_BODY()
public: 
		APD_E_Door();

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// Called every frame
	virtual void Tick(float DeltaSeconds) override;

	// Se llamará a esta funcion para activar el interactuable
	virtual void InteractToActivate(AActor* interactor, bool overwriteState = false);

	// Se llamará a esta función para desactivar el interactuable
	virtual void InteractToDeactivate(AActor* interactor, bool overwriteState = false);
	
};
