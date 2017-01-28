// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "PD_MG_LogicPosition.h"
/// CLASS REFERENCES

class PD_MG_Tile;
//class PATD_MG_LogicPosition;

class PATD_SERVER_API PD_MG_Room
{
private:
	//TMap<PATD_MG_LogicPosition, PATD_MG_Tile*> _tiles;
	TArray<PD_MG_Tile*> _tiles;

public:
	PD_MG_Room();
	~PD_MG_Room();



};
