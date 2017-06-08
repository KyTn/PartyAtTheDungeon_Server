// Fill out your copyright notice in the Description page of Project Settings.

#include "PATD_Server.h"
#include "PD_E_ElementActor.h"
#include "PD_ServerGameInstance.h"
#include "MapGeneration/PD_MG_LogicPosition.h"
#include "GM_Game/PD_GM_MapManager.h"
#include "MapInfo/PD_MM_MapInfo.h"

// Sets default values
APD_E_ElementActor::APD_E_ElementActor()
{
	//SGI = Cast<UPD_ServerGameInstance>(GetWorld()->GetGameInstance());
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void APD_E_ElementActor::BeginPlay()
{
	Super::BeginPlay();
	SGI = Cast<UPD_ServerGameInstance> (GetWorld()->GetGameInstance());
}

// Called every frame
void APD_E_ElementActor::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );

	FOutputDeviceDebug  debug;

	UE_LOG(LogTemp, Log, TEXT("APD_E_ElementActor::Tick - ActualLogicPosition (%d,%d)"), ActualLogicPosition.GetX(), ActualLogicPosition.GetY());


	if (SGI->mapManager->MapInfo->roomByLogPos.Contains(ActualLogicPosition)) {
		if (isActive != SGI->mapManager->MapInfo->roomByLogPos[ActualLogicPosition]->IsActive) {

			if (SGI->mapManager->MapInfo->roomByLogPos[ActualLogicPosition]->IsActive) {
				this->CallFunctionByNameWithArguments(TEXT("BP_ACTIVATE"), debug, this, true);
			}
			else {
				this->CallFunctionByNameWithArguments(TEXT("BP_DEACTIVATE"), debug, this, true);
			}

		}
	}
}



void APD_E_ElementActor::SetMaterialSkin(MapSkinType inMaterialSkin) { 
	
	mapSkin = inMaterialSkin; 

	ChangeMaterialProperties_MapSkin();

};



void APD_E_ElementActor::ChangeMaterialProperties_MapSkin() {
	
	FOutputDeviceDebug  debug;
	this->CallFunctionByNameWithArguments(TEXT("BP_ChangeMaterialProperties_MapSkin"), debug, this, true);
}

