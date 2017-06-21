// Fill out your copyright notice in the Description page of Project Settings.

#include "PATD_Server.h"
#include "Actors/PD_E_ElementActor.h"
#include "PD_ServerGameInstance.h"
#include "MapGeneration/PD_MG_LogicPosition.h"
#include "PD_E_WallActor.h"
#include "GM_Game/PD_GM_MapManager.h"
#include "MapInfo/PD_MM_MapInfo.h"

APD_E_WallActor::APD_E_WallActor() {
	adjacentRooms = TArray<PD_MM_Room*>();
}

// Called when the game starts or when spawned
void APD_E_WallActor::BeginPlay()
{
	Super::BeginPlay();
	adjacentRooms = TArray<PD_MM_Room*>();


	for (int i = 0; i < SGI->mapManager->MapInfo->rooms.Num(); i++) {
		if (SGI->mapManager->MapInfo->rooms[i]->LogicWallPosInRoom.Contains(ActualLogicPosition)) {
			adjacentRooms.Add(SGI->mapManager->MapInfo->rooms[i]);
		}
	}

	isActive = false;



	/*
	TArray<PD_MG_LogicPosition> adj_reales = SGI->mapManager->Get_LogicPosition_Adyacents_To(ActualLogicPosition);
	PD_MG_LogicPosition N = PD_MG_LogicPosition(ActualLogicPosition.GetX() - 1, ActualLogicPosition.GetY());
	PD_MG_LogicPosition S = PD_MG_LogicPosition(ActualLogicPosition.GetX() + 1, ActualLogicPosition.GetY());
	PD_MG_LogicPosition E = PD_MG_LogicPosition(ActualLogicPosition.GetX(), ActualLogicPosition.GetY() + 1);
	PD_MG_LogicPosition W = PD_MG_LogicPosition(ActualLogicPosition.GetX(), ActualLogicPosition.GetY() - 1);
	
	if (adj_reales.Contains(N)) {
		adjacentRooms.Add(SGI->mapManager->MapInfo->roomByLogPos[N]);
	}
	if (adj_reales.Contains(S)) {
		adjacentRooms.Add(SGI->mapManager->MapInfo->roomByLogPos[S]);
	}
	if (adj_reales.Contains(W)) {
		adjacentRooms.Add(SGI->mapManager->MapInfo->roomByLogPos[W]);
	}
	if (adj_reales.Contains(E)) {
		adjacentRooms.Add(SGI->mapManager->MapInfo->roomByLogPos[E]);
	}
	*/
	
}

// Called every frame
void APD_E_WallActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);


	// los WallActors no tienen una habitacion asociada, por lo que no entran nunca en el if del tick del padre ... 
	// hacemos un custom aqui

	FOutputDeviceDebug  debug;

	//UE_LOG(LogTemp, Log, TEXT("APD_E_ElementActor::Tick - ActualLogicPosition (%d,%d)"), ActualLogicPosition.GetX(), ActualLogicPosition.GetY());


	
	if (!isActive) {

		for (int i = 0; i < adjacentRooms.Num(); i++) {
			if (adjacentRooms[i]->IsActive) {
				isActive = true;
				break;
			}
		}

		if (isActive) {
			this->CallFunctionByNameWithArguments(TEXT("BP_DEACTIVATE_FOG"), debug, this, true);
		}

	}
	
}



void APD_E_WallActor::SetMaterialSkin(PD_MG_LogicPosition lp) {
	ChangeMaterialProperties_MapSkin(lp);
};


void APD_E_WallActor::ChangeRotationToReal(PD_MG_LogicPosition lp) {

	FOutputDeviceDebug  debug;

	Super::ActualLogicPosition = lp;
	Super::SGI = Cast<UPD_ServerGameInstance>(GetWorld()->GetGameInstance());

	TArray<PD_MG_LogicPosition> adj_reales = Super::SGI->mapManager->Get_LogicPosition_Adyacents_To(Super::ActualLogicPosition);
	PD_MG_LogicPosition N = PD_MG_LogicPosition(Super::ActualLogicPosition.GetX() - 1, Super::ActualLogicPosition.GetY());
	PD_MG_LogicPosition S = PD_MG_LogicPosition(Super::ActualLogicPosition.GetX() + 1, Super::ActualLogicPosition.GetY());
	PD_MG_LogicPosition E = PD_MG_LogicPosition(Super::ActualLogicPosition.GetX(), Super::ActualLogicPosition.GetY() + 1);
	PD_MG_LogicPosition W = PD_MG_LogicPosition(Super::ActualLogicPosition.GetX(), Super::ActualLogicPosition.GetY() - 1);

	UE_LOG(LogTemp, Log, TEXT("APD_E_Door::ChangeRotationToReal - adj_reales %d isTile %d isProp %d && "),
		adj_reales.Contains(N), Super::SGI->mapManager->IsLogicPositionATile(N), Super::SGI->mapManager->IsLogicPositionAProp(N));

	// Tienes una tile en el norte o el sur?
	if ((adj_reales.Contains(N) && (Super::SGI->mapManager->IsLogicPositionATile(N) || Super::SGI->mapManager->IsLogicPositionADoor(N)))
		|| (adj_reales.Contains(S) && (Super::SGI->mapManager->IsLogicPositionATile(S) || Super::SGI->mapManager->IsLogicPositionADoor(S)))
		) {
		this->CallFunctionByNameWithArguments(TEXT("BP_Thin_NS_Visible"), debug, this, true);
	}


	// Tienes una tile en el este o el oeste?
	else if ((adj_reales.Contains(E) && (Super::SGI->mapManager->IsLogicPositionATile(E) || Super::SGI->mapManager->IsLogicPositionADoor(E)))
		|| (adj_reales.Contains(W) && (Super::SGI->mapManager->IsLogicPositionATile(W) || Super::SGI->mapManager->IsLogicPositionADoor(W)))
		) {
		this->CallFunctionByNameWithArguments(TEXT("BP_Thin_WE_Visible"), debug, this, true);
		rotated = true;
	}
}

void APD_E_WallActor::ChangeMaterialProperties_MapSkin(PD_MG_LogicPosition lp) {
	FOutputDeviceDebug debug;
	Super::ActualLogicPosition = lp;
	Super::SGI = Cast<UPD_ServerGameInstance>(GetWorld()->GetGameInstance());
	TArray<PD_MG_LogicPosition> adj_reales = Super::SGI->mapManager->Get_LogicPosition_Diagonals_And_Adyacents_To(Super::ActualLogicPosition);
	PD_MG_LogicPosition N = PD_MG_LogicPosition(Super::ActualLogicPosition.GetX() - 1, Super::ActualLogicPosition.GetY());
	PD_MG_LogicPosition S = PD_MG_LogicPosition(Super::ActualLogicPosition.GetX() + 1, Super::ActualLogicPosition.GetY());
	PD_MG_LogicPosition E = PD_MG_LogicPosition(Super::ActualLogicPosition.GetX(), Super::ActualLogicPosition.GetY() + 1);
	PD_MG_LogicPosition W = PD_MG_LogicPosition(Super::ActualLogicPosition.GetX(), Super::ActualLogicPosition.GetY() - 1);

	PD_MG_LogicPosition NE = PD_MG_LogicPosition(Super::ActualLogicPosition.GetX() - 1, Super::ActualLogicPosition.GetY() + 1);
	PD_MG_LogicPosition SE = PD_MG_LogicPosition(Super::ActualLogicPosition.GetX() + 1, Super::ActualLogicPosition.GetY() + 1);
	PD_MG_LogicPosition NW = PD_MG_LogicPosition(Super::ActualLogicPosition.GetX() - 1, Super::ActualLogicPosition.GetY() - 1);
	PD_MG_LogicPosition SW = PD_MG_LogicPosition(Super::ActualLogicPosition.GetX() + 1, Super::ActualLogicPosition.GetY() - 1);


	UE_LOG(LogTemp, Log, TEXT("APD_E_WallActor::ChangeMaterialProperties_MapSkin - adj_reales %d isTile %d isProp %d && "), adj_reales.Contains(N), Super::SGI->mapManager->IsLogicPositionATile(N), Super::SGI->mapManager->IsLogicPositionAProp(N));


	// visibilidad
	if (isThinWall) {
		if (rotated) 
		{
			this->CallFunctionByNameWithArguments(TEXT("BP_Thin_NS_Visible"), debug, this, true);
		}
		else
		{
			this->CallFunctionByNameWithArguments(TEXT("BP_Thin_WE_Visible"), debug, this, true);
		}
	}
	else 
	{
		this->CallFunctionByNameWithArguments(TEXT("BP_Normal_AllVisible"), debug, this, true);
	}

	// seteando el skin


	// Tienes una tile en el norte ?
	if (adj_reales.Contains(N)) {
		if (Super::SGI->mapManager->IsLogicPositionATile(N) || Super::SGI->mapManager->IsLogicPositionAProp(N)) {
			mapSkin_N = APD_E_ElementActor::SGI->mapManager->MapInfo->roomByLogPos[N]->mapSkin;
		}
		else
		{
			//this->CallFunctionByNameWithArguments(TEXT("BP_N_NotVisible"), debug, this, true);
		}
	}


	// Tienes una tile en el sur?
	if (adj_reales.Contains(S)) {
		if (Super::SGI->mapManager->IsLogicPositionATile(S) || Super::SGI->mapManager->IsLogicPositionAProp(S)) {
			mapSkin_S = APD_E_ElementActor::SGI->mapManager->MapInfo->roomByLogPos[S]->mapSkin;
		}
		else
		{
			//this->CallFunctionByNameWithArguments(TEXT("BP_S_NotVisible"), debug, this, true);
		}
	}

	// Tienes una tile en el este?
	if (adj_reales.Contains(E)) {
		if (Super::SGI->mapManager->IsLogicPositionATile(E) || Super::SGI->mapManager->IsLogicPositionAProp(E)) {
			mapSkin_E = APD_E_ElementActor::SGI->mapManager->MapInfo->roomByLogPos[E]->mapSkin;
		}
		else
		{
			//this->CallFunctionByNameWithArguments(TEXT("BP_E_NotVisible"), debug, this, true);
		}
	}


	// Tienes una tile en el oeste?
	if (adj_reales.Contains(W)) {
		if (Super::SGI->mapManager->IsLogicPositionATile(W) || Super::SGI->mapManager->IsLogicPositionAProp(W)) {
			mapSkin_W = APD_E_ElementActor::SGI->mapManager->MapInfo->roomByLogPos[W]->mapSkin;
		}
		else
		{
			//this->CallFunctionByNameWithArguments(TEXT("BP_W_NotVisible"), debug, this, true);
		}
	}



	if (adj_reales.Contains(NE)) {
		if (Super::SGI->mapManager->IsLogicPositionATile(NE) || Super::SGI->mapManager->IsLogicPositionAProp(NE)) {
			mapSkin_NE = APD_E_ElementActor::SGI->mapManager->MapInfo->roomByLogPos[NE]->mapSkin;
		}
		else
		{
			//this->CallFunctionByNameWithArguments(TEXT("BP_N_NotVisible"), debug, this, true);
		}
	}


	// Tienes una tile en el sur?
	if (adj_reales.Contains(SE)) {
		if (Super::SGI->mapManager->IsLogicPositionATile(SE) || Super::SGI->mapManager->IsLogicPositionAProp(SE)) {
			mapSkin_SE = APD_E_ElementActor::SGI->mapManager->MapInfo->roomByLogPos[SE]->mapSkin;
		}
		else
		{
			//this->CallFunctionByNameWithArguments(TEXT("BP_S_NotVisible"), debug, this, true);
		}
	}

	// Tienes una tile en el este?
	if (adj_reales.Contains(SW)) {
		if (Super::SGI->mapManager->IsLogicPositionATile(SW) || Super::SGI->mapManager->IsLogicPositionAProp(SW)) {
			mapSkin_SW = APD_E_ElementActor::SGI->mapManager->MapInfo->roomByLogPos[SW]->mapSkin;
		}
		else
		{
			//this->CallFunctionByNameWithArguments(TEXT("BP_E_NotVisible"), debug, this, true);
		}
	}


	// Tienes una tile en el oeste?
	if (adj_reales.Contains(NW)) {
		if (Super::SGI->mapManager->IsLogicPositionATile(NW) || Super::SGI->mapManager->IsLogicPositionAProp(NW)) {
			mapSkin_NW = APD_E_ElementActor::SGI->mapManager->MapInfo->roomByLogPos[NW]->mapSkin;
		}
		else
		{
			//this->CallFunctionByNameWithArguments(TEXT("BP_W_NotVisible"), debug, this, true);
		}
	}






	// aplicar los skins a los elementos visibles

	this->CallFunctionByNameWithArguments(TEXT("BP_ChangeMaterialProperties_MapSkin"), debug, this, true);

}

