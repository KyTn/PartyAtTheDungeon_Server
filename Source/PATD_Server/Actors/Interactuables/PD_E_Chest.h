// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Actors/Interactuables/PD_E_Interactuable.h"
#include "MapGeneration/PD_MG_LogicPosition.h"
#include "MapInfo/PD_MM_MapInfo.h"
#include "PD_E_Chest.generated.h"



class PD_MM_Room;
class PD_MM_InteractuableInfo;

/**
 * 
 */
UCLASS()
class PATD_SERVER_API APD_E_Chest : public APD_E_Interactuable
{
	GENERATED_BODY()

public:

		APD_E_Chest();

	bool isChestOpened = false;
	PD_MM_InteractuableInfo* interacInfo;


	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// Called every frame
	virtual void Tick(float DeltaSeconds) override;

	// Guarda una referencia a su interInfo e inicializa las variables con su info 
	virtual void Set_ChestInfo(TArray<APD_E_Interactuable*> otherInteractuables, PD_MM_InteractuableInfo* interInfo);

	virtual void Interact(AActor* interactor, bool overwriteState = false);

	// Se llamará a esta funcion para activar el interactuable
	virtual void InteractToActivate(AActor* interactor, bool overwriteState = false);

	// Se llamará a esta función para desactivar el interactuable
	virtual void InteractToDeactivate(AActor* interactor, bool overwriteState = false);


	void ChangeRotationToClosestWall();

	UFUNCTION(BlueprintCallable, Category = "OpenChest")
		bool OpenChest();
	UFUNCTION(BlueprintCallable, Category = "CloseChest")
		bool CloseChest();
	
	
};
