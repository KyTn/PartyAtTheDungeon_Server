// Fill out your copyright notice in the Description page of Project Settings.

#include "PATD_Server.h"
#include "PATD_Server/MapGeneration/PD_MG_LogicPosition.h"



PD_MG_LogicPosition::PD_MG_LogicPosition() {
	_x = 0;
	_y = 0;
}
PD_MG_LogicPosition::PD_MG_LogicPosition(uint32 x, uint32 y)
{
	_x = x;
	_y = y;
}


PD_MG_LogicPosition::PD_MG_LogicPosition(uint16 pos)
{
	_x = (uint32)((pos >> 8) & 0x00FF);
	_y = (uint32)((pos) & 0x00FF);
}

PD_MG_LogicPosition::~PD_MG_LogicPosition()
{
}


