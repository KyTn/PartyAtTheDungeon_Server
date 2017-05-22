// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "MapGeneration/PD_MG_LogicPosition.h"
#include "Structs/PD_ServerEnums.h"
#include "PD_E_ElementActor.generated.h"




UCLASS()
class PATD_SERVER_API APD_E_ElementActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	APD_E_ElementActor();

	PD_MG_LogicPosition ActualLogicPosition;


	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	// Called every frame
	virtual void Tick( float DeltaSeconds ) override;

	//UPROPERTY(EditAnywhere, Category = "Material Type")
		MapSkinType mapSkin;

	UFUNCTION(BlueprintCallable, Category = "Element Actor")
	MapSkinType GetMaterialSkin() { return mapSkin; };

	UFUNCTION(BlueprintCallable, Category = "Element Actor")
	void SetMaterialSkin(MapSkinType inMaterialSkin) { mapSkin = inMaterialSkin; };
};
