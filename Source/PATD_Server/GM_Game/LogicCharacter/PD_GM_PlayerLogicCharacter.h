// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
//Include de herencia (interfaz)
#include "PD_GM_iLogicCharacter.h"

/**
 * 
 */
class PATD_SERVER_API PD_GM_PlayerLogicCharacter : public PD_GM_iLogicCharacter
{
public:
	PD_GM_PlayerLogicCharacter();
	~PD_GM_PlayerLogicCharacter();
	//Hereda de iLogicCharacter
};
