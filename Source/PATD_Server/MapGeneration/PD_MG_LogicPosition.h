// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "PATD_Server.h"
/**
 * 
 */
class PATD_SERVER_API PD_MG_LogicPosition
{
private:
	uint32 _x;
	uint32 _y;

public:

	PD_MG_LogicPosition(uint32 x, uint32 y);
	~PD_MG_LogicPosition();

	uint32 GetX() { return _x; }
	void SetX(uint32 x) { _x = x; }
	uint32 GetY() { return _y; }
	void SetY(uint32 y) { _y = y; }
	
	FORCEINLINE bool operator==(const PD_MG_LogicPosition& other) const
	{
		return this->_x == other._x && this->_y == other._y;
	}
	
	friend uint32 PD_MG_LogicPosition::GetTypeHash(const PD_MG_LogicPosition& logpos)
	{
		return (logpos._x << 16) + logpos._y;
	}


	static FVector* LogicToWorldPosition(PD_MG_LogicPosition* pos) {
		return new FVector(-1.0f * pos->GetX()*100.0f, pos->GetY() * 100.0f, 0.f);
	}

	FVector* ToWorldPosition() {
		return LogicToWorldPosition(this);
	}

	TArray<PD_MG_LogicPosition> GetAdjacents(TArray<PD_MG_LogicPosition> list) {
		
		TArray<PD_MG_LogicPosition> res;
		
		for (int i = 0; i < list.Num(); i++) {
			if ((list[i].GetX() - this->GetX()) <= 1 || (list[i].GetY() - this->GetY()) <= 1) {
				res.Add(list[i]);
			}

		}
		
		return res;
	}
};
