// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

/// CLASS REFERENCES

class PATD_MG_StaticMap;
class PATD_MG_Room;
class PATD_MG_LogicPosition;


class PATD_SERVER_API PATD_MG_Tile
{
private:
	PATD_MG_StaticMap* _SMap;
	PATD_MG_Room* _room;
	PATD_MG_Tile* _neigthbours[3][3];
	PATD_MG_LogicPosition* _LogicPos;


public:
	PATD_MG_Tile();
	~PATD_MG_Tile();
};
