// Fill out your copyright notice in the Description page of Project Settings.

#include "PATD_Server.h"
#include "PATD_MG_LogicPosition.h"

PATD_MG_LogicPosition::PATD_MG_LogicPosition(uint32 x, uint32 y)
{
	_x = x;
	_y = y;
}

PATD_MG_LogicPosition::~PATD_MG_LogicPosition()
{
}



uint32 PATD_MG_LogicPosition::GetTypeHash(const PATD_MG_LogicPosition& logpos)
{
	return 8;
}

