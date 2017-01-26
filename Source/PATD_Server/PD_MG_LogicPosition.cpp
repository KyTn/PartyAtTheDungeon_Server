// Fill out your copyright notice in the Description page of Project Settings.

#include "PD_Server.h"
#include "PD_MG_LogicPosition.h"

PD_MG_LogicPosition::PD_MG_LogicPosition(uint32 x, uint32 y)
{
	_x = x;
	_y = y;
}

PD_MG_LogicPosition::~PD_MG_LogicPosition()
{
}



uint32 PD_MG_LogicPosition::GetTypeHash(const PD_MG_LogicPosition& logpos)
{
	return 8;
}

