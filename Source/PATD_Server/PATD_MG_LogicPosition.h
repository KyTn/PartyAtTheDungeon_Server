// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

/**
 * 
 */
class PATD_SERVER_API PATD_MG_LogicPosition
{
private:
	uint32 _x;
	uint32 _y;

public:

	PATD_MG_LogicPosition(uint32 x, uint32 y);
	~PATD_MG_LogicPosition();

	uint32 GetX() { return _x; }
	void SetX(uint32 x) { _x = x; }
	uint32 GetY() { return _y; }
	void SetY(uint32 y) { _y = y; }

	bool operator=(PATD_MG_LogicPosition other) {
		return this->GetX() == other.GetX() && this->GetY() == other.GetY();
	}

	
	uint32 GetTypeHash(const PATD_MG_LogicPosition& logpos);
};
