// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

/// CLASS REFERENCES
class PD_MG_Room;
class PD_MG_LogicPosition;


class PATD_SERVER_API PD_MG_StaticMap
{
private:
	TArray<PD_MG_Room*>* _rooms;
	TArray<PD_MG_LogicPosition*>* _LogicPositionsRefs;

public:
	PD_MG_StaticMap();
	~PD_MG_StaticMap();

	bool AddNewLogicPosition(uint32 x, uint32 y);
	bool AddLogicPosition(PD_MG_LogicPosition* ref);

	bool AddNewEmptyRoom();
	bool AddRoom(PD_MG_Room* newRoom);

};
