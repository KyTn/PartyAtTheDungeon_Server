// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Actors/PD_GenericController.h"
#include "PD_CharacterController.generated.h"

/**
 * 
 */
UCLASS()
class PATD_SERVER_API APD_CharacterController : public APD_GenericController
{
public:
	GENERATED_BODY()
	
	virtual bool MoveTo(float x, float y) override;
	virtual bool ActionTo(float x, float y, uint8 id_action) override;
	virtual bool Animate(uint8 typeAnimation) override;
	
	
};
