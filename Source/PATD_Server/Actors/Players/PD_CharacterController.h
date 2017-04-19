// Fill out your copyright notice in the Description page of Project Settings.

#pragma once



//include de unreal
#include "Actors/PD_GenericController.h"
#include "Structs/PD_NetStructs.h"
#include "PD_CharacterController.generated.h"



/**
 * 
 */
UCLASS()
class PATD_SERVER_API APD_CharacterController : public APD_GenericController
{
private:

public:
	
	GENERATED_BODY()
	
	virtual bool MoveTo(float x, float y) override;
	virtual bool ActionTo(FStructTargetToAction action) override;
	virtual bool Animate(uint8 typeAnimation) override;
	

};
