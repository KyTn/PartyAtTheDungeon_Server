#pragma once
// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "PD_MG_LogicPosition.h"

/// CLASS REFERENCES
class PD_MG_Room;
class PD_MG_LogicPosition;


class PATD_SERVER_API PD_MG_DynamicMap
{
private:
	
	uint32 _Height, _Width;

	TMap<PD_MG_LogicPosition, TCHAR> _xymap;
	TMap<TCHAR, FString> enemies;

	TArray<PD_MG_Room*> _rooms;
	TArray<PD_MG_LogicPosition*> _LogicPositionsRefs;

	PD_MG_LogicPosition* _StartPointLP;

public:
	PD_MG_DynamicMap();
	~PD_MG_DynamicMap();

#pragma region GET_SET

	uint32 GetHeight() { return _Height; }
	uint32 GetWidth() { return _Width; }

	void SetHeight(uint32 val) { _Height = val; }
	void SetWidth(uint32 val) { _Width = val; }

	TMap<PD_MG_LogicPosition, TCHAR> GetXYMap() { return _xymap; }
	TArray<PD_MG_LogicPosition*> GetLogicPositions() { return _LogicPositionsRefs; }

#pragma endregion

	PD_MG_LogicPosition* AddNewLogicPosition(uint32 x, uint32 y, TCHAR c);

	void AddEnemyDictionary(TCHAR key, FString value);

	FString GetEnemy(TCHAR key);

	bool IsEnemy(TCHAR key);

	bool Clear();
};
