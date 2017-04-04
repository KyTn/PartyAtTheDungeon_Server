// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "../Actors/PD_SplineActors.h"
#include "PD_GM_SplineManager.generated.h"

UCLASS()
class PATD_SERVER_API APD_GM_SplineManager : public AActor
{
	GENERATED_BODY()
	
public:	

	//Variables
	int initialSplines = 20;
	TArray<APD_SplineActors*> splinesPool;

	//Funciones - Manejar la Pool de Splines
	APD_SplineActors* GetSpline(); //Devuelve un Spline disponible, si no crea uno nuevo y devuelve este.

	// Sets default values for this actor's properties
	APD_GM_SplineManager();

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	// Called every frame
	virtual void Tick( float DeltaSeconds ) override;

	
	
};
