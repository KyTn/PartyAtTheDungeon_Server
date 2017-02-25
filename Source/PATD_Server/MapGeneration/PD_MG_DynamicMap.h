#pragma once
// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "PD_MG_LogicPosition.h"
#include "PATD_Server/Actors/Enemies/PD_E_EnemiesEnum.h"
/// CLASS REFERENCES
class PD_MG_Room;
class PD_MG_LogicPosition;
class PD_GM_IALogicCharacter;
class PD_E_EnemiesEnum;
class PATD_SERVER_API PD_MG_DynamicMap
{
private:
	
	uint32 _Height, _Width;
	//TMap< PD_MG_LogicPosition, EEnemiesType> posEnemies;
	TMap<PD_MG_LogicPosition, EEnemiesType> _xymap;
	TMap<PD_MG_LogicPosition, TArray<PD_GM_IALogicCharacter*>> enemies;
	TArray<PD_MG_LogicPosition*> _LogicPositionsRefs;

public:
	PD_MG_DynamicMap();
	~PD_MG_DynamicMap();

#pragma region GET_SET

	uint32 GetHeight() { return _Height; }
	uint32 GetWidth() { return _Width; }

	void SetHeight(uint32 val) { _Height = val; }
	void SetWidth(uint32 val) { _Width = val; }

	TMap<PD_MG_LogicPosition, TArray<PD_GM_IALogicCharacter*>> getEnemies() { return enemies; }
	TMap<PD_MG_LogicPosition, EEnemiesType> GetXYMap() { return _xymap; }
	TArray<PD_MG_LogicPosition*> GetLogicPositions() { return _LogicPositionsRefs; }

#pragma endregion

	//PD_MG_LogicPosition* AddNewLogicPosition(uint32 x, uint32 y, TCHAR c);

	void AddNewEnemy(uint32 x, uint32 y, PD_GM_IALogicCharacter* ch, EEnemiesType type);

	void UpdateActor(AActor* actor, PD_MG_LogicPosition* lp);

	bool Clear();
};
