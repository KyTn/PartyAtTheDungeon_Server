// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

/// CLASS REFERENCES
class PATD_MG_Room;
class PATD_MG_LogicPosition;


class PATD_SERVER_API PATD_MG_StaticMap
{
private:
	TArray<PATD_MG_Room*>* _rooms;
	TArray<PATD_MG_LogicPosition*>* _LogicPositionsRefs;

public:
	PATD_MG_StaticMap();
	~PATD_MG_StaticMap();

	bool AddNewLogicPosition(uint32 x, uint32 y);
	bool AddLogicPosition(PATD_MG_LogicPosition* ref);

	bool AddNewEmptyRoom();
	bool AddRoom(PATD_MG_Room* newRoom);

};
