// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

class PD_MG_LogicPosition;
class PD_MG_StaticMap;
class PD_MG_DynamicMap;
class PD_GM_iLogicCharacter;
/**
 * 
 */
class PATD_SERVER_API PD_GM_MapManager
{
public:
	PD_GM_MapManager();
	~PD_GM_MapManager();

	PD_MG_StaticMap* StaticMapRef;
	PD_MG_DynamicMap* DynamicMapRef;

	PD_GM_iLogicCharacter* getLogicCharacterAt(PD_MG_LogicPosition* pos);
	
	//Esto iria aqui, o si hacemos otra clase para la visualizacion, en esa clase.
	FVector getUWorldPosition(PD_MG_LogicPosition* pos);
};
