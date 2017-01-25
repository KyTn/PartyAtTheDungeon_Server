// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

/// CLASS REFERENCES
class PATD_MG_Room;
class PATD_SERVER_API PATD_MG_StaticMap
{
private:
	TArray<PATD_MG_Room>* _rooms;

public:
	PATD_MG_StaticMap();
	~PATD_MG_StaticMap();

	bool AddNewEmptyRoom();
	bool AddRoom(PATD_MG_Room* newRoom);

};
