// Fill out your copyright notice in the Description page of Project Settings.

#include "PATD_Server.h"
#include "math.h"
#include "PD_E_WallProp.h"
#include "Actors/PD_E_ElementActor.h"
#include "PD_ServerGameInstance.h"
#include "MapGeneration/PD_MG_LogicPosition.h"
#include "PD_E_WallActor.h"
#include "GM_Game/PD_GM_MapManager.h"
#include "MapInfo/PD_MM_MapInfo.h"

APD_E_WallProp::APD_E_WallProp() {

}

// Called when the game starts or when spawned
void APD_E_WallProp::BeginPlay()
{
	Super::BeginPlay();

}

// Called every frame
void APD_E_WallProp::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}


void APD_E_WallProp::SearchWallAndFix(PD_MG_LogicPosition lp) {
	FOutputDeviceDebug  debug;

	Super::ActualLogicPosition = lp;
	Super::SGI = Cast<UPD_ServerGameInstance>(GetWorld()->GetGameInstance());

	TArray<PD_MG_LogicPosition> adj_reales = Super::SGI->mapManager->Get_LogicPosition_Adyacents_To(Super::ActualLogicPosition);
	PD_MG_LogicPosition N = PD_MG_LogicPosition(Super::ActualLogicPosition.GetX() - 1, Super::ActualLogicPosition.GetY());
	PD_MG_LogicPosition S = PD_MG_LogicPosition(Super::ActualLogicPosition.GetX() + 1, Super::ActualLogicPosition.GetY());
	PD_MG_LogicPosition E = PD_MG_LogicPosition(Super::ActualLogicPosition.GetX(), Super::ActualLogicPosition.GetY() + 1);
	PD_MG_LogicPosition W = PD_MG_LogicPosition(Super::ActualLogicPosition.GetX(), Super::ActualLogicPosition.GetY() - 1);

	TArray<PD_MG_LogicPosition> posibles = TArray<PD_MG_LogicPosition>();
	UE_LOG(LogTemp, Log, TEXT("APD_E_WallProp::SearchWallAndFix"));


	// Generamos las posibles posiciones a donde puede apuntar el prop

	if (adj_reales.Contains(N) && Super::SGI->mapManager->IsLogicPositionAWall(N)) {
		posibles.Add(N);
	}

	if (adj_reales.Contains(S) && Super::SGI->mapManager->IsLogicPositionAWall(S)) {
		posibles.Add(S);
	}

	if (adj_reales.Contains(W) && Super::SGI->mapManager->IsLogicPositionAWall(W)) {
		posibles.Add(W);
	}

	if (adj_reales.Contains(E) && Super::SGI->mapManager->IsLogicPositionAWall(E)) {
		posibles.Add(E);
	}

	// si no hay ninguna, pues se desactiva el render del mesh

	if (posibles.Num() == 0) {
		//this->CallFunctionByNameWithArguments(TEXT("BP_Not_Visible"), debug, this, true);
		return;
	}
	// si hay posibles, dame uno

	int r = FMath::RandRange(0, posibles.Num() - 1);

	if (posibles[r] == N) {
		this->CallFunctionByNameWithArguments(TEXT("BP_Rotate_180"), debug, this, true);
	}
	else if (posibles[r] == S) {
		//this->CallFunctionByNameWithArguments(TEXT("BP_Rotate_180"), debug, this, true);
	}
	else if (posibles[r] == W) {
		this->CallFunctionByNameWithArguments(TEXT("BP_Rotate_90"), debug, this, true);
	}
	else if (posibles[r] == E) {
		this->CallFunctionByNameWithArguments(TEXT("BP_Rotate_270"), debug, this, true);
	}
	

	
}
