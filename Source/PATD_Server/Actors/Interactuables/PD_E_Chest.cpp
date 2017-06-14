// Fill out your copyright notice in the Description page of Project Settings.

#include "PATD_Server.h"
#include "PD_E_Chest.h"
#include "Actors/PD_E_ElementActor.h"
#include "PD_ServerGameInstance.h"
#include "MapGeneration/PD_MG_LogicPosition.h"
#include "GM_Game/PD_GM_MapManager.h"
#include "MapInfo/PD_MM_MapInfo.h"

#include "Actors/Interactuables/PD_E_Interactuable.h"



// Sets default values
APD_E_Chest::APD_E_Chest()
{
	//Super::APD_E_Interactuable();
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void APD_E_Chest::BeginPlay()
{
	Super::BeginPlay();

}

// Called every frame
void APD_E_Chest::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}


void APD_E_Chest::Set_ChestInfo(TArray<APD_E_Interactuable*> otherInteractuables, PD_MM_InteractuableInfo* interInfo)
{

	UE_LOG(LogTemp, Log, TEXT("APD_E_Chest::Set_ChestInfo"));

	this->interacInfo = interInfo;
	this->ID_Interactuable = interacInfo->IDInteractuable;

	for (int i = 0; i < this->interacInfo->reactuables.Num(); i++) {
		for (APD_E_Interactuable* other : otherInteractuables) {
			if (other->ID_Interactuable == this->interacInfo->reactuables[i]) {
				ActivateThisReactorsWhenActive.Add(other);
				break;
			}
		}
	}

	InteractFromThisLogicPositions = this->interacInfo->logpos.GenerateAdjacents();

	ChangeRotationToClosestWall();
}


void APD_E_Chest::ChangeRotationToClosestWall() {

	FOutputDeviceDebug  debug;

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

	// si está pegada a una pared... 
	if (posibles.Num() != 0) {
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
}


// Se llamará a esta funcion para activar el interactuable
void APD_E_Chest::Interact(AActor* interactor, bool overwriteState) {

	if (IsCurrentlyActivated) {
		IsCurrentlyActivated = !IsCurrentlyActivated;
		InteractToDeactivate(interactor, overwriteState);
	}
	else {
		IsCurrentlyActivated = !IsCurrentlyActivated;
		InteractToActivate(interactor, overwriteState);
	}

}

void APD_E_Chest::InteractToActivate(AActor * interactor, bool overwriteState)
{
	if (isChestOpened) {
		return;
	}
	OpenChest();



	isChestOpened = true;

	for (APD_E_Interactuable* reactives : ActivateThisReactorsWhenActive) {
		if (reactives->IsCurrentlyActivated) {
			reactives->InteractToDeactivate(this, true);
		}
		else {
			reactives->InteractToActivate(this, true);
		}
	}
}


void APD_E_Chest::InteractToDeactivate(AActor* interactor, bool overwriteState) {

	CloseChest();

	for (APD_E_Interactuable* reactives : ActivateThisReactorsWhenActive) {
		if (reactives->IsCurrentlyActivated) {
			reactives->InteractToDeactivate(this, true);
		}
		else {
			reactives->InteractToActivate(this, true);
		}
	}
}





bool APD_E_Chest::OpenChest() {

}
bool APD_E_Chest::CloseChest() {

}