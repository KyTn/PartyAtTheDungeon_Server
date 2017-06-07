// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Actors/PD_E_ElementActor.h"
#include "PD_E_WallProp.generated.h"

/**
 * 
 */
UCLASS()
class PATD_SERVER_API APD_E_WallProp : public APD_E_ElementActor
{
	GENERATED_BODY()

public:

	APD_E_WallProp();
	
	

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// Called every frame
	virtual void Tick(float DeltaSeconds) override;

	
	void SearchWallAndFix(PD_MG_LogicPosition lp);



};
