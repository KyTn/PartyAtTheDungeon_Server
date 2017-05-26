// Fill out your copyright notice in the Description page of Project Settings.

#include "PATD_Server.h"
#include "PD_E_Interactuable.h"
#include "Actors/PD_E_ElementActor.h"


// Sets default values
APD_E_Interactuable::APD_E_Interactuable()
{
	//Super::APD_E_ElementActor();
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void APD_E_Interactuable::BeginPlay()
{
	Super::BeginPlay();

}

// Called every frame
void APD_E_Interactuable::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}


// Se llamar� a esta funcion para activar el interactuable
void APD_E_Interactuable::InteractToActivate(AActor* interactor, bool overwriteState) {}

// Se llamar� a esta funci�n para desactivar el interactuable
void APD_E_Interactuable::InteractToDeactivate(AActor* interactor, bool overwriteState) {}


