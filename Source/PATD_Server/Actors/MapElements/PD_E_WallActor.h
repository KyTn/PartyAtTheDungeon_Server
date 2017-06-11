// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Actors/PD_E_ElementActor.h"
#include "MapGeneration/PD_MG_LogicPosition.h"
#include "Structs/PD_ServerEnums.h"
#include "PD_E_WallActor.generated.h"



class UPD_ServerGameInstance;
class PD_MM_Room;

/**
 * 
 */
UCLASS()
class PATD_SERVER_API APD_E_WallActor : public APD_E_ElementActor
{
	GENERATED_BODY()

public:

	APD_E_WallActor();

	TArray<PD_MM_Room*> adjacentRooms; 




	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Material Type")
		MapSkinType mapSkin_N;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Material Type")
		MapSkinType mapSkin_S;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Material Type")
		MapSkinType mapSkin_E;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Material Type")
		MapSkinType mapSkin_W;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Rotated")
		bool rotated = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Thin Wall")
		bool isThinWall = false;

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// Called every frame
	virtual void Tick(float DeltaSeconds) override;


	void SetMaterialSkin(PD_MG_LogicPosition lp);


	void ChangeRotationToReal(PD_MG_LogicPosition lp);

private:
	void ChangeMaterialProperties_MapSkin(PD_MG_LogicPosition lp);


};
